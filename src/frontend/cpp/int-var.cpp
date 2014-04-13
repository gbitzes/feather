#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <base/utils.h>
#include <feather/int-domain.h>

namespace feather {

void IntVar::initialize(Solver &_slv, IntVarID id) {
	initialized = true;
	this->slv = &_slv;
	this->id = id;
	representationDomain = &slv->getRepresentationDomain(id);
	domain = representationDomain;
	currentSolutionID = slv->getSolutionID();
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

Int IntVar::min() {
	updateDomain();
	return domain->min();
}

Int IntVar::max() {
	updateDomain();
	return domain->max();
}

Int IntVar::value() {
	updateDomain();
	if(domain->max() == domain->min() )
		return domain->max();

	FEATHER_THROW("Variable is not bound to a single value");
}

IntDomain const* IntVar::getDomain() const {
	return domain;
}

void IntVar::updateDomain() {
	if( slv->getSolutionID() != currentSolutionID ) {
		if(domain != representationDomain)
			delete domain;

		domain = slv->getDomain(id);
	}
}

IntVarID IntVar::getID() const {
	return id;
}

bool IntVar::operator==(const IntVar other) const {
	return other.hasSameSolver(*this) && getID() == other.getID();
}


}