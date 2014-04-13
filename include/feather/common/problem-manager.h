#ifndef FEATHER_COMMON_PROBLEMMANAGER_H
#define FEATHER_COMMON_PROBLEMMANAGER_H

#include <feather/types.h>
#include <vector>

namespace feather {

class Representation;

/*
 * The basic interface from which middle managers 
 * and backends inherit
 *
 * It defines methods with which to supply the representation
 * of the problem and receive all results
 */

class ProblemManager {

	public:
		virtual void supplyRepresentation(const Representation &) = 0;
		virtual void clearRepresentation() = 0;
		virtual bool nextSolution() = 0;

		/* Caller responsible to delete given object */
		virtual IntDomain* getDomain(IntVarID) = 0;
};

/*
 * A special case of ProblemManager that
 * manages other ProblemManagers
 */

class IntermediateManager : public ProblemManager {
	public:
		virtual Int getMinObjValue() = 0;
		virtual void updateMinObjValue(Int) = 0;
		virtual void newInstance(std::vector<bool> decisions) = 0;
		virtual bool needMoreWork() = 0;
};


} // namespace feather



#endif