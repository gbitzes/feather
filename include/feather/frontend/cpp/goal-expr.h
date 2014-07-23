#ifndef FEATHER_FRONTEND_CPP_GOALEXPR_H
#define FEATHER_FRONTEND_CPP_GOALEXPR_H

#include <feather/types.h>

namespace feather {

class IntVar;
class Goal;
class IntVarArray;

Goal* InDomain(IntVar &var);
Goal* Labeling(IntVarArray &arr);
Goal* ParallelInDomain(IntVar&, Int);
Goal* ParallelLabeling(IntVarArray&, Int, Int);

}

#endif
