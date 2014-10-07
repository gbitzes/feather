#ifndef FEATHER_COMMON_PROBLEMMANAGER_H
#define FEATHER_COMMON_PROBLEMMANAGER_H

#include <feather/types.h>
#include <vector>

namespace feather {

class Representation;

/*
 * The basic interface for an object that
 * declares "Hey, I can solve a CSP problem"
 * but no guarantees that can have a parent
 * or children
 */

class ProblemManager {

	public:
        virtual ~ProblemManager() {} 
		virtual void supplyRepresentation(const Representation &) = 0;
		virtual void clearRepresentation() = 0;
		virtual bool nextSolution() = 0;
        virtual void restart() = 0;

		/* Caller responsible to delete given object */
		virtual IntDomain* getDomain(IntVarID) = 0;
};


class ChildManager;

/*
 * A ProblemManager that can manage chlidren
 */
class ParentManager : public virtual ProblemManager {
	public:
		virtual Int getMinObjValue() = 0;
		virtual void updateMinObjValue(Int) = 0;
		virtual void newInstance(std::vector<bool> decisions) = 0;
		virtual bool needMoreWork() = 0;
};

/*
 * A ProblemManager that can be managed by parents
 */
class ChildManager : public virtual ProblemManager {
	public:
		virtual void setParent(ParentManager*) = 0;
		virtual void setInitialDecisions(std::vector<bool>) = 0;
};

/*
 * A problem manager that can act both as a child
 * and parent
 */

class IntermediateManager : public ParentManager, public ChildManager { };


} // namespace feather



#endif
