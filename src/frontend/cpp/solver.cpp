#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <common/representation.h>

namespace feather {

Solver::Solver() {
	repr = new Representation();
}

Solver::~Solver() {
	delete repr;
}

IntVarID Solver::makeIntVar(Int min, Int max) {
	repr->vars.push_back( RepresentationIntVar(min, max) );
}

IntDomain& Solver::getDomain(IntVarID id) {
	return repr->vars[id].domain;
}



}
