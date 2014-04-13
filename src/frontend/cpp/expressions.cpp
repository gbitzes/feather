#include <feather/frontend/cpp/expressions.h>
#include <common/constraints.h>
#include <iostream>
#include <base/utils.h>
#include <base/int-ranges.h>

namespace feather {


/*
 * Y + C
 */

	IntVar operator+ (IntVar Y, Int C) {
		if(C == 0)
			return Y;

		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()+C, Y.max()+C );
		slv.addConstraint( new Constr_XeqYplusC(id, Y.getID(), C) );

		return IntVar(id, slv);
	}

	IntVar operator+ (Int C, IntVar Y) {
		return Y+C;
	}

 	IntVar operator- (IntVar Y, Int C) {
 		return Y + (-C);
 	}

/*
 * Y / C
 */

 	IntVar operator/ (IntVar Y, Int C) {

 	}

/*
 * Y + Z
 */

 	IntVar operator+ (IntVar Y, IntVar Z) {

 	}

/*
 * Y - Z
 */

 	IntVar operator- (IntVar Y, IntVar Z) {

 	}

/*
 * C - Z
 */

 	IntVar operator- (Int C, IntVar Z) {

 	}

 	IntVar operator- (IntVar Z) {
 		return 0 - Z;
 	}

/*
 * Y * Z
 */

 	IntVar operator* (IntVar Y, IntVar Z) {

 	}

/*
 * Y * C
 */

	IntVar operator* (IntVar Y, Int C) {

	}

	IntVar operator* (Int C, IntVar Y) {
		return Y*C;
	}



}
