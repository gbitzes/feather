#ifndef FEATHER_BACKEND_NAXOS_H
#define FEATHER_BACKEND_NAXOS_H

#include <common/representation.h>
#include <feather/common/problem-manager.h>
#include <backend/naxos/NsIntVar.h>
#include <backend/naxos/NsGoal.h>
#include <backend/naxos/NsDataStructures.h>
#include <backend/naxos/basics.h>
#include <map>
#include <fstream>

namespace feather {

class NsIntVar;
class Ns_Constraint;

/* 	The type of the objects in the queue for the AC algorithm */

class Ns_QueueItem {
	private:		
		/* The domain of the variable that has been modified */
		NsIntVar *varFired;

		/* The index of the constraint to check */
		std::deque<NsIntVar::ConstraintAndFailure>::size_type currentConstr;

		struct RemovedValueRecord_t;

		/* The index of the removed value that will be checked against constraints (that require arc consistency) */
		std::deque<RemovedValueRecord_t>::size_type currentRemovedValue;

		/*  Holds information having to do with the modification (if any) of the bounds of varFired */
		struct RemovedBoundRecord_t {
			/*  True if the minimum or maximum of the domain of varFired has been changed */
			bool removedBound;

			/* The constraint that fired the last modification of the bounds of varFired.  
		   	   If no constraint provoked the removal, then constrFired==0 */
			
			const Ns_Constraint *constrFired;

			/* The domain removal serial number */
			unsigned long removalTime;
			
			/* Constructor */
			RemovedBoundRecord_t() : removedBound(false) { }

			/* Records a bounds modification */
			void boundChangedBy(const Ns_Constraint *constrFired_init, const unsigned long removalTime_init) {
				constrFired = constrFired_init;
				removedBound = true;
				removalTime = removalTime_init;
			}
		};

		/* Used to record a modification of the bounds (if any) */
		RemovedBoundRecord_t removedBoundRec;

		/* Describes the removal of the member removedValue from the domain of varFired
		   (Contains the value that was removed, and which constraint did it) */

		struct RemovedValueRecord_t {
			/* The value that was taken from the domain of varFired.  (The `w' of the AC-5 algorithm.) */
			const Int value;

			/* The constraint that fired the removal of the value removedValue from the domain of the variable varFired.  
		   	   If no constraint provoked the removal, then constrFired==0 */
			
			const Ns_Constraint *constrFired;

			/* Constructor */
			RemovedValueRecord_t(const Int removedValue_init, const Ns_Constraint *constrFired_init) 
				: value(removedValue_init),
		   		constrFired(constrFired_init) { }
		};
		
		/* An array that records the values removed from the domain of varFired */
		std::deque<RemovedValueRecord_t> removedValues;
	public:

		/* Constructor */
		Ns_QueueItem (NsIntVar *varFired_init)
			: varFired(varFired_init),
			currentConstr(0),
			currentRemovedValue(0) { }

		~Ns_QueueItem() {
			if(varFired->queueItem == this) {

				//  Is there any possibility of
				//   varFired->queueItem != this ?  Yes, when there
				//   are two items in AC queue for `varFired', the
				//   first being examined now (by the AC
				//   algorithm).

				varFired->queueItem = 0;
			}
		}

		void boundChangedBy(const Ns_Constraint *constr);
		void add(const Int removedVal, const Ns_Constraint *constrThatRemovedIt);
		Ns_Constraint* getNextConstraint();

		/* Returns the variable that fired the constraint propagation */
		NsIntVar* getVarFired() const {
			return varFired;
		}

		/* Returns the value that has been removed from the variable */
		Int getW() const {
			return removedValues[currentRemovedValue-1].value;
		}

		/* When a constraint provokes an inconsistency, then its rank 
		   (index in the `varFired->constraints' array) should be updated 
		   according to the current number of inconsistencies it provoked 
		   (according to a heuristic) */
};

	class Ns_SearchNode;


	/* Contains all the Ns_SearchNodes
	   A Ns_SearchNode contains the current status of the problem; it is
	   pushed into the stack when we make a choice (e.g. when we select a
	   specific value to assign it to a variable) and it is popped when we
	   want to cancel this choice, and we want to revert back to the
	   previous problem status */

class Ns_StackSearch : public NsStack<Ns_SearchNode>  {

	public:

		Ns_StackSearch (void);
		~Ns_StackSearch (void);
		void  clear (void);

		///  @{
		///  \name  Provision of history ids to the search nodes

	private:

		///  History-ids together with time statistics.

		struct  history_time_t  {

				///  An ID for the current history level.
				UInt  validHistoryId;

				///  Constructor.
				history_time_t (void) : validHistoryId(0) {

				}

				///  Augments the valid history ID.
				void invalidate (void) {
					++validHistoryId;
				}

		};

		std::deque<history_time_t>  history_time;


		unsigned long  nSearchTreeNodes;


	public:


		unsigned long numSearchTreeNodes (void)  const {
			return  ( nSearchTreeNodes - 1 );
		}

		void  push (const value_type& newNode);
		void  pop (void);

		///  Restores the validHistoryId 's state as it was before search began.
		void reset (void) {
			history_time[0].validHistoryId  =  0;
		}


		Ns_HistoryId_t getCurrentHistoryId (void)  const {
			Ns_HistoryId_t  di;

			di.level  =  size() - 1;
			di.id     =  history_time[di.level].validHistoryId;

			return  di;
		}


		bool isCurrentHistoryId (const Ns_HistoryId_t di)  const {
			return  ( di.level == size() - 1
			          && di.id == history_time[di.level].validHistoryId );
		}


		bool isValidHistoryId (const Ns_HistoryId_t di)  const {
			return  ( di.id  ==  history_time[di.level].validHistoryId );
		}

		///  @}



		///  @{
		///  \name  Representation of the search tree as a graph

	private:

		///  File to store the search tree graph.
		std::ofstream  fileSearchGraph;

		///  True if it should write the objective value.
		bool       recordObjective;

		///  The last recorded objectiveValue.
		Int      objectiveValue;

	public:

		void  searchToGraphFile (const char *fileName);

		void  solutionNode (NsIntVar *vObjective);

		///  @}



		//friend class goal_iterator;


		///  Iterates through all the goals in the current Ns_GoalStack and the Ns_GoalStack below it.

		///  (All of them consist a stack of Ns_GoalStack, named
		///   \c stackOfStacks.)

		class  goal_iterator  {

			private:

				///  A stack containing the search nodes (so each frame of this stack contains a Ns_GoalStack).
				Ns_StackSearch  *stackOfStacks;

				///  Points to the current search node.
				Ns_StackSearch::iterator  curr_Stack_it;

				///  Points to the current goal (in the Ns_GoalStack) of the current search node.
				Ns_GoalStack::iterator   curr_node_it;


			public:

				goal_iterator (void)
					: stackOfStacks(0)
				{    }


				goal_iterator (Ns_StackSearch& stackOfStacks_init);


				bool  operator == (const goal_iterator& b)  const
				{
					FEATHER_ASSERT(stackOfStacks != 0);
					return  ( curr_Stack_it == b.curr_Stack_it
							   &&  (  curr_Stack_it == stackOfStacks->end()
								|| curr_node_it == b.curr_node_it ) );
				}


				bool  operator != (const goal_iterator& b)  const
				{
					return  ! ( *this  ==  b );
				}


				NsGoal*  operator * (void)  const
				{
					FEATHER_ASSERT(stackOfStacks != 0);
					FEATHER_ASSERT(curr_Stack_it != stackOfStacks->end()
							/*&&  curr_node_it != curr_Stack_it->stackAND.end()*/);

					return  *curr_node_it;
				}


				goal_iterator&  end (void)
				{
					FEATHER_ASSERT(stackOfStacks != 0);

					curr_Stack_it = stackOfStacks->end();
					return  *this;
				}


				goal_iterator&  operator ++ (void);
		};


		goal_iterator  gbegin (void)
		{
			return  goal_iterator(*this);
		}


		goal_iterator  gend (void)
		{
			goal_iterator  iterEnd(*this);
			return  iterEnd.end();
		}
};

///  Describes a search node of the binary search tree.
///  A critical type that can describe the current status of the problem.

struct  Ns_SearchNode  {

	public:

		///  Alternative goal to satisfy if search fails.
		NsGoal  *goalNextChoice;

		/* 
		 * Number of OR-goal decisions made in this search node 
     * (or alternatively, the number of OR-decisions we need to
        reverse to go back to the previous state)
		 */
		Int numDecisions;

		Int searchSpaceEstimate;

		///  Pointer to the first goal of the previous search level that has not been yet satisfied.
		Ns_StackSearch::goal_iterator  delayedGoal;

		/* All the goals of this list should be satisfied */
		Ns_GoalStack   stackAND;

		/* The best objective value possible if we start from this node */
		Int bestObjValuePossible;


		///  Constructor.
		Ns_SearchNode (NsGoal *goalNextChoice_init,
				Ns_StackSearch::goal_iterator git)
		 : goalNextChoice(goalNextChoice_init),
		   delayedGoal(git),
		   has_a_child(false),
		   numDecisions(0)
		{    }


	private:

		/* Store that contains all domains that have been modified because of this search node */
		class DomainStore : public NsStack<IntDomain*> {
			public:
				/* Restores all the saved bit-set domains */
				void restore() {
					while ( ! empty() )   {
						top()->restore();
						pop();
					}
				}
		};

	public:

		/* Store to keep track of the bitsets that were modified during this searchNode, so
		   that it is possible to backtrack */
		
		DomainStore domainStore;

		///  True, if the current node creates another OR-node.
		bool  has_a_child;

};



typedef enum { OK=1, NO_SOLUTIONS, ONLY_ONE_SOLUTION  } pruneResult_t;

///  Constraint satisfaction problem manager and solver class.

///  A problem manager keeps information for the constrained variables, the
///   constraints network, and the goal that will be executed.

class Naxos : public ProblemManager {

	private:
		/* The stack that contains the choice points that have appeared during search
		 * 
		 * Each time the solver encounters an OR-goal, a new item is pushed into searchNodes
		 * that keeps track of all necessary information to backtrack, as well as the alternative
		 * goal to satisfy in case the first choice in the OR-goal fails
		 *
		 * (for example it keeps track of which bitsets have been modified since the solver met the OR-node)
		 */

		Ns_StackSearch searchNodes;

		/* The AC algorithm event queue that keeps track of value and bounds modifications since
		 * the last time imposeArcConsistency() was called
		 * 
		 * Calling imposeArcConsistency() will empty this queue
		 */
		
		NsQueue<Ns_QueueItem>  Q;

	public:
		NsIntVar* makeVar(Int min, Int max);

	private:
		/* Specifies whether a representation has already been provided */
		bool hasRepresentation;

		std::map<IntVarID, NsIntVar*> vars;
		std::map<IntVarArrayID, NsIntVarArray*> vararrays;

		std::vector<Ns_Constraint*> constraints;
		std::map<IntDequeID, NsDeque<Int>*> deques; 

		void addVar(const RepresentationIntVar&);
		void addArray(const RepresentationIntVarArray&);
		void addDeque(const RepresentationIntDeque&);
		

		Ns_Constraint* addConstraint(const Constraint&);
		NsGoal* convertGoal(const Goal&);

	private:

		///  True if the available time is real (i.e.\ not system time).
		bool     isRealTime;

		///  The first (system) time that nextSolution() has been called.
		clock_t  startTime;

		///  The first time that nextSolution() has been called.
		time_t   startRealTime;

		///  Available time (for running the search process) in seconds.
		unsigned long  timeLim;

		///  True if nextSolution() has not been yet called.
		bool  firstNextSolution;

		///  True if a limit to the available time (for search process) has been set.
		bool  calledTimeLimit;

	private:
		/* Some optimization problems require that we find the best solution according to some criterion,
		 * for example the minimum distance or the minimum cost.

		 * This variable is our objective that needs to be minimized
		 */

		NsIntVar *vMinObj;

		/* The minimum upper limit for vMinObj foudn so far (initially +oo) */
		Int bestMinObjValue;

		unsigned long  nFailures, nBacktracks, nGoals,
					   nConstraintChecks, //nRemovals,
					   backtrackLim;

	private:
		IntermediateManager *parent;

		UInt iterations;
		UInt donatedGoals;

		std::vector<bool> decisions;
		std::vector<bool> initialDecisions;

		bool backtrack();
		void restart();
		void start();

		pruneResult_t prune(std::vector<bool> *decisions);
		void giveupWork();

		/* The list of the soft (meta)constraints to be satisfied */
		NsIntVarArray vSoftConstraintsTerms;

	public:
		///  Returns the AC algorithm queue.
		NsQueue<Ns_QueueItem>& getQueue (void) {
			return  Q;
		}

		void minimize (NsIntVar& VarObj) {
			vMinObj          =  &VarObj;
			bestMinObjValue  =  kPlusInf;
		}


	public:
		virtual void supplyRepresentation(const Representation&);
		virtual void clearRepresentation();	

		Naxos();
		virtual ~Naxos();
		virtual bool nextSolution();
		virtual IntDomain* getDomain(IntVarID);

		///  True, if an inconsistency has been found.
		bool foundInconsistency;


		///  File to store the constraint network graph.
		std::ofstream fileConstraintsGraph;

		///  Writes to a file a view of the constraint network in a Graphviz supported format.
		void constraintsToGraphFile(const char *fileName);

		bool imposeArcConsistency();

		unsigned long
		numConstraintChecks (void)  const
		{
			return  nConstraintChecks;
		}

		/* Informs NsProblemManager than an inconsistency has been found */
		void foundAnInconsistency (void) {
			foundInconsistency = true;
		}

			Ns_HistoryId_t
		getCurrentHistoryId (void)  const
		{
			return  searchNodes.getCurrentHistoryId();
		}


			bool
		isCurrentHistoryId (const Ns_HistoryId_t di)  const
		{
			return  searchNodes.isCurrentHistoryId(di);
		}


			bool
		isValidHistoryId (const Ns_HistoryId_t di)  const
		{
			return  searchNodes.isValidHistoryId(di);
		}
		
		/* Record that the bitsetDomain has been modified, so it can be restore()d later */
		void domainModified (IntDomain* domain) {
			searchNodes.top().domainStore.push( domain );
		}





};


} // namespace feather


#endif
