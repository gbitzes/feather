#ifndef ___FEATHER_COMMON_CONSTRAINTS_H
#define ___FEATHER_COMMON_CONSTRAINTS_H

/*
 * This file contains all the possible constraints
 * and how they're defined inside a representation
 * object
 */

#include <vector>
#include <feather/types.h>

namespace feather {

namespace Constraints {
	enum Type {
		XlessthanY = 0,
		XlesseqthanY,
		XeqAbsY,
		XeqY,
		XneqY,
		XorY,
		XeqYplusC,
		XeqYdivC,
		XeqYtimesC,
		XeqYmodC,
		XeqYplusZ,
		XeqYtimesZ,
		XeqYmodZ,
		XeqCminusZ,
		XeqCdivZ,
		XeqCmodZ,
		MetaXeqYlessthanC,
		MetaXeqYlesseqthanC,
		MetaXeqYgreaterthanC,
		MetaXeqYgreatereqthanC,
		MetaXeqYeqC,
		MetaXeqYneqC,
		MetaXeqYlessthanZ,
		MetaXeqYlesseqthanZ,
		MetaXeqYeqZ,
		MetaXeqYandZ,
		MetaXeqYorZ,
		AllDiff,
		AllDiffStrong,
		XeqSum,
		XeqElement,
		Count
	};
};

class Constraint {
	public:
		const Constraints::Type fType;
		Constraint(Constraints::Type type) : fType(type) {}
};

/*
 * Since there's a high degree of code repetition in the constraint
 * definitions, the following code is auto-generated with
 * generate-constraint-definitions.py
 *
 * Do not edit by hand!
 */

#include <common/constraints.icc>

} // namespace feather
#endif

