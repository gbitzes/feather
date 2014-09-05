#ifndef FEATHER_MANAGEMENT_SOCKETCLIENT_H
#define FEATHER_MANAGEMENT_SOCKETCLIENT_H

#include <feather/common/problem-manager.h>
#include <feather/management/manager-generator.h>
#include <queue>
#include <semaphore.h>
#include <map>

namespace feather { 

struct SolverAddress {
    std::string hostname;
    std::string port;
    int fd;
    bool currentlyBusy;
    FILE *in, *out;
    SolverAddress(std::string host_, std::string port_) : hostname(host_), port(port_) {
        currentlyBusy = false;
    }
};

namespace SocketClientState {
    enum State {
        UNINITIALIZED = 0,
        INITIALIZED_NOT_SEARCHING,
        INITIALIZED_SEARCHING,
        INITIALIZED_SEARCH_FINISHED
    };  
};

class Representation;
class SocketClient : public ProblemManager {
    private:
        SocketClientState::State state;
        Representation const* representation;
        ChildGenerator *generator;
        Int minObjValue;
        std::string serialization;
        const char *serializationBuf;


        int establishConnection(const SolverAddress&);
        int activeServers;
        std::vector<SolverAddress> servers;

        std::queue<SolverAddress> waitingServers;
        std::queue<std::vector<bool> > waitingJobs;
        pthread_mutex_t waitingMutex;
        pthread_mutex_t solutionMutex;
        pthread_mutex_t activeServersMutex;

        std::queue<std::map<IntVarID, IntDomain* > > solutions;
        sem_t pendingSolutions;

        void initializeSessions();
        void broadcastRepresentation(const SolverAddress&);
        void broadcastNeedMoreWork();
        void dispatchWork();

        std::map<IntVarID, IntDomain* > currentSolution;
        
    public:
        void monitorServer();
        SocketClient(std::vector<SolverAddress>);
		virtual void supplyRepresentation(const Representation &);
		virtual void clearRepresentation();
		virtual bool nextSolution();
        virtual void restart();

		/* Caller responsible to delete given object */
		virtual IntDomain* getDomain(IntVarID);
};
}
#endif
