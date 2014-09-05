#ifndef ___FEATHER_COMMON_CONSTRAINTS_H
#define ___FEATHER_COMMON_CONSTRAINTS_H

/*
 * This file contains all the possible constraints
 * and how they're defined inside a representation
 * object
 */

#include <vector>
#include <feather/types.h>
#include <sstream>

namespace feather {

namespace Constraints {
	enum Type {
		XlessthanY = 0,
		XlesseqthanY,
		XeqAbsY,
		XeqY,
		XneqY,
		XorY,
		XeqYplusC,
		XeqYdivC,
		XeqYtimesC,
		XeqYmodC,
		XeqYplusZ,
		XeqYtimesZ,
		XeqYmodZ,
		XeqCminusZ,
		XeqCdivZ,
		XeqCmodZ,
		MetaXeqYlessthanC,
		MetaXeqYlesseqthanC,
		MetaXeqYgreaterthanC,
		MetaXeqYgreatereqthanC,
		MetaXeqYeqC,
		MetaXeqYneqC,
		MetaXeqYlessthanZ,
		MetaXeqYlesseqthanZ,
		MetaXeqYeqZ,
		MetaXeqYandZ,
		MetaXeqYorZ,
		AllDiff,
		AllDiffStrong,
		XeqSum,
		XeqElement,
		Count
	};
};

class Constraint {
	public:
		const Constraints::Type fType;
		Constraint(Constraints::Type type) : fType(type) {}
        virtual std::string serialize() const = 0;
};

/*
 * Since there's a high degree of code repetition in the constraint
 * definitions, the following code is auto-generated with
 * generate-constraint-definitions.py
 *
 * Do not edit by hand!
 */

#include <common/constraints.icc>

class Constr_Count : public Constraint {
    public:

        const IntVarArrayID fArr;
        const IntDequeID fValues, fOccurences;
        const std::vector<IntDequeID> fSplitpositions;
        const UInt fSplit;
        const UInt fDwin;

        Constr_Count(IntVarArrayID arr, IntDequeID values, IntDequeID occurences, std::vector<IntDequeID> splitPositions, UInt split, UInt dwin)
             : Constraint(Constraints::Count), fArr(arr), fValues(values), fOccurences(occurences), fSplitpositions(splitPositions), fSplit(split), fDwin(dwin) { } 
         virtual std::string serialize() const {
             std::ostringstream os;
             os << "CONSTR " << Constraints::Count << " " << fArr
                << " " << fValues
                << " " << fOccurences;
             os << fSplitpositions.size();
             for(int i = 0; i < fSplitpositions.size(); i++) os << " " << fSplitpositions[i];
             os << " " << fSplit
                << " " << fDwin;
             return os.str();
         }

        static Constraint* deserialize(std::string s) {
            std::stringstream ss(s);
            IntVarArrayID arr;
            ss >> arr;
            IntDequeID values;
            ss >> values;
            IntDequeID occurences;
            ss >> occurences;

            int count;
            ss >> count;
            std::vector<IntDequeID> splitPositions;
            for(int i = 0; i < count; i++) {
                IntDequeID tmp;
                ss >> tmp;
                splitPositions.push_back(tmp);
            }
            UInt split;
            ss >> split;
            UInt dwin;
            ss >> dwin;
            return new Constr_Count( arr,values,occurences,splitPositions,split,dwin);
        }
};


} // namespace feather
#endif

