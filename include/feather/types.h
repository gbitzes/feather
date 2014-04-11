#ifndef FEATHER_TYPES_H
#define FEATHER_TYPES_H

#include <climits>
#include <stdexcept>
#include <string>

namespace feather {

typedef long Int;
typedef unsigned long UInt;

/* A type to be used as a holder for bits */
typedef unsigned long long Chunk;
const UInt CHUNK_BITS = CHAR_BIT * sizeof(Chunk);


const Int kPlusInf = LONG_MAX;
const Int kMinusInf = LONG_MIN;
const Int kUPlusInf = ULONG_MAX;

class FeatherException : public std::logic_error {
	public:
		FeatherException(const std::string& message)
			: std::logic_error(message) {}
};

struct Range {
	int start, end;

	Range(int start, int end) {
		this->start = start;
		this->end = end;
	}

	bool isValid() {
		return (end >= start);
	}
};

/*
 * Forward declarations
 */

// class Solver;
class IntDomain;


} // namespace feather



#endif