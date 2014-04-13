#ifndef FEATHER_BACKEND_NAXOS_BASICS_H
#define FEATHER_BACKEND_NAXOS_BASICS_H

namespace feather {


struct Ns_HistoryId_t {
	
	/* The depth of the node in the search tree */
	UInt level;
	
	/* The identity of the search node */
	UInt id;
	
	Ns_HistoryId_t() { }

	Ns_HistoryId_t(UInt level, UInt id) {
		this->level = level;
		this->id = id;
	}
};


}

#endif