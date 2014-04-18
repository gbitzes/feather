#include <feather/frontend/cpp/expressions.h>
#include <common/constraints.h>
#include <iostream>
#include <base/utils.h>
#include <base/int-ranges.h>

namespace feather {

namespace {
	bool intersectionEmpty(IntVar &Y, IntVar Z) {
		return (Y.min() > Z.max()) || (Y.max() < Z.min());
	}
}


/*
 * Y + C
 */

 	IntVar ExprYplusC::post() {
 		if(C == 0)
 			return Y;

		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()+C, Y.max()+C );
		slv.add( new Constr_XeqYplusC(id, Y.getID(), C) );

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
		slv.add( new Constr_XeqYdivC(id, Y.getID(), C) );

		return IntVar(id, slv);
 	}

/*
 * Y + Z
 */

 	IntVar ExprYplusZ::post() {
		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()+Z.min(), Y.max()+Z.max() );
		slv.add( new Constr_XeqYplusZ(id, Y.getID(), Z.getID()) );

		return IntVar(id, slv);
 	}


/*
 * Y - Z
 */

 	IntVar ExprYminusZ::post() {
		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()-Z.max(), Y.max()-Z.min() );
		slv.add( new Constr_XeqYplusZ(Y.getID(), id, Z.getID()) );

		return IntVar(id, slv);
 	}

/*
 * C - Z
 */

 	IntVar ExprCminusZ::post() {
		Solver &slv = Z.getSolver();
		IntVarID id = slv.makeIntVar( C-Z.max(), C-Z.min() );
		slv.add( new Constr_XeqCminusZ(id, C, Z.getID()) );

		return IntVar(id, slv);
 	}

/*
 * Y * Z
 */

 	IntVar ExprYtimesZ::post() {
		Solver &slv = Y.getSolver();
		IntVarID id = slv.makeIntVar( Y.min()*Z.min(), Y.max()*Z.max() );
		slv.add( new Constr_XeqYtimesZ(id, Y.getID(), Z.getID()) );

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
		slv.add( new Constr_XeqYtimesC(id, Y.getID(), C) );

		return IntVar(id, slv);
 	}

/*
 * Y / Z
 */

 	// IntVar ExprYdivZ::post() {
 	// 	if(C == 1)
 	// 		return Y;

		// Solver &slv = Y.getSolver();
		// IntVarID id = slv.makeIntVar( Y.min()*C, Y.max()*C );
		// slv.add( new Constr_XeqYtimesC(id, Y.getID(), C) );

		// return IntVar(id, slv);
 	// }

/*
 * Y < C
 */

	IntVar ExprConstrYlessthanC::postC() {
		Solver &slv = Y.getSolver();
		IntVarID id;
		if(isPositive) {
			id = slv.makeIntVar(Y.max() < C, Y.min() < C);
			slv.add( new Constr_MetaXeqYlessthanC(id, Y.getID(), C) );
		}
		else {
			id = slv.makeIntVar(Y.min() >= C, Y.max() >= C);
			slv.add( new Constr_MetaXeqYgreatereqthanC(id, Y.getID(), C));
		}
		return IntVar(id, slv);
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
 		Solver &slv = Y.getSolver();
 		IntVarID id;
 		if(isPositive) {
 			id = slv.makeIntVar(Y.max() <= C, Y.min() <= C);
 			slv.add( new Constr_MetaXeqYlesseqthanC(id, Y.getID(), C));
 		}
 		else {
 			id = slv.makeIntVar(Y.min() > C, Y.max() > C);
 			slv.add( new Constr_MetaXeqYgreaterthanC(id, Y.getID(), C));
 		}
 	}

	Constraint const* ExprConstrYlesseqthanC::postConstraint() {
		if(isPositive)
			Y.remove(C+1, kPlusInf);
		else
			Y.remove(kMinusInf, C);
		return NULL;
	}

  /*
   * Y == C
   */

	IntVar ExprConstrYeqC::postC() {
		Solver &slv = Y.getSolver();
		IntVarID id;
		if(isPositive) {
			id = slv.makeIntVar(Y.min()==C && Y.max()==C, Y.contains(C) );
			slv.add( new Constr_MetaXeqYeqC(id, Y.getID(), C));
		}
		else {
			id = slv.makeIntVar(!Y.contains(C), (Y.min()!=C || Y.max()!=C) );
			slv.add( new Constr_MetaXeqYneqC(id, Y.getID(), C));
		}
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
  		Solver &slv = Y.getSolver();
  		IntVarID id;
  		if(isPositive) {
  			id = slv.makeIntVar(Y.max() < Z.min(), Y.min() < Z.max());
  			slv.add( new Constr_MetaXeqYlessthanZ(id, Y.getID(), Z.getID()) );
  		}
  		else {
  			id = slv.makeIntVar(Z.max() <= Y.min(), Z.min() <= Y.max());
			slv.add( new Constr_MetaXeqYlesseqthanZ(id, Z.getID(), Y.getID() ));
  		}
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
  		Solver &slv = Y.getSolver();
  		IntVarID id;
  		if(isPositive) {
  			id = slv.makeIntVar(Y.max() <= Z.min(), Y.min() <= Z.max());
  			slv.add( new Constr_MetaXeqYlesseqthanZ(id, Y.getID(), Z.getID()) );
  		}
  		else {
  			id = slv.makeIntVar(Z.max() < Y.min(), Z.min() < Y.max());
			slv.add( new Constr_MetaXeqYlessthanZ(id, Z.getID(), Y.getID() ));
  		}
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
  		Solver &slv = Y.getSolver();
  		IntVarID id;
  		if(isPositive) {
  			id = slv.makeIntVar(Y.max()==Z.min() && Y.min()==Z.max(), !intersectionEmpty(Y, Z) );
  			slv.add( new Constr_MetaXeqYeqZ(id, Y.getID(), Z.getID(), false));
  		}
  		else {
  			id = slv.makeIntVar(intersectionEmpty(Y, Z), !(Y.max()==Z.min() && Y.min()==Z.max()) );
			slv.add( new Constr_MetaXeqYeqZ(id, Z.getID(), Y.getID(), true));
  		}
  	}

  	Constraint const* ExprConstrYeqZ::postConstraint() {
  		if(isPositive)
  			return new Constr_XeqY(Y.getID(), Z.getID());
  		else
  			return new Constr_XneqY(Z.getID(), Y.getID());
  	}

  /*
   * alldiff
   */

	Constraint const* AllDiff(IntVarArray VarArr, unsigned long Capacity) {
		if(VarArr.size() <= Capacity || (Capacity == 0 && VarArr.size() <= 1))
			return 0; // no constraint

		Constraint *newconstr;
		if(Capacity == 0)
			newconstr = new Constr_AllDiff(VarArr.getID());
		else
			newconstr = new Constr_AllDiffStrong(VarArr.getID(), Capacity);

		return newconstr;
	}

}
