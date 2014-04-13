#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>
#include <common/representation.h>
#include <backend/naxos/naxos.h>
#include <base/utils.h>

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

RepresentationIntVarArray& Solver::getArray(IntVarArrayID id) {
	return *(repr->arrays[id]);
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

void Solver::addConstraint(const Constraint *const constr) {
	repr->constraints.push_back(constr);
}

bool Solver::nextSolution() {
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

void Solver::addGoal(Goal *goal) {
	repr->goals.push_back(goal);
}

SolutionID Solver::getSolutionID() {
	return currentSolutionID;
}

}
