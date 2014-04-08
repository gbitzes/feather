#ifndef FEATHER_TYPES_H
#define FEATHER_TYPES_H

#include <climits>
#include <stdexcept>
#include <string>

namespace feather {

typedef long Int;
typedef unsigned long UInt;
typedef unsigned long long Word;

/* The number of bits that a word can hold */
const UInt MW_BITS = CHAR_BIT * sizeof(Word);


const Int kPlusInf = LONG_MAX;
const Int kMinusInf = LONG_MIN;
const Int kUPlusInf = ULONG_MAX;

/* The number of bits that a machine word holds */
const UInt kMachineWordBits = CHAR_BIT * sizeof(size_t);

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