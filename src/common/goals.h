#ifndef ___FEATHER_COMMON_GOALS_H
#define ___FEATHER_COMMON_GOALS_H

/*
 * This file contains all the possible goals
 * and how they're defined inside a representation
 * object
 */

#include <feather/types.h>

namespace feather {

namespace Goals {
	enum Type {
		InDomain = 0,
		Labeling,
        ParallelInDomain,
        ParallelLabeling
	};
};

class Goal {
	public:
		const Goals::Type fType;
		Goal(Goals::Type type) : fType(type) {}
};

class Goal_InDomain : public Goal {
	public:
		const IntVarID fVar;

		Goal_InDomain(const IntVarID var)
			: Goal(Goals::InDomain), fVar(var) {}
};

class Goal_Labeling : public Goal {
	public:
		const IntVarArrayID fArr;

		Goal_Labeling(const IntVarArrayID arr)
			 : Goal(Goals::Labeling), fArr(arr) {} 
};

class Goal_ParallelInDomain : public Goal {
    public:
        const IntVarID fVar;
        const Int fLimit;

        Goal_ParallelInDomain(const IntVarID var, const Int limit) 
            : Goal(Goals::ParallelInDomain), fVar(var), fLimit(limit) {}
};

class Goal_ParallelLabeling : public Goal {
    public:
        const IntVarArrayID fArr;
        const Int fVarlimit, fValuelimit;
        
        Goal_ParallelLabeling(const IntVarArrayID arr, const Int varlimit, const Int valuelimit)
            : Goal(Goals::ParallelLabeling), fArr(arr), fVarlimit(varlimit), fValuelimit(valuelimit) {}
};


} //namespace feather


#endif
