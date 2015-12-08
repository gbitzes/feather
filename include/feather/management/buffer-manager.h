#ifndef FEATHER_MANAGEMENT_BUFFERMANAGER_H
#define FEATHER_MANAGEMENT_BUFFERMANAGER_H

#include <feather/common/problem-manager.h>
#include <feather/management/manager-generator.h>
#include <queue>
#include <map>
#include <semaphore.h>

namespace feather {

class Representation;

class BufferManager : public IntermediateManager {
	private:
        //Int minObjValue;
		Representation const* representation;
        ParentManager* parent;
        ChildManager* child;

        std::vector< std::map<Int, IntDomain* > > safe;
        Int bufsize;
        bool firstSolution, endofsearch;

        SearchState state;
        void cleanup();
	public:
        BufferManager(Int bufsize, ChildManager *child);
        ~BufferManager();

		virtual void supplyRepresentation(const Representation &);
		virtual void clearRepresentation();
		virtual bool nextSolution();

		/* Caller responsible to delete given object */
		virtual IntDomain* getDomain(IntVarID);

		virtual Int getMinObjValue();
		virtual void updateMinObjValue(Int);
		virtual void newInstance(const SearchState &state);
        virtual void setInitialState(const SearchState &state);
        virtual void setParent(ParentManager*);
		virtual bool needMoreWork();
        virtual void restart();
};

} // namespace feather

#endif
