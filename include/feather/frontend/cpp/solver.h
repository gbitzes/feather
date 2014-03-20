#ifndef FEATHER_FRONTEND_CPP_INTVAR_H
#define FEATHER_FRONTEND_CPP_INTVAR_H

#include <feather/types.h>
#include <feather/common/problem-manager.h>
#include <common/representation.h>

namespace feather {

class Solver {

	private:
		Representation repr;
		IntVarID makeIntVar(Int min, Int max);

	public:

	
};


} // namespace feather

#endif