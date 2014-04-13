#include <feather/frontend/cpp/expressions.h>
#include <common/constraints.h>
#include <iostream>
#include <base/utils.h>
#include <base/int-ranges.h>

namespace feather {


/*
 * Y + C
 */

 	IntVar ExprYplusC::post() {
 		if(C == 0)
 			return Y;

		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()+C, Y.max()+C );
		slv.addConstraint( new Constr_XeqYplusC(id, Y.getID(), C) );

		return IntVar(id, slv);
 	}

/*
 * Y / C
 */

 	IntVar ExprYdivC::post() {
 		if(C == 0) FEATHER_THROW("Attempted division by zero");
 		if(C == 1) return Y;

		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()/C, Y.max()/C );
		slv.addConstraint( new Constr_XeqYdivC(id, Y.getID(), C) );

		return IntVar(id, slv);
 	}

/*
 * Y + Z
 */

 	IntVar ExprYplusZ::post() {
		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()+Z.min(), Y.max()+Z.max() );
		slv.addConstraint( new Constr_XeqYplusZ(id, Y.getID(), Z.getID()) );

		return IntVar(id, slv);
 	}


/*
 * Y - Z
 */

 	IntVar ExprYminusZ::post() {
		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()-Z.min(), Y.max()-Z.max() );
		slv.addConstraint( new Constr_XeqYminusZ(id, Y.getID(), Z.getID()) );

		return IntVar(id, slv);
 	}

/*
 * C - Z
 */

 	IntVar ExprCminusZ::post() {
		Solver &slv = Z.getSolver();
		IntVarID id = slv.makeIntVar( C-Z.min(), C-Z.max() );
		slv.addConstraint( new Constr_XeqCminusZ(id, C, Z.getID()) );

		return IntVar(id, slv);
 	}

/*
 * Y * Z
 */

 	IntVar ExprYtimesZ::post() {
		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()*Z.min(), Y.max()*Z.max() );
		slv.addConstraint( new Constr_XeqYtimesZ(id, Y.getID(), Z.getID()) );

		return IntVar(id, slv);
 	}

/*
 * Y * C
 */

 	IntVar ExprYtimesC::post() {
 		if(C == 1)
 			return Y;

		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()*C, Y.max()*C );
		slv.addConstraint( new Constr_XeqYtimesC(id, Y.getID(), C) );

		return IntVar(id, slv);
 	}



}
