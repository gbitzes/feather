#ifndef ___FEATHER_COMMON_REPRESENTATION_H
#define ___FEATHER_COMMON_REPRESENTATION_H

#include <feather/types.h>
#include <feather/frontend/cpp/types.h>
#include <common/constraints.h>
#include <common/goals.h>
#include <base/int-ranges.h>

#include <vector>

namespace feather {

struct RepresentationIntVar {
	IntRanges *domain;
	IntVarID id;
};

class RepresentationIntVarArray {
	public:
		std::deque<IntVarID> vars;
		IntVarArrayID id;
};

struct RepresentationIntDeque {
	std::deque<Int> contents;
	IntDequeID id;

	RepresentationIntDeque(UInt size) : contents(size) {}
	RepresentationIntDeque() {}
};

class Representation {

	public:
		/*
		 * The variables are represented as a set 
		 * of ranges that specify their domain.
		 * The index of them inside the vector is very
		 * important as it defines their ID.
		 */

		std::vector<RepresentationIntVar> vars;
		std::vector<const Constraint*> constraints;
		std::vector<const Goal*> goals;
		std::vector<RepresentationIntVarArray*> arrays;
		std::vector<RepresentationIntDeque*> deques;
		IntVarID minObj;

		IntVarID addVar(Int min, Int max);
		IntVarArrayID addArray();
		IntDequeID addDeque(UInt size = 0);

        Representation() {
            minObj = -1;
        }

		~Representation();
};



} // namespace feather


#endif
