#include "representation.h"
#include <base/utils.h>

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

} // namespace feather
