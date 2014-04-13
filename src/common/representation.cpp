#include "representation.h"
#include <base/utils.h>
#include <vector>

namespace feather {

IntVarID Representation::addVar(Int min, Int max) {
	IntVarID id = vars.size();
	IntRanges *domain = new IntRanges(min, max);

	RepresentationIntVar r;
	r.id = id;
	r.domain = domain;

	vars.push_back(r);

	return id;
}

IntVarArrayID Representation::addArray() {
	IntVarArrayID id = arrays.size();
	RepresentationIntVarArray *r = new RepresentationIntVarArray();
	r->id = id;
	arrays.push_back(r);
	return id;
}

Representation::~Representation() {
	/* Delete vars */
	{
		std::vector<RepresentationIntVar>::iterator it;
		for(it = vars.begin(); it != vars.end(); it++)
			delete it->domain;
	}
	/* Delete constraints */
	{
		std::vector<const Constraint*>::iterator it;
		for(it = constraints.begin(); it != constraints.end(); it++)
			delete *it;
	}
	/* Delete arrays */
	{
		std::vector<RepresentationIntVarArray*>::iterator it;
		for(it = arrays.begin(); it != arrays.end(); it++)
			delete *it;
	}
}

} // namespace feather
