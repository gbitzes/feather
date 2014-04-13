#ifndef FEATHER_FRONTEND_CPP_GOALEXPR_H
#define FEATHER_FRONTEND_CPP_GOALEXPR_H


namespace feather {

class IntVar;
class Goal;
class IntVarArray;

Goal* InDomain(IntVar &var);
Goal* Labeling(IntVarArray &arr);

}

#endif
