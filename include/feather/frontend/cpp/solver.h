#ifndef FEATHER_FRONTEND_CPP_SOLVER_H
#define FEATHER_FRONTEND_CPP_SOLVER_H

#include <feather/types.h>
#include <feather/frontend/cpp/goal-expr.h>
#include "int-var.h"
#include "types.h"

namespace feather {

class RepresentationIntVarArray;
class RepresentationIntDeque;
class Goal;
class ExprConstr;

class Solver {

	private:
		ProblemManager *fPM;
		Representation *repr;
		bool finalized;
		bool solving;
		SolutionID currentSolutionID;
		void finalize();
	public:

		IntDomain& getRepresentationDomain(IntVarID);
		IntDomain* getDomain(IntVarID);
		RepresentationIntVarArray& getArray(IntVarArrayID);
		RepresentationIntDeque& getDeque(IntDequeID);

		Solver(ProblemManager*);
		~Solver();

		void addGoal(Goal*);

		IntVarID makeIntVar(Int min, Int max);
		IntVarArrayID makeIntVarArray();
		IntDequeID makeIntDeque(UInt size);

		void minimize(const IntVar);
		void add(const Constraint *const constr);
		void add(const ExprConstr &expr);
		bool isFinalized();
		bool nextSolution();

		SolutionID getSolutionID();		

};


} // namespace feather

#endif