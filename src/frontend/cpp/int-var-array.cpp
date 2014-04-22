#include <feather/frontend/cpp/int-var-array.h>
#include <feather/utils.h>
#include <common/representation.h>

namespace feather {

Solver& IntVarArray::getSolver() const {
	return slv;
}

IntVarArrayID IntVarArray::getID() const {
	return id;
}

void IntVarArray::push_front(const IntVar& var) {
	if( &var.getSolver() != &slv )
		FEATHER_THROW("Attempted to insert variable of a different solver");

	array.vars.push_front(var.getID());
}

void IntVarArray::push_back(const IntVar& var) {
	if( &var.getSolver() != &slv )
		FEATHER_THROW("Attempted to insert variable of a different solver");

	array.vars.push_back(var.getID());		
}

IntVarArray::iterator IntVarArray::begin() {
	return IntVarArray::iterator(slv, &array.vars );
}

IntVarArray::iterator IntVarArray::end() {
	return begin().end();
}

IntVar IntVarArray::operator[](const UInt index) {
	if(array.vars.size() <= index) FEATHER_THROW("Attempted out-of-bounds array access");
	return IntVar(array.vars[index], slv);
}

UInt IntVarArray::size() const {
	return array.vars.size();
}

bool IntVarArray::empty() const {
	return array.vars.empty();
}




} // namespace feather

