#ifndef FEATHER_FRONTEND_CPP_SOLVER_H
#define FEATHER_FRONTEND_CPP_SOLVER_H

#include <feather/types.h>
#include <feather/frontend/cpp/goal-expr.h>
#include "int-var.h"
#include "types.h"

namespace feather {

class RepresentationIntVarArray;
class Goal;

class Solver {

	private:
		ProblemManager *fPM;
		Representation *repr;
		bool finalized;
		bool solving;
		SolutionID currentSolutionID;
	public:

		IntDomain& getRepresentationDomain(IntVarID);
		IntDomain* getDomain(IntVarID);
		RepresentationIntVarArray& getArray(IntVarArrayID);

		Solver(ProblemManager*);
		~Solver();

		void addGoal(Goal*);

		IntVarID makeIntVar(Int min, Int max);
		IntVarArrayID makeIntVarArray();

		void addConstraint(const Constraint *const constr);

		void finalize();
		bool nextSolution();

		SolutionID getSolutionID();		

};


} // namespace feather

#endif