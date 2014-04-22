#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <feather/utils.h>
#include <feather/int-domain.h>

#define ASSERT_INITIALIZED() if(!initialized) FEATHER_THROW("IntVar has not been initialized")

namespace feather {

void IntVar::initialize(Solver &_slv, IntVarID id) {
	initialized = true;
	this->slv = &_slv;
	this->id = id;
	representationDomain = &slv->getRepresentationDomain(id);
	domain = representationDomain;
	currentSolutionID = 0;
	updateDomain();
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
	ASSERT_INITIALIZED();
	updateDomain();
	return domain->min();
}

Int IntVar::max() {
	ASSERT_INITIALIZED();
	updateDomain();
	return domain->max();
}

Int IntVar::value() {
	ASSERT_INITIALIZED();
	updateDomain();
	if(domain->max() == domain->min() )
		return domain->max();

	FEATHER_THROW("Variable is not bound to a single value");
}

IntDomain const* IntVar::getDomain() const {
	ASSERT_INITIALIZED();
	return domain;
}

void IntVar::updateDomain() {
	ASSERT_INITIALIZED();
	if( slv->getSolutionID() != currentSolutionID ) {
		if(domain != representationDomain)
			delete domain;

		domain = slv->getDomain(id);
	}
}

void IntVar::remove(Int from, Int to) {
	ASSERT_INITIALIZED();
	if(slv->isFinalized() )
		FEATHER_THROW("Problem has been finalized - not possible to make changes");
	if(from > to)
		FEATHER_THROW("Invalid range");
	if( representationDomain->removeRange(from, to) == IntDomain::WOULDCLEAR )
		FEATHER_THROW("Removal of values would give an empty domain");
}

void IntVar::remove(Int val) {
	ASSERT_INITIALIZED();
	remove(val,val);
}

bool IntVar::contains(Int val) const {
	ASSERT_INITIALIZED();
	return representationDomain->contains(val);
}

IntVarID IntVar::getID() const {
	ASSERT_INITIALIZED();
	return id;
}

bool IntVar::equals(const IntVar other) const {
	ASSERT_INITIALIZED();
	return other.hasSameSolver(*this) && getID() == other.getID();
}

std::string IntVar::toString() {
	updateDomain();
	return domain->toString();
}

IntVar::iterator IntVar::begin() {
	ASSERT_INITIALIZED();
	return domain->begin();
}

IntVar::iterator IntVar::end() {
	ASSERT_INITIALIZED();
	return domain->end();
}


}