#include <common/create-constraint.h>

namespace feather {

Constraint __create_XeqYplusC(IntVarID X, IntVarID Y, Int C) {
	Constraint ret;
	ret.type = ConstraintType::XeqYplusC;
	ret.values.resize(3);

	ret.values[0] = X;
	ret.values[1] = Y;
	ret.values[2] = C;

	return ret;
}

}

