#ifndef FEATHER_FRONTEND_CPP_INTVAR_H
#define FEATHER_FRONTEND_CPP_INTVAR_H

#include <feather/types.h>
#include "solver.h"
#include "types.h"
#include <feather/int-domain.h>

namespace feather {

/*
 * IntVar is a convenience class used by the user
 * to define their problem. Uses operator overloading
 * so the user can do things like IntVar a = b + c
 */

class IntVar {

	private:
		IntVarID id;
		Solver *slv;
		IntDomain *representationDomain;
		IntDomain *domain;
		bool initialized;
		SolutionID currentSolutionID;

		void updateDomain();
	public:

		void initialize(Solver&, IntVarID);

		IntVar(Solver&, Int min, Int max);
		IntVar(IntVarID id, Solver& slv);
		IntVar();

		void remove(Int, Int);
		void remove(Int);
		bool contains(Int) const;

		/*
		 * The default, member-wise copy
		 * constructor is what we want and
		 * we don't bother re-implementing it
		 */

		Solver &getSolver() const;
		Int min();
		Int max();
		Int value();
		Int isBound();

		IntVarID getID() const;
		IntDomain const* getDomain() const;


		/*
		 * Convenience function that returns
		 * whether two variables share the
		 * same solver
		 */

		bool hasSameSolver(const IntVar other) const;

		/*
		 * Tests whether two IntVars refer to the same
		 * variable
		 */

		bool equals(const IntVar other) const;

};


} // namespace feather


#endif
