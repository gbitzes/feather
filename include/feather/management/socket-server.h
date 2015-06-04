#ifndef FEATHER_MANAGEMENT_SOCKETSERVER_H
#define FEATHER_MANAGEMENT_SOCKETSERVER_H

#include <feather/common/problem-manager.h>
#include <feather/management/manager-generator.h>
#include <queue>
#include <semaphore.h>

namespace feather { 

namespace SocketServerState {
	enum State {
		UNINITIALIZED = 0,
		INITIALIZED_NOT_SEARCHING,
		INITIALIZED_SEARCHING,
		INITIALIZED_SEARCH_FINISHED
	};
};

class Representation;
class SocketServer : public ParentManager {
    private:
        SocketServerState::State state;
		Representation const* representation;
		ChildGenerator *generator;
		Int minObjValue;
        ChildManager* child; 
        bool needwork;
        Representation* receiveRepresentation();
        std::vector<bool> receiveDecisions();
        int fd;
        FILE *in, *out;
		pthread_mutex_t socketWriteMutex;
		pthread_mutex_t minObjMutex;

        void safeWrite(char*);
        void handleRound();

		  pthread_mutex_t vaultMutex;
		  sem_t vaultSem;
        std::vector< std::map<Int, IntDomain* >* > solutionvault;
		  bool roundFinished;
    public:
		SocketServer(ChildManager* child, Int loggingLevel = 0);
		~SocketServer();

        void monitorIncoming();
		virtual void supplyRepresentation(const Representation &);
		virtual void clearRepresentation();

		/* Caller responsible to delete given object */
		virtual IntDomain* getDomain(IntVarID);

		virtual Int getMinObjValue();
		virtual void updateMinObjValue(Int);
		virtual void newInstance(std::vector<bool> decisions);
		virtual bool needMoreWork();
        virtual bool nextSolution();
        virtual void restart();

        void setNeedMoreWork(bool);
        void handleSession(int);
        void solveRound();

		  void collectSolutions();
};

}

#endif
