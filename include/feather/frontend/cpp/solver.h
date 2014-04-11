#ifndef FEATHER_FRONTEND_CPP_SOLVER_H
#define FEATHER_FRONTEND_CPP_SOLVER_H

#include <feather/types.h>
#include "int-var.h"
#include "types.h"

namespace feather {

class Solver {

	private:
		Representation *repr;

	public:

		IntDomain& getDomain(IntVarID);
		Solver();
		~Solver();
		IntVarID makeIntVar(Int min, Int max);
		void addConstraint(const Constraint *const constr);

};


} // namespace feather

#endif