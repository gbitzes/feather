#ifndef ___FEATHER_COMMON_REPRESENTATION_H
#define ___FEATHER_COMMON_REPRESENTATION_H

#include <feather/types.h>
#include <feather/frontend/cpp/types.h>
#include <common/constraints.h>
#include <common/goals.h>
#include <base/int-ranges.h>

#include <vector>

namespace feather {

/*
 * We don't really know what kinds of things a
 * constraint or goal might need to store inside -
 * we just provide a vector in which to store integers.
 * The meaning and order is decided by each 
 * individual constraint.
 *
 * Same for goals.
 */

// class Constraint {
// 	enum ConstraintType::Type type;
// 	std::vector<Int> values;
// };

struct Goal {
	enum GoalType::Type type;
	std::vector<Int> values;
};

struct RepresentationIntVar {
	IntRanges *domain;
	IntVarID id;
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
		std::vector<Goal> goals;

		IntVarID addVar(Int min, Int max);
		~Representation();
};



} // namespace feather


#endif
