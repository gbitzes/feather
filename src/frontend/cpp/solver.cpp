#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <common/representation.h>
#include <base/utils.h>

namespace feather {

Solver::Solver() {
	repr = new Representation();
}

Solver::~Solver() {
	delete repr;
}

IntVarID Solver::makeIntVar(Int min, Int max) {
	return repr->addVar(min, max);
}

IntDomain& Solver::getDomain(IntVarID id) {
	return *repr->vars[id].domain;
}

void Solver::addConstraint(const Constraint *const constr) {
	repr->constraints.push_back(constr);
}


}
