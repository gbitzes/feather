#include <feather/management/thread-manager.h>
#include <feather/utils.h>
#include <common/representation.h>

namespace feather {

namespace {

struct arg_struct {
	ThreadManager *threadmanager;
	ChildManager *pm;
	ProblemManagerID id;
};

void* spawn_thread(void *arguments) {
	arg_struct *args = (arg_struct*) arguments;
	args->threadmanager->runThread(*(args->pm), args->id);
    delete args->pm;
	free(arguments);
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

} // anonymous namespace

ThreadManager::ThreadManager(ChildGenerator *generator, Int target, Int loggingLevel) {
	this->generator = generator;
	this->loggingLevel = loggingLevel;
    parent = NULL;
	targetThreads = target;
    activeThreads = 0;
    minObjValue = kPlusInf;
    prevObjValue = kPlusInf;

	representation = NULL;
    cleanup();
	this->log(10, "ThreadManager constructed");
}

void ThreadManager::cleanup() {
    activeManager = NULL;
	pthread_mutex_init(&objValueMutex, NULL);
	pthread_mutex_init(&pendingSolutionsMutex, NULL);
	pthread_mutex_init(&loggingMutex, NULL);
	pthread_mutex_init(&newInstanceMutex, NULL);
	pthread_mutex_init(&activeThreadsMutex, NULL);

	this->state = ThreadManagerState::UNINITIALIZED;
	sem_init(&pending, 0, 0);
}

void ThreadManager::restart() {
    if(state != ThreadManagerState::INITIALIZED_SEARCH_FINISHED)
        FEATHER_THROW("Cannot do a restart at this point");
    cleanup();
    state = ThreadManagerState::INITIALIZED_NOT_SEARCHING;
}

void ThreadManager::supplyRepresentation(const Representation &representation) {
	if(state != ThreadManagerState::UNINITIALIZED)
		FEATHER_THROW("ThreadManager has already been initialized!");

	this->representation = &representation;
	state = ThreadManagerState::INITIALIZED_NOT_SEARCHING;
}

void ThreadManager::clearRepresentation() {
	this->representation = NULL;
}

ThreadManager::~ThreadManager() {
	/* Cleanup */
	for(int i = 0; i < threads.size(); i++ ) {
		delete threads[i].handle;
		delete threads[i].waiting;
		delete threads[i].pm;
	}
}

void ThreadManager::updateMinObjValue(Int newBestValue) {
    if(parent) {
        parent->updateMinObjValue(newBestValue);
        return;
    }
	
    pthread_mutex_lock(&objValueMutex);
    std::cout << "min obj value updated to " << newBestValue << std::endl;

	/* It's possible that due to races, the given newBestValue is
	 * not actually better compared to what I have already. Maybe
	 * a new thread found a better solution and managed to acquire
	 * the mutex first. I still need to check if it's actually better 
	 */

	 if(newBestValue < minObjValue)
	 	minObjValue = newBestValue;

	pthread_mutex_unlock(&objValueMutex);
}

IntDomain* ThreadManager::getDomain(IntVarID var) {
	FEATHER_ASSERT(activeManager != NULL);
	return activeManager->getDomain(var);
}

Int ThreadManager::getMinObjValue() {
    if(parent) return parent->getMinObjValue();
	return minObjValue;
}

void ThreadManager::setInitialDecisions(std::vector<bool> decisions) {
	if(state != ThreadManagerState::UNINITIALIZED && state != ThreadManagerState::INITIALIZED_NOT_SEARCHING)
		FEATHER_THROW("Cannot set initial decisions at this point");

    this->decisions = decisions;
}

void ThreadManager::setParent(ParentManager *parent) {
    if(state != ThreadManagerState::UNINITIALIZED && state != ThreadManagerState::INITIALIZED_NOT_SEARCHING)
		FEATHER_THROW("Cannot set initial decisions at this point");

    this->parent = parent;
}

bool ThreadManager::needMoreWork() {
	return activeThreads < targetThreads || (parent != NULL && parent->needMoreWork());
}

void ThreadManager::log(int level, std::string msg) {
	if(level <= loggingLevel) {
		pthread_mutex_lock(&loggingMutex);
		std::stringstream ss;
		ss << currentDateTime() << " [" << pthread_self() << "] " << msg << std::endl;
		std::cerr << ss.str();
		pthread_mutex_unlock(&loggingMutex);
	}
}

void ThreadManager::runThread(ChildManager &pm, ProblemManagerID id) {
	this->log(3, STR("New thread spawned with id = " << id << ". Active threads: " << activeThreads));

	pm.setParent(this);
	pm.supplyRepresentation(*representation);

	/* Let's get to work.. */
	Int nsolutions = 0;
	while( pm.nextSolution() ) {

		/* 
		 * We have a solution.. Add me to the pending
		 * solutions
		 */

		nsolutions++;
		pthread_mutex_lock(&pendingSolutionsMutex);
		pendingSolutions.push(id);
		pthread_mutex_unlock(&pendingSolutionsMutex); 
	
		/* Wake up the main thread.. */
		sem_post(&pending);

		/* Sleep until the main thread has processed my solution */
		sem_wait(threads[id].waiting);
	}

	/* No more solutions from me.. has the search ended? */
	Int n = addToActiveThreads(-1);
	this->log(3, STR("#" << id << ": Terminating after finding " << nsolutions << " solutions - active threads " << n));
	if(n == 0) {
		sem_post(&pending);
	}

}

Int ThreadManager::getActiveThreads() {
	return activeThreads;
}

bool ThreadManager::nextSolution() {
	if(state == ThreadManagerState::UNINITIALIZED)
		FEATHER_THROW("ThreadManager not initialized!");
	if(state == ThreadManagerState::INITIALIZED_SEARCH_FINISHED)
		return false;
	if(state == ThreadManagerState::INITIALIZED_NOT_SEARCHING) {
		/* Launch the first thread */
		newInstance(decisions);
		state = ThreadManagerState::INITIALIZED_SEARCHING;
	}
	FEATHER_ASSERT(representation != NULL);

	/* The thread of the previous solution is still waiting -
	 * need to tell it to start searching
	 */
	if(activeManager != NULL) {
		activeManager = NULL;
		sem_post(threads[activeID].waiting);
	}

	/* Wait until a solution is available */
	sem_wait(&pending);

	/* Has the search finished? */
	if(activeThreads == 0) {
		state = ThreadManagerState::INITIALIZED_SEARCH_FINISHED;
		return false;
	}

	/* No - pass on the solution */
	pthread_mutex_lock(&pendingSolutionsMutex);
	activeID = pendingSolutions.front();
	activeManager = threads[activeID].pm;
	pendingSolutions.pop();
	pthread_mutex_unlock(&pendingSolutionsMutex);

    /* Worse solution due to races? */
    if(representation->minObj != -1) {
        IntDomain *obj = getDomain(representation->minObj);
        Int objval = obj->max();
        delete obj;
        if(objval > prevObjValue) return nextSolution(); /* discard solution */
        prevObjValue = objval;
    }

	return true;
}

Int ThreadManager::addToActiveThreads(Int n) {
	pthread_mutex_lock(&activeThreadsMutex);
	activeThreads += n;
	Int ret = activeThreads;
	pthread_mutex_unlock(&activeThreadsMutex);
	return ret;
}

void ThreadManager::newInstance(std::vector<bool> decisions) {
    /* Parent needs work? */
    if(parent != NULL && state == ThreadManagerState::INITIALIZED_SEARCHING && parent->needMoreWork()) {
        if(loggingLevel >= 2) {
            std::stringstream ss;
            for(int i = 0; i < decisions.size(); i++)
                ss << decisions[i];
            this->log(2, "Donating " + ss.str() + " to parent");
        }
        parent->newInstance(decisions);
        return;
    }

	addToActiveThreads(+1);
	pthread_mutex_lock(&newInstanceMutex);

	ThreadInfo info;
	info.handle = new pthread_t;
	info.waiting = new sem_t;
	sem_init(info.waiting, 0, 0);

	/* Detached thread attribute */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	ChildManager *pm = generator->generate();
	pm->setInitialDecisions(decisions);
	info.pm = pm;
	threads.push_back(info);

	struct arg_struct *arguments = new arg_struct();
	arguments->threadmanager = this;
	arguments->pm = pm;
	arguments->id = threads.size()-1;

    if(loggingLevel >= 2) {
        std::stringstream ss;
        for(int i = 0; i < decisions.size(); i++)
	       	ss << decisions[i];
	    this->log(2, "About to create new thread with decisions = " + ss.str() );
    }

	pthread_create(info.handle, &attr, spawn_thread, arguments);
	pthread_attr_destroy(&attr);

	pthread_mutex_unlock(&newInstanceMutex);
}

} // namespace feather
