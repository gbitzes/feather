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


/*
 * Y < C
 */

	IntVar ExprConstrYlessthanC::postC() {
		// Solver &slv = Y.getSolver();
		// if(isPositive) {
		// 	IntVarID id = slv.makeIntVar(Y.max() < C, Y.min() < C);
		// }
	}

	Constraint const* ExprConstrYlessthanC::postConstraint() {
		if(isPositive)
			Y.remove(C, kPlusInf);
		else
			Y.remove(kMinusInf, C-1);
		return NULL;
	}

 /*
  * Y <= C
  */

 	IntVar ExprConstrYlesseqthanC::postC() {

 	}

	Constraint const* ExprConstrYlesseqthanC::postConstraint() {
		if(isPositive)
			Y.remove(C+1, kPlusInf);
		else
			Y.remove(kMinusInf, C);
		return NULL;
	}

	IntVar ExprConstrYeqC::postC() {

	}

	Constraint const* ExprConstrYeqC::postConstraint() {
		if(isPositive) {
			Y.remove(kMinusInf, C-1);
			Y.remove(C+1, kPlusInf);
		} 
		else {
			if(Y.contains(C))
				Y.remove(C);
		}
		return NULL;
	}

 /*
  * Y < Z
  */

  	IntVar ExprConstrYlessthanZ::postC() {

  	}

  	Constraint const* ExprConstrYlessthanZ::postConstraint() {
  		if(isPositive)
  			return new Constr_XlessthanY(Y.getID(), Z.getID());
  		else
  			return new Constr_XlesseqthanY(Z.getID(), Y.getID());
  	}

  /*
   * Y <= Z
   */

   	IntVar ExprConstrYlesseqthanZ::postC() {

  	}

  	Constraint const* ExprConstrYlesseqthanZ::postConstraint() {
  		if(isPositive)
  			return new Constr_XlesseqthanY(Y.getID(), Z.getID());
  		else
  			return new Constr_XlessthanY(Z.getID(), Y.getID());
  	}

  /*
   * Y == Z
   */

   	IntVar ExprConstrYeqZ::postC() {

  	}

  	Constraint const* ExprConstrYeqZ::postConstraint() {
  		if(isPositive)
  			return new Constr_XeqY(Y.getID(), Z.getID());
  		else
  			return new Constr_XneqY(Z.getID(), Y.getID());
  	}



}
