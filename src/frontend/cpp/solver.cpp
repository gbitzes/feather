#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <common/representation.h>
#include <backend/naxos/naxos.h>
#include <feather/utils.h>

namespace feather {

Solver::Solver(ProblemManager *pm) : fPM(pm) {
	repr = new Representation();
	finalized = false;
	solving = false;
}

Solver::~Solver() {
	delete repr;
	delete fPM;
}

IntVarID Solver::makeIntVar(Int min, Int max) {
	return repr->addVar(min, max);
}

IntVarArrayID Solver::makeIntVarArray() {
	return repr->addArray();
}

IntDequeID Solver::makeIntDeque(UInt size) {
	return repr->addDeque(size);
}

RepresentationIntVarArray& Solver::getArray(IntVarArrayID id) {
	return *(repr->arrays[id]);
}

RepresentationIntDeque& Solver::getDeque(IntDequeID id) {
	return *(repr->deques[id]);
}

IntDomain& Solver::getRepresentationDomain(IntVarID id) {
	return *repr->vars[id].domain;
}

IntDomain* Solver::getDomain(IntVarID id) {
	if(!solving)
		return repr->vars[id].domain;
	else 
		return fPM->getDomain(id);
}

void Solver::add(const Constraint *const constr) {
	/* 
	 * There is a valid case in which the constraint
	 * is "static" - all changes can be performed just
	 * by modifying the domain and nothing more needs
	 * to be passed on to the backend. In this case,
	 * a NULL constraint is returned by the expressions
	 */

	if(constr != 0)
		repr->constraints.push_back(constr);
}

bool Solver::nextSolution() {
	if(!finalized)
		finalize();

	solving = true;
	bool ret = fPM->nextSolution();
	if(ret)
		currentSolutionID++;

	return ret;
}

void Solver::finalize() {
	finalized = true;
	fPM->supplyRepresentation(*repr);
}

void Solver::minimize(const IntVar var) {
	if(repr->minObj != -1)
		FEATHER_THROW("Already set minimization target");

	repr->minObj = var.getID();
}

bool Solver::isFinalized() {
	return finalized;
}

void Solver::addGoal(Goal *goal) {
	repr->goals.push_back(goal);
}

SolutionID Solver::getSolutionID() {
	return currentSolutionID;
}

}
