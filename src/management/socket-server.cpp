#include <feather/management/socket-server.h>
#include <feather/utils.h>
#include <common/serializer.h>
#include <unistd.h>
#include <poll.h>

#define BUFLEN 10000

namespace feather {

SocketServer::SocketServer(ChildManager* child, Int loggingLevel) {
    this->child = child;
    child->setParent(this);
    state = SocketServerState::UNINITIALIZED;

	pthread_mutex_init(&socketWriteMutex, NULL);
}
SocketServer::~SocketServer() {
}

void SocketServer::supplyRepresentation(const Representation &representation) {
	if(state != SocketServerState::UNINITIALIZED)
		FEATHER_THROW("SocketServer has already been initialized!");

	this->representation = &representation;
	state = SocketServerState::INITIALIZED_NOT_SEARCHING;
    needwork = true;

    child->supplyRepresentation(representation);
}

void SocketServer::clearRepresentation() {
    this->representation = NULL;
}

IntDomain* SocketServer::getDomain(IntVarID var) {
	return child->getDomain(var);
}

Int SocketServer::getMinObjValue() {
	return minObjValue;
}

void SocketServer::updateMinObjValue(Int newBestValue) {
	 if(newBestValue < minObjValue)
	 	minObjValue = newBestValue;
}

bool SocketServer::needMoreWork() {
    return true;
    return needwork;
}

void SocketServer::setNeedMoreWork(bool val) {
    needwork = val;
}

void SocketServer::newInstance(std::vector<bool> decisions) {
    needwork = false;
	pthread_mutex_lock(&socketWriteMutex);
    fprintf(out, "DONATION ");
    std::cout << "donating.. ";
    for(int i = 0; i < decisions.size(); i++) {
        if(decisions[i] == true) fprintf(out, "1");
        if(decisions[i] == false) fprintf(out, "0");
        std::cout << decisions[i];
    }
    std::cout << std::endl;
    fprintf(out, "\n");
    fflush(out);
    pthread_mutex_unlock(&socketWriteMutex);
}

bool SocketServer::nextSolution() {
    return child->nextSolution();
}

Representation* SocketServer::receiveRepresentation() {
    char buf[BUFLEN];

    std::stringstream r;
    while(1) {
        fgets(buf, BUFLEN-1, in);
        std::string tmp(buf);
        tmp = tmp.substr(0, tmp.length()-1);

        r << tmp << std::endl;
        std::cout << "I got " << tmp << std::endl;
        if(tmp == "END REPRESENTATION") 
            break;
    }

    RepresentationSerializer serializer;
    std::string srepr = r.str();
    Representation *representation = serializer.deserialize(srepr);
    std::cout << "Received representation" << std::endl;
    return representation;
}

std::vector<bool> SocketServer::receiveDecisions() {
    fprintf(out, "GIVE DECISIONS\n");
    fflush(out);

    char buf[BUFLEN];
    fgets(buf, BUFLEN-1, in);
    std::vector<bool> decisions;
    for(int i = 0; i < BUFLEN && buf[i] != '\n'; i++) {
        if(buf[i] == '0')
            decisions.push_back(false);
        else if(buf[i] == '1')
            decisions.push_back(true);
        else {
            std::cout << "ERROR: received " << std::string(buf) << std::endl;
            std::cout << "bailing out" << std::endl;
            exit(2);
        }
    }

    std::cout << "Received decisions" << std::endl;
    for(int i = 0; i < decisions.size(); i++) std::cout << decisions[i];
    std::cout << std::endl;
    return decisions;
}

void SocketServer::safeWrite(char* msg) {
	pthread_mutex_lock(&socketWriteMutex);
    fprintf(out, msg);
    pthread_mutex_unlock(&socketWriteMutex);
}

namespace {
void* spawn_thread(void *args) {
    SocketServer *ss = (SocketServer*) args;
    ss->solveRound();
} }

void SocketServer::monitorIncoming() {
    char buf[BUFLEN];
    while(1) {
        char *res = fgets(buf, BUFLEN-1, in);
        /* Time to quit? */
        if(res == NULL) {
            std::cout << "Error when reading" << std::endl;
            exit(3);
            break;
        }

        std::stringstream ss(buf);
        std::string command;
        ss >> command;

        if(command == "UPDATEOBJ") {
            Int objval;
            ss >> objval;
            std::cout << "Received updated objective value: " << objval << std::endl;
            updateMinObjValue(objval);
        }
        else if(command == "NEEDWORK") {
            setNeedMoreWork(true);
        }
        else if(command == "RESTART") {
            break;
        }
        else if(command == "EXIT") {
            close(this->fd);
            break;
        }
        else {
            std::cout << "Received unrecognized command, " << command << ". Bailing out" << std::endl;
            exit(2);
        }
    }
    std::cout << "monitorIncoming: round finished" << std::endl;
}

void SocketServer::restart() {
    FEATHER_THROW("NIY");
}

void SocketServer::solveRound() {
    int nsolutions = 0;
    while(nextSolution()) {
	    pthread_mutex_lock(&socketWriteMutex);
        nsolutions++;
        fprintf(out, "SOLUTION\n");
        for(int i = 0; i < representation->vars.size(); i++) {
            fprintf(out, "%d ", representation->vars[i].id);
            IntDomain *domain = getDomain(representation->vars[i].id);
            fprintf(out, "%s\n", domain->toString().c_str());
            delete domain;
        }
        fprintf(out, "END SOLUTION\n");
        fflush(out);
        pthread_mutex_unlock(&socketWriteMutex);
    }
    fprintf(out, "NO-MORE-SOLUTIONS\n");
    fflush(out);
}

void SocketServer::handleRound() {
    /* Get decisions */
    std::vector<bool> decisions = receiveDecisions();
    child->setInitialDecisions(decisions);

    /* Start search */
    fprintf(out, "SEARCHING\n");
    fflush(out);

    /* Start solving */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t handle;

	pthread_create(&handle, &attr, spawn_thread, this);
	pthread_attr_destroy(&attr);

    /* Careful now.. there might be multiple threads in Child,
     * which might attempt to do socket-writing things
     * (such as creating new instances).
     *
     * All writes to the socket from this point on need to be
     * protected by mutex
     */

    setNeedMoreWork(false);
    monitorIncoming();
}

void SocketServer::handleSession(int fd) {
    /* Use FILE streams for convenience */
    out = fdopen(fd, "w");
    in = fdopen(fd, "r");
    this->fd = fd;

    /* Initialize protocol */
    fprintf(out, "CSP SOLVER READY\n");
    fflush(out);

    /* Read representation */
    Representation *repr = receiveRepresentation();
    supplyRepresentation(*repr);

    while(1) {
        handleRound();
        child->restart();
    }
}

}
