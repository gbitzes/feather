#ifndef ___FEATHER_COMMON_CONSTRAINTS_H
#define ___FEATHER_COMMON_CONSTRAINTS_H

/*
 * This file contains all the possible constraints
 * and how they're defined inside a representation
 * object
 */

#include <feather/types.h>

namespace feather {

namespace Constraints {
	enum Type {
		XeqYplusC = 0,
		XlessthanY,
		XlesseqthanY
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

class Constr_XlessthanY : public Constraint {
	public:

		const IntVarID fX, fY;

		Constr_XlessthanY(IntVarID x, IntVarID y)
			 : Constraint(Constraints::XlessthanY), fX(x), fY(y) { } 
};

class Constr_XlesseqthanY : public Constraint {
	public:

		const IntVarID fX, fY;

		Constr_XlesseqthanY(IntVarID x, IntVarID y)
			 : Constraint(Constraints::XlesseqthanY), fX(x), fY(y) { } 
};

class Constr_XeqYplusC : public Constraint {
	public:

		const IntVarID fX, fY;
		const Int fC;

		Constr_XeqYplusC(IntVarID x, IntVarID y, Int c)
			 : Constraint(Constraints::XeqYplusC), fX(x), fY(y), fC(c) { } 
};



} // namespace feather
#endif

