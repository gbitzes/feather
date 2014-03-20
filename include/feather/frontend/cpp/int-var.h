#ifndef FEATHER_FRONTEND_CPP_INTVAR_H
#define FEATHER_FRONTEND_CPP_INTVAR_H

#include <feather/types.h>
#include <feather/frontend/cpp/solver.h>

namespace feather {

class IntVar {

	private:
		IntVarID id;
		Solver &slv;
		IntDomain &domain;

	public:
		IntVar(Solver&, Int min, Int max);
};


} // namespace feather


#endif
