#ifndef FEATHER_BACKEND_NAXOS_BASICS_H
#define FEATHER_BACKEND_NAXOS_BASICS_H


#ifdef  __GNUC__
	#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
		#define  Ns_OLD_UNORDERED_SET
	#endif
#endif

#ifdef Ns_OLD_UNORDERED_SET
	#include <ext/hash_set>
	#include <ext/hash_map>
	#define  Ns_UNORDERED_SET  __gnu_cxx::hash_set
	#define  Ns_UNORDERED_MAP  __gnu_cxx::hash_map
#else
	#include <unordered_set>
	#include <unordered_map>
	#define  Ns_UNORDERED_SET  std::unordered_set
	#define  Ns_UNORDERED_MAP  std::unordered_map
#endif

namespace feather {



typedef size_t NsIndex;
typedef  unsigned long  Ns_pointer_t;


/* Naxos methods throw this type of exception */
class NsException : public std::logic_error  {
	
	public:
	
		NsException (const std::string& error_message) : logic_error("Naxos: " + error_message) {

		}
};

#define DISABLE_NS_ASSERTIONS
inline void assert_Ns (const bool condition, const char* errorMessage) {
	#ifndef DISABLE_NS_ASSERTIONS
		if(!condition)
			throw NsException(errorMessage);
	#endif
}


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