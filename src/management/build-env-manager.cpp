#include <feather/feather.h>
#include <feather/management/build-env-manager.h>
#include <feather/utils.h>

#include <string>

using namespace std;

namespace feather {

ProblemManager* buildEnvManager(bool logging) {
	if((getenv("MANAGER")) == NULL) FEATHER_THROW("MANAGER not set");
	string manager(getenv("MANAGER"));

	/* asking for SocketClient? */
	string tmp("sockets,");
	if(std::equal(tmp.begin(), tmp.end(), manager.begin())) {
		std::vector<SolverAddress*> addresses;
		int prev = tmp.size();
		for(std::string::const_iterator i = manager.begin() + tmp.size(); i <= manager.end(); ++i) {
			if( *i == ',') {
				int index = i-manager.begin();
				string addr = manager.substr(prev, index-prev);
				addresses.push_back(new SolverAddress(addr));
				prev = index+1;
			}
		}
		string addr = manager.substr(prev, manager.size()-prev);
		addresses.push_back(new SolverAddress(addr));

		std::cout << "Using a SocketClient with " << addresses.size() << " hosts." << std::endl;
		return new SocketClient(addresses);
	}
	return buildChildManager(logging);
}

ChildManager* buildChildManager(bool logging) {
	if((getenv("MANAGER")) == NULL) FEATHER_THROW("MANAGER not set");
	string manager(getenv("MANAGER"));

	/* asking for a bare naxos? */
	if(manager == "naxos") {
		if(logging) std::cout << "Using a bare Naxos manager." << std::endl;
		return new Naxos();
	}
	/* asking for a threadmanager? */
	string tmp("threads");
	if(std::equal(tmp.begin(), tmp.end(), manager.begin())) {
		string threads = manager.substr(tmp.size(), manager.size());
		int nthreads = stoi(threads);
		if(logging) std::cout << "Using a ThreadManager with " << nthreads << " threads." << std::endl;
		return new ThreadManager(new NaxosGenerator(), nthreads, 1000);
	}
	FEATHER_THROW("Unknown manager: " << manager);
}

} // namespace feather
