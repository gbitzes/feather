#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <base/utils.h>
#include <feather/int-domain.h>

namespace feather {

void IntVar::initialize(Solver &_slv, IntVarID id) {
	initialized = true;
	this->slv = &_slv;
	this->id = id;
	domain = &slv->getDomain(id);
}

Solver& IntVar::getSolver() const {
	if(!initialized)
		FEATHER_THROW("Uninitialized IntVar");

	return *slv;
}



/*
 * Initialize an IntVar by creating a new variable
 * inside representation
 */

IntVar::IntVar(Solver &_slv, Int min, Int max) {
	IntVarID newvar = _slv.makeIntVar(min, max);
	initialize(_slv, newvar);
}

/*
 * Initialize an IntVar by providing an already
 * existing id and solver
 */

IntVar::IntVar(IntVarID id, Solver& slv) {
	initialize(slv, id);
}

/*
 * Provide an empty constructor for creating an uninitialized
 * IntVar. Otherwise, we wouldn't be able to do IntVar a = b + c
 * as the assignment operator needs all variables to have been
 * already constructed
 */

IntVar::IntVar() { 
	initialized = false; 
}

bool IntVar::hasSameSolver(IntVar other) const {
	if(!initialized || !other.initialized )
		return false;

	return slv == other.slv;
}

Int IntVar::min() const {
	return domain->min();
}

Int IntVar::max() const {
	return domain->max();
}

IntVarID IntVar::getID() const {
	return id;
}

}