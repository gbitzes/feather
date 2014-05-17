#include <feather/frontend/cpp/expressions.h>
#include <feather/frontend/cpp/int-deque.h>
#include <common/constraints.h>
#include <iostream>
#include <feather/utils.h>
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

namespace {

void product_min_max (IntVar &Y, IntVar &Z, Int& min, Int& max) {
	Int  prod;
	prod = Y.min() * Z.min();
	min = max = prod;

	prod = Y.max() * Z.min();
	if (prod < min)
		min = prod;
	if (prod > max)
		max = prod;

	prod = Y.min() * Z.max();
	if (prod < min)
		min = prod;
	if (prod > max)
		max = prod;

	prod = Y.max() * Z.max();
	if (prod < min)
		min = prod;
	if (prod > max)
		max = prod;
}
}

 	IntVar ExprYtimesZ::post() {
		Solver &slv = Y.getSolver();
		Int prodmin, prodmax;
		product_min_max(Y, Z, prodmin, prodmax);
		IntVarID id = slv.makeIntVar( prodmin, prodmax );
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
 		return IntVar(id, slv);
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
		return IntVar(id, slv);
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
  		return IntVar(id, slv);
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
  		return IntVar(id, slv);
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
  			slv.add( new Constr_MetaXeqYeqZ(id, Y.getID(), Z.getID(), isPositive));
  		}
  		else {
  			id = slv.makeIntVar(intersectionEmpty(Y, Z), !(Y.max()==Z.min() && Y.min()==Z.max()) );
			slv.add( new Constr_MetaXeqYeqZ(id, Z.getID(), Y.getID(), isPositive));
  		}
  		return IntVar(id, slv);
  	}

  	Constraint const* ExprConstrYeqZ::postConstraint() {
  		if(isPositive)
  			return new Constr_XeqY(Y.getID(), Z.getID());
  		else
  			return new Constr_XneqY(Z.getID(), Y.getID());
  	}
  
  /*
   * Y or Z
   */

  IntVar ExprConstrYorZ::postC() {
      Solver &slv = Y.getSolver();
      IntVarID id;
      if(isPositive)
          id = slv.makeIntVar(Y.min()+Z.min()!=0, Y.max()+Z.max()!=0);
      else
      	  id = slv.makeIntVar(!(Y.max() + Z.max() != 0), !(Y.min() + Z.min() != 0));
      slv.add( new Constr_MetaXeqYorZ(id, Y.getID(), Z.getID(), isPositive));
      return IntVar(id, slv);
  }

  Constraint const* ExprConstrYorZ::postConstraint() {
  	  return new Constr_XorY(Y.getID(), Z.getID(), isPositive);
  }

  /*
   * alldiff
   */

	Constraint const* AllDiff(IntVarArray& VarArr, unsigned long Capacity) {
		if(VarArr.size() <= Capacity || (Capacity == 0 && VarArr.size() <= 1))
			return 0; // no constraint

		Constraint *newconstr;
		if(Capacity == 0)
			newconstr = new Constr_AllDiff(VarArr.getID());
		else
			newconstr = new Constr_AllDiffStrong(VarArr.getID(), Capacity);

		return newconstr;
	}

namespace {

void array_sum_min_max(IntVarArray& arr, UInt start, UInt length,
			Int& sumMin, Int& sumMax) {
	sumMin = sumMax = 0;
	for (UInt i=start; i < start+length; ++i) {
		IntVar V = arr[i];
		sumMin += V.min();
		sumMax += V.max();
	}
}

}
  
/*
 * sum
 */

	IntVar ExprSum::post() {
		if(Arr.empty())
			FEATHER_THROW("Cannot get the sum of an empty array");

		Solver &slv = Arr.getSolver();

		Int summin = 0, summax = 0;
		array_sum_min_max(Arr, start, length, summin, summax);
		IntVarID id = slv.makeIntVar(summin, summax);
		slv.add(new Constr_XeqSum(id, Arr.getID(), start, length));

		return IntVar(id, slv);
	}

/*
 * deque[index]
 */

namespace  {

	void
	array_elements_min_max (const IntDeque& intArray,
			IntVar& VarIndex,
			Int& minElement, Int& maxElement)
	{
		minElement  =  kPlusInf;
		maxElement  =  kMinusInf;

		for (IntVar::iterator index = VarIndex.begin();
				index != VarIndex.end();
				++index)
		{
			if ( 0 <= *index
			&& static_cast<UInt>(*index) < intArray.size() )
			{

				if ( intArray[*index]  <  minElement )
					minElement  =  intArray[*index];

				if ( intArray[*index]  >  maxElement )
					maxElement  =  intArray[*index];
			}
		}

		if(minElement == kPlusInf)
			FEATHER_THROW("Failed to index the integer array");
	}
}

	IntVar IntDeque::operator[](IntVar index) {
		if(&slv != &index.getSolver())
			FEATHER_THROW("Attempted to post operator[] constraint on a deque of a different solver");
		
		Int minElement, maxElement;
		array_elements_min_max(*this, index, minElement, maxElement);
		IntVarID varid = slv.makeIntVar(minElement, maxElement);
		slv.add(new Constr_XeqElement(varid, index.getID(), id));

		return IntVar(varid, slv);
	}

/*
 * count
 */

	Constraint const* Count(IntVarArray& arr,
							const IntDeque& values,
							const IntDeque& occurences,
							const std::deque<IntDeque>& splitPositions,
							const UInt split,
							const UInt dwin) {

		/* 
		 * Convert from std::deque<IntDeque> into
		 * std::vector<IntDequeID>
		 */

		std::vector<IntDequeID> spositions;
		for(int i = 0; i < splitPositions.size(); i++)
			spositions.push_back(i);

		return new Constr_Count(arr.getID(), values.getID(), occurences.getID(),
							spositions, split, dwin);
	}

/*
 * IfThen
 */

ExprConstrYorZ IfThen(const ExprConstr& Y, const ExprConstr& Z) {
	ExprConstr& Ync = const_cast<ExprConstr&>(Y);
	ExprConstr& Znc = const_cast<ExprConstr&>(Z);

	return !Ync || Znc;
}





}
