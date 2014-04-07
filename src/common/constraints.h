#ifndef ___FEATHER_COMMON_CONSTRAINTS_H
#define ___FEATHER_COMMON_CONSTRAINTS_H

/*
 * This file contains all the possible constraints
 * and how they're defined inside a representation
 * object
 */

#include <feather/types.h>

namespace feather {

struct ConstraintType {
	enum Type {

		/*
		 * X = Y + C
		 * [0] is id of X
		 * [1] is id of Y
		 * [2] is C
		 */

		XeqYplusC = 0

	};
};

} // namespace feather
#endif

