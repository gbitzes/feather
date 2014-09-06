#include <feather/management/socket-client.h>
#include <common/serializer.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <common/util.h>

#include <feather/utils.h>

#define BUFLEN 1000

namespace feather {

SocketClient::SocketClient(std::vector<SolverAddress> addresses) {
    this->servers = addresses;
    state = SocketClientState::UNINITIALIZED;
    sem_init(&pendingSolutions, 0, 0);
    activeServers = 0;

    pthread_mutex_init(&solutionMutex, NULL);
    pthread_mutex_init(&activeServersMutex, NULL);
    pthread_mutex_init(&waitingMutex, NULL);
}

void SocketClient::supplyRepresentation(const Representation &repr) {
    this->representation = &repr;
    std::cout << "SocketClient - received representation" << std::endl;
    RepresentationSerializer ser;
    serialization = ser.serialize(repr); 
    serializationBuf = serialization.c_str();


    state = SocketClientState::INITIALIZED_NOT_SEARCHING;
}

void SocketClient::clearRepresentation() {
    this->representation = NULL;
    state = SocketClientState::UNINITIALIZED;
}

void SocketClient::broadcastRepresentation(const SolverAddress &address) {
   char buf[BUFLEN];
   char *res = fgets(buf, BUFLEN-1, address.in); 
   std::string tmp(res);
   tmp = tmp.substr(0, tmp.length()-1);
   std::cout << "I received " << tmp << std::endl;
   if(tmp != "CSP SOLVER READY") {
       std::cout << "Protocol error - expected to receive 'CSP SOLVER READY', got " << tmp << " instead." << std::endl;
       exit(3);
   }

   /* send serialization */
   fprintf(address.out, "%s", serializationBuf);
   fflush(address.out);
}

/* Returns the fd */
int SocketClient::establishConnection(const SolverAddress &address) {
    struct addrinfo hints, *servinfo, *p;
    int rv, sockfd;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if((rv = getaddrinfo(address.hostname.c_str(), address.port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    //inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    //printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
    return sockfd;
}

void SocketClient::initializeSessions() {
    for(int i = 0; i < servers.size(); i++) {
        int fd = establishConnection(servers[i]);
        servers[i].fd = fd;
        servers[i].in = fdopen(fd, "r");
        servers[i].out = fdopen(fd, "w");

        broadcastRepresentation(servers[i]);
        waitingServers.push(servers[i]);
    }
}

void SocketClient::broadcastNeedMoreWork() {
    //needsWork = true;
    for(int i = 0; i < servers.size(); i++) {
        if(servers[i].currentlyBusy) {
            fprintf(servers[i].out, "NEEDWORK\n");
            fflush(servers[i].out);
        }
    }
}

namespace {

struct thread_args {
    SocketClient *sc;
    SolverAddress s;
    std::vector<bool> decisions;
};

void* spawn_thread(void *args) {
    std::cout << "spawning thread" << std::endl;
    thread_args *ta = (thread_args*) args;
    ta->sc->monitorServer(ta->s, ta->decisions);
    delete ta;
} 

}

void SocketClient::monitorServer(SolverAddress &s, std::vector<bool> &decisions) {
    char buf[BUFLEN];
    // pthread_mutex_lock(&waitingMutex);
    // SolverAddress s = waitingServers.front();
    // waitingServers.pop();
    //
    // std::vector<bool> decisions = waitingJobs.front();
    // waitingJobs.pop();
    // pthread_mutex_unlock(&waitingMutex);
    //
    pthread_mutex_lock(&activeServersMutex);
    activeServers++;
    pthread_mutex_unlock(&activeServersMutex);

    std::cout << "in monitor server" << std::endl;

    /* Make sure server state is sane */
    { char *res = fgets(buf, BUFLEN-1, s.in); 
    std::string tmp(res);
    tmp = tmp.substr(0, tmp.length()-1);
    if(tmp != "GIVE DECISIONS") {
        std::cout << "Protocol error - expected 'GIVE DECISIONS', received " << tmp << std::endl;
        exit(3);
    } }

    /* Send decisions */
    for(int i = 0; i < decisions.size(); i++) {
        int val = 0;
        if(decisions[i] == true) val = 1;
        fprintf(s.out, "%d", val);
    }
    fprintf(s.out, "\n");
    fflush(s.out);

    std::cout << "Decisions sent" << std::endl;

    /* Make sure server state is sane */
    { char *res = fgets(buf, BUFLEN-1, s.in); 
    std::string tmp(res);
    tmp = tmp.substr(0, tmp.length()-1);
    if(tmp != "SEARCHING") {
        std::cout << "Protocol error - expected 'GIVE DECISIONS', received " << tmp << std::endl;
        exit(3);
    } }
    
    /* Collect solutions and/or other messages */
    while(1) {
        char* res = fgets(buf, BUFLEN-1, s.in);
        std::stringstream ss(res);
        std::string msg;
        ss >> msg;

        if(msg == "SOLUTION") {
            std::map<IntVarID, IntDomain*> solution;

            while(1) {
                IntVarID id;
                char *res = fgets(buf, BUFLEN-1, s.in);
                std::stringstream ss(res);
                std::string tmp(res);
                tmp = tmp.substr(0, tmp.length()-1);
                if(tmp == "END SOLUTION") break;
                ss >> id;
                ss >> tmp;
                // std::cout << "got domain for " << id << std::endl;
                // std::cout << "got " << ss.str() << std::endl;
                // std::cout << "-" << tmp << "-" << std::endl;
                IntDomain *domain = deserializeDomain(tmp);
                solution[id] = domain;
            }
            pthread_mutex_lock(&solutionMutex);
            solutions.push(solution);
            pthread_mutex_unlock(&solutionMutex);
            sem_post(&pendingSolutions);
        }
        else if(msg == "NO-MORE-SOLUTIONS") {
            fprintf(s.out, "RESTART\n");
            fflush(s.out);

            pthread_mutex_lock(&waitingMutex);
            waitingServers.push(s);
            pthread_mutex_unlock(&waitingMutex);

            dispatchWork();

            pthread_mutex_lock(&activeServersMutex);
            pthread_mutex_lock(&waitingMutex);
            activeServers--;
            if(activeServers == 0 && waitingJobs.size() == 0) {
                state = SocketClientState::INITIALIZED_SEARCH_FINISHED;
                sem_post(&pendingSolutions);
                std::cout << "Search has finished!!" << std::endl;
            }
            pthread_mutex_unlock(&waitingMutex);
            pthread_mutex_unlock(&activeServersMutex);

            return;
        }
        else if(msg == "DONATION") {
            std::vector<bool> donation;
            for(int i = 9; i < BUFLEN && buf[i] != '\n'; i++) {
                if(buf[i] == '0')
                    donation.push_back(false);
                else if(buf[i] == '1')
                    donation.push_back(true);
                else {
                    std::cout << "ERROR: received " << std::string(buf) << std::endl;
                    std::cout << "bailing out" << std::endl;
                    exit(2);
                }
            }
            std::cout << "received donation: ";
            for(int i = 0; i < donation.size(); i++) std::cout << donation[i];
            std::cout << std::endl;
            
            pthread_mutex_lock(&waitingMutex);
            waitingJobs.push(donation);
            std::cout << "size of waiting jobs: " << waitingJobs.size() << std::endl;
            pthread_mutex_unlock(&waitingMutex);
        }
        else {
            std::cout << "WARNING: unrecognized message " << msg << std::endl;
        }
    }
}

void SocketClient::restart() {
    FEATHER_THROW("NIY");
}

void SocketClient::dispatchWork() {
    pthread_mutex_lock(&waitingMutex);
    if(waitingServers.size() > waitingJobs.size())
        broadcastNeedMoreWork();

    int dispatch = waitingServers.size();
    if(dispatch > waitingJobs.size()) dispatch = waitingJobs.size();
    std::cout << "dispatch = " << dispatch << std::endl;
    for(int i = 0; i < dispatch; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_t handle;

        thread_args *ta = new thread_args();
        ta->sc = this;
        ta->decisions = waitingJobs.front();
        waitingJobs.pop();
        ta->s = waitingServers.front();
        waitingServers.pop();
        
        pthread_create(&handle, &attr, spawn_thread, ta);
        pthread_attr_destroy(&attr);
    }
    pthread_mutex_unlock(&waitingMutex);
}

bool SocketClient::nextSolution() {
    if(state == SocketClientState::INITIALIZED_SEARCH_FINISHED)
        return false;
    if(state == SocketClientState::INITIALIZED_NOT_SEARCHING) {
        /* Establish all connections */
        initializeSessions();

        /* Fire up the first */
        {
            std::vector<bool> empty;
            empty.push_back(0);
            empty.push_back(1);
            waitingJobs.push(empty);
        }
        {
            std::vector<bool> empty;
            empty.push_back(0);
            empty.push_back(0);
            waitingJobs.push(empty);
        }
        {
            std::vector<bool> empty;
            empty.push_back(1);
            empty.push_back(0);
            waitingJobs.push(empty);
        }
        {
            std::vector<bool> empty;
            empty.push_back(1);
            empty.push_back(1);
            waitingJobs.push(empty);
        }


        dispatchWork();
        state = SocketClientState::INITIALIZED_SEARCHING;
    }
    
    sem_wait(&pendingSolutions);
    if(state == SocketClientState::INITIALIZED_SEARCH_FINISHED) {
        return false;
    }
    
    pthread_mutex_lock(&solutionMutex);
    currentSolution = solutions.front();
    solutions.pop();
    pthread_mutex_unlock(&solutionMutex);
    return true;
}

IntDomain* SocketClient::getDomain(IntVarID id) {
    return currentSolution[id];
}

}