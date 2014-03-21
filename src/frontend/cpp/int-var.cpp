#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/solver.h>

namespace feather {

/*
 * Initialize an IntVar by creating a new variable
 * inside representation
 */

IntVar::IntVar(Solver &_slv, Int min, Int max) {
	initialized = true;
	this->slv = &_slv;
	id = slv->makeIntVar(min, max);
	domain = &slv->getDomain(id);
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


}