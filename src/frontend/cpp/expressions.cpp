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
		Solver &slv = Y.getSolver();
		IntVarID id;
		if(isPositive) {
			id = slv.makeIntVar(Y.max() < C, Y.min() < C);
			slv.addConstraint( new Constr_MetaXeqYlessthanC(id, Y.getID(), C) );
		}
		else {
			id = slv.makeIntVar(Y.min() >= C, Y.max() >= C);
			slv.addConstraint( new Constr_MetaXeqYgreatereqthanC(id, Y.getID(), C));
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
 			slv.addConstraint( new Constr_MetaXeqYlesseqthanC(id, Y.getID(), C));
 		}
 		else {
 			id = slv.makeIntVar(Y.min() > C, Y.max() > C);
 			slv.addConstraint( new Constr_MetaXeqYgreaterthanC(id, Y.getID(), C));
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
			slv.addConstraint( new Constr_MetaXeqYeqC(id, Y.getID(), C));
		}
		else {
			id = slv.makeIntVar(!Y.contains(C), (Y.min()!=C || Y.max()!=C) );
			slv.addConstraint( new Constr_MetaXeqYneqC(id, Y.getID(), C));
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
  			slv.addConstraint( new Constr_MetaXeqYlessthanZ(id, Y.getID(), Z.getID()) );
  		}
  		else {
  			id = slv.makeIntVar(Z.max() <= Y.min(), Z.min() <= Y.max());
			slv.addConstraint( new Constr_MetaXeqYlesseqthanZ(id, Z.getID(), Y.getID() ));
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
  			slv.addConstraint( new Constr_MetaXeqYlesseqthanZ(id, Y.getID(), Z.getID()) );
  		}
  		else {
  			id = slv.makeIntVar(Z.max() < Y.min(), Z.min() < Y.max());
			slv.addConstraint( new Constr_MetaXeqYlessthanZ(id, Z.getID(), Y.getID() ));
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
  			slv.addConstraint( new Constr_MetaXeqYeqZ(id, Y.getID(), Z.getID(), false));
  		}
  		else {
  			id = slv.makeIntVar(intersectionEmpty(Y, Z), !(Y.max()==Z.min() && Y.min()==Z.max()) );
			slv.addConstraint( new Constr_MetaXeqYeqZ(id, Z.getID(), Y.getID(), true));
  		}
  	}

  	Constraint const* ExprConstrYeqZ::postConstraint() {
  		if(isPositive)
  			return new Constr_XeqY(Y.getID(), Z.getID());
  		else
  			return new Constr_XneqY(Z.getID(), Y.getID());
  	}



}
