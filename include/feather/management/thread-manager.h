#ifndef FEATHER_MANAGEMENT_THREADMANAGER_H
#define FEATHER_MANAGEMENT_THREADMANAGER_H

#include <feather/common/problem-manager.h>
#include <feather/management/manager-generator.h>
#include <queue>
#include <semaphore.h>

namespace feather {

class Representation;

namespace ThreadManagerState {
	enum State {
		UNINITIALIZED = 0,
		INITIALIZED_NOT_SEARCHING,
		INITIALIZED_SEARCHING,
		INITIALIZED_SEARCH_FINISHED
	};
};

typedef int ProblemManagerID;

struct ThreadInfo {
	ProblemManager *pm;
	pthread_t *handle;
	sem_t *waiting;
};

class ThreadManager : public IntermediateManager {
	private:
		ThreadManagerState::State state;

		Int loggingLevel;
		Int minObjValue;
		Int activeThreads;
		Int targetThreads;

		Representation const* representation;
		ChildGenerator *generator;

		std::vector<ThreadInfo> threads;
        std::vector<bool> decisions;
        ParentManager* parent;

		// std::vector<ProblemManager*> pms;
		// std::vector<pthread_t*> threads;
		// std::vector<pthread_t*> threads;


		std::queue<ProblemManagerID> pendingSolutions;
		sem_t pending;

		/* 
		 * If not null, we've currently found a solution and supplying
		 * it upstream. This is the downstream manager who has found it 
		 */
		ProblemManager* activeManager;
		ProblemManagerID activeID;

		pthread_mutex_t objValueMutex, pendingSolutionsMutex, loggingMutex;
		pthread_mutex_t newInstanceMutex, activeThreadsMutex;

        void cleanup();
	public:
		ThreadManager(ChildGenerator*, Int target, Int loggingLevel = 0);
		~ThreadManager();

		virtual void supplyRepresentation(const Representation &);
		virtual void clearRepresentation();
		virtual bool nextSolution();

		/* Caller responsible to delete given object */
		virtual IntDomain* getDomain(IntVarID);

		virtual Int getMinObjValue();
		virtual void updateMinObjValue(Int);
		virtual void newInstance(std::vector<bool> decisions);
		virtual void setInitialDecisions(std::vector<bool> decisions);
        virtual void setParent(ParentManager*);
		virtual bool needMoreWork();
        virtual void restart();

		void runThread(ChildManager &pm, ProblemManagerID id);
		void log(int, std::string);
		Int getActiveThreads();
		Int addToActiveThreads(Int);
};

} // namespace feather

#endif
