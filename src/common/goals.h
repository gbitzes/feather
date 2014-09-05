#ifndef ___FEATHER_COMMON_GOALS_H
#define ___FEATHER_COMMON_GOALS_H

/*
 * This file contains all the possible goals
 * and how they're defined inside a representation
 * object
 */

#include <feather/types.h>
#include <sstream>

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
        virtual std::string serialize() const = 0;
};

class Goal_InDomain : public Goal {
	public:
		const IntVarID fVar;

		Goal_InDomain(const IntVarID var)
			: Goal(Goals::InDomain), fVar(var) {}
        virtual std::string serialize() const {
            std::ostringstream ss;
            ss << "GOAL " << fType << " " << fVar;
            return ss.str();
        }
        static Goal* deserialize(std::string s) {
            std::stringstream ss(s);
            IntVarID var;
            ss >> var;
            return new Goal_InDomain(var);
        }
};

class Goal_Labeling : public Goal {
	public:
		const IntVarArrayID fArr;

		Goal_Labeling(const IntVarArrayID arr)
			 : Goal(Goals::Labeling), fArr(arr) {} 
        virtual std::string serialize() const {
            std::ostringstream ss;
            ss << "GOAL " << fType << " " << fArr;
            return ss.str();
        }
        static Goal* deserialize(std::string s) {
            std::stringstream ss(s);
            IntVarArrayID arr;
            ss >> arr;
            return new Goal_Labeling(arr);
        }
};

class Goal_ParallelInDomain : public Goal {
    public:
        const IntVarID fVar;
        const Int fLimit;

        Goal_ParallelInDomain(const IntVarID var, const Int limit) 
            : Goal(Goals::ParallelInDomain), fVar(var), fLimit(limit) {}
        virtual std::string serialize() const {
            std::ostringstream ss;
            ss << "GOAL " << fType << " " << fVar << " " << fLimit;
            return ss.str();
        }
        static Goal* deserialize(std::string s) {
            std::stringstream ss(s);
            IntVarID var;
            ss >> var;
            Int limit;
            ss >> limit;
            return new Goal_ParallelInDomain(var, limit);
        }
};

class Goal_ParallelLabeling : public Goal {
    public:
        const IntVarArrayID fArr;
        const Int fVarlimit, fValuelimit;
        
        Goal_ParallelLabeling(const IntVarArrayID arr, const Int varlimit, const Int valuelimit)
            : Goal(Goals::ParallelLabeling), fArr(arr), fVarlimit(varlimit), fValuelimit(valuelimit) {}
        virtual std::string serialize() const {
            std::ostringstream ss;
            ss << "GOAL " << fType << " " << fArr << " " << fVarlimit << " " << fValuelimit;
            return ss.str();
        }
        static Goal* deserialize(std::string s) {
            std::stringstream ss(s);
            IntVarArrayID arr;
            ss >> arr;
            Int varlimit;
            ss >> varlimit;
            Int valuelimit;
            ss >> valuelimit;
            return new Goal_ParallelLabeling(arr, varlimit, valuelimit);
        }
};


} //namespace feather


#endif
