#include <feather/frontend/cpp/expressions.h>
#include <common/create-constraint.h>
#include <iostream>
#include <base/utils.h>
#include <base/int-ranges.h>

namespace feather {


/*
 * Y + C
 */

	IntVar operator+ (const IntVar Y, const Int C) {
		if(C == 0)
			return Y;

		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()+C, Y.max()+C );
		slv.addConstraint( __create_XeqYplusC(id, Y.getID(), C) );

		return IntVar(id, slv);
	}

	IntVar operator+ (const Int C, const IntVar Y) {
		return Y+C;
	}

 	IntVar operator- (const IntVar Y, const Int C) {
 		return Y + (-C);
 	}

/*
 * Y / C
 */

 	IntVar operator/ (const IntVar Y, const Int C) {

 	}

/*
 * Y + Z
 */

 	IntVar operator+ (const IntVar Y, const IntVar Z) {

 	}

/*
 * Y - Z
 */

 	IntVar operator- (const IntVar Y, const IntVar Z) {

 	}

/*
 * C - Z
 */

 	IntVar operator- (const Int C, const IntVar Z) {

 	}

 	IntVar operator- (const IntVar Z) {
 		return 0 - Z;
 	}

/*
 * Y * Z
 */

 	IntVar operator* (const IntVar Y, const IntVar Z) {

 	}

/*
 * Y * C
 */

	IntVar operator* (const IntVar Y, const Int C) {

	}

	IntVar operator* (const Int C, const IntVar Y) {
		return Y*C;
	}



}
