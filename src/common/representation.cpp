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

Representation::~Representation() {
	
	{
		std::vector<RepresentationIntVar>::iterator it;
		for(it = vars.begin(); it != vars.end(); it++)
			delete it->domain;
	}
	{
		std::vector<const Constraint*>::iterator it;
		for(it = constraints.begin(); it != constraints.end(); it++)
			delete *it;
	}
}

} // namespace feather
