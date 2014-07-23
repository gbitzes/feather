#include <feather/frontend/cpp/goal-expr.h>
#include <common/representation.h>
#include <feather/frontend/cpp/solver.h>
#include <feather/frontend/cpp/int-var-array.h>

namespace feather {

Goal* InDomain(IntVar &var) {
	Goal_InDomain* ret = new Goal_InDomain(var.getID());
	return ret;
}

Goal* Labeling(IntVarArray &arr) {
	Goal_Labeling* ret = new Goal_Labeling(arr.getID());
	return ret;
}

Goal* ParallelInDomain(IntVar &var, Int limit) {
    Goal_ParallelInDomain* ret = new Goal_ParallelInDomain(var.getID(), limit);
    return ret;
}

Goal* ParallelLabeling(IntVarArray &arr, Int varlimit, Int valuelimit) {
    Goal_ParallelLabeling* ret = new Goal_ParallelLabeling(arr.getID(), varlimit, valuelimit);
    return ret;
}

}
