#include "naxos.h"
#include <common/representation.h>
#include <feather/utils.h>
#include <base/bitset.h>
#include <vector>
#include "NsIntVar.h"
#include <backend/naxos/Ns_Constraint.h>
#include <backend/naxos/NsGoal.h>

namespace feather {

using namespace std;








Ns_StackSearch::goal_iterator::goal_iterator (Ns_StackSearch& stackOfStacks_init)
	: stackOfStacks(&stackOfStacks_init),
	  curr_Stack_it(stackOfStacks->begin())
{
	if ( curr_Stack_it  !=  stackOfStacks->end()
	    && (curr_node_it = curr_Stack_it->stackAND.begin())  ==  curr_Stack_it->stackAND.end() )
	{
		*this  =  curr_Stack_it->delayedGoal;
	}
}





Ns_StackSearch::goal_iterator&
Ns_StackSearch::goal_iterator::operator ++ (void)
{
	FEATHER_ASSERT( stackOfStacks != 0);
	FEATHER_ASSERT( curr_Stack_it != stackOfStacks->end()
			&& curr_node_it != curr_Stack_it->stackAND.end() );

	if ( ++curr_node_it  ==  curr_Stack_it->stackAND.end() )
		*this = curr_Stack_it->delayedGoal;

	return  *this;
}








///  Writes to a file a view of the search tree in a Graphviz supported format.

void Ns_StackSearch::searchToGraphFile (const char *fileName) {
	fileSearchGraph.open(fileName);
	if(fileSearchGraph == NULL)
		FEATHER_THROW("Could not open file");

	fileSearchGraph << "digraph  \"Search Tree\"  {\n\n"
	                << "\tnode [shape=point];\n\n"
		        << "\tedge [arrowhead=none];\n";
}

///  If it does not exist, creates a \c validHistoryId entry for the current search node.

    void
Ns_StackSearch::push (const value_type& newNode)
{
	++nSearchTreeNodes;

	if ( ! empty() )   {

		top().has_a_child  =  true;

		//top().timePrevious  =  clock();
	}

	// push_back(newNode);

	NsStack<Ns_SearchNode>::push(newNode);

	if ( history_time.size()  <  size() )
		history_time.push_back( history_time_t() );
	//else
	//	history_time[size()-1].validate();
}




///  Records the solution node to the goals graph file (if created).

    void
Ns_StackSearch::solutionNode (NsIntVar *vObjective)
{
	if ( fileSearchGraph )    {

		fileSearchGraph << "\n\t\"("
			<< size()-1 << ","
			<< history_time[size()-1].validHistoryId
			<< ")"
			<< ( ( top().has_a_child ) ? "LastChild" : "" )
			<< "\" [shape=doublecircle, height=0.1, label=\"\"];\n";

		//  If the node has children, and it is a solution, then it
		//   it is the last child of itself.  (Besides, after the
		//   success it will be popped by the backtracking
		//   algorithm.)

		if ( vObjective  !=  0 )    {

			recordObjective  =  true;

			objectiveValue   =  vObjective->max();
		}
	}
}




///  Invalidates the \c validHistoryId for the current search node.

	void
Ns_StackSearch::pop (void)
{
	//clock_t  timeNow = clock();

	if ( fileSearchGraph  &&  size() - 1 > 0  &&  top().has_a_child )  {

		fileSearchGraph << "\n\t\"("
			<< size()-1 << ","
			<< history_time[size()-1].validHistoryId << ")\"   ->   \"("
			<< size()-1 << ","
			<< history_time[size()-1].validHistoryId << ")LastChild\"";


		if ( recordObjective )    {

			recordObjective  =  false;

			fileSearchGraph << "   [fontsize=9";

			fileSearchGraph << ", headlabel=\""
				<< objectiveValue << "\"";

			fileSearchGraph //<< ", label=\""
				//<< timeNow - top().timePrevious
				<< "\"]";
		}

		fileSearchGraph << ";\n";

		//  If the node has children, then it it is the last child
		//   of itself.  (Besides, the `goalNextChoice'--the second
		//   subgoal of an OR-goal--is executed one level before
		//   the execution of the first subgoal, in the search
		//   tree.)
	}

	if ( fileSearchGraph  &&  size() - 1 > 1 )    {

		fileSearchGraph << "\n\t\"("
			<< size()-2 << ","
			<< history_time[size()-2].validHistoryId << ")\"   ->   \"("
			<< size()-1 << ","
			<< history_time[size()-1].validHistoryId << ")\"";


		if ( recordObjective )    {

			recordObjective  =  false;

			fileSearchGraph << "   [fontsize=9";

			fileSearchGraph << ", headlabel=\""
				<< objectiveValue << "\"";

			fileSearchGraph //<< ", label=\""
				//<< timeNow - top().timePrevious
				<< "\"]";
		}

		fileSearchGraph << ";\n";
	}


	history_time[ size() - 1 ].invalidate();

	NsStack<Ns_SearchNode>::pop();
	// pop_back();
	
}

///  Writes to a file a view of the constraint network in a Graphviz supported format.

	void
Naxos::constraintsToGraphFile (const char *fileName)
{
	fileConstraintsGraph.open(fileName);

	if(fileConstraintsGraph == NULL)
		FEATHER_THROW("Could not open file");

	fileConstraintsGraph
		<< "digraph  \"Constraint Network\"  {\n\n"
		<< "\tnode [shape=plaintext, fontsize=13, height=0.05];\n\n"
		<< "\tedge [arrowsize=0.5, fontsize=10];\n";


	fileConstraintsGraph << "\n\n\n\t/*  Variables  */\n";


	// for (std::deque<NsIntVar*>::const_iterator
	// 		v = vars.begin();
	// 		v != vars.end();
	// 		++v)
	// {
	// 	fileConstraintsGraph << "\n\tVar" << *v
	// 		<< "  [label=\"["
	// 		<< (*v)->min();

	// 	if ( ! (*v)->isBound() )
	// 		fileConstraintsGraph << ".." << (*v)->max();

	// 	fileConstraintsGraph << "]\"];\n";

	// 	//fileConstraintsGraph << "\n\t/* Constraints: "
	// 	//	<< (*v)->constraints.size() << " */\n";
	// }


	// fileConstraintsGraph << "\n\n\n\t/*  Intermediate Variables (drawn with a smaller font)  */\n";


	// for (NsDeque<NsIntVar*>::const_iterator
	// 		v = intermediateVars.begin();
	// 		v != intermediateVars.end();
	// 		++v)
	// {
	// 	fileConstraintsGraph << "\n\tVar" << *v
	// 		<< "  [fontsize=9];\n";

	// 	//fileConstraintsGraph << "\n\t/* Constraints: "
	// 	//	<< (*v)->constraints.size() << " */\n";
	// }


	// fileConstraintsGraph << "\n\n\n\t/*  Constraints  */\n";


	// for (Ns_constraints_array_t::iterator  c = constraints.begin();
	// 		c != constraints.end();
	// 		++c)
	// {
	// 	(*c)->toGraphFile(fileConstraintsGraph);
	// }


	// fileConstraintsGraph << "}\n";


	fileConstraintsGraph.close();
}








///  Fetches the next constraint to currentConstr, that concerns the variable varFired.

///  \remarks
///  \li
///  In implementing the queue, to reduce the number of queue operations,
///   one way is simply enqueue the variable whose domain has shrunk,
///   instead of enqueue all relevant arcs. When we dequeue a variable from
///   the queue, we just revise all constraints pointing to this variable.
///   The method is also called variable oriented propagation. (This idea
///   appeared in [McG79] and in [CJ98].)
///  Source: <em>An Optimal Coarse-grained Arc Consistency Algorithm,</em>
///   by C.\ Bessiere et al.
///
///  \li
///  Strictly speaking, in AC-3, arc (\a i, \a j) is not enqueued when arc
///   (\a j, \a i) is made consistent. This optimization could be added in
///   AC-5 by adding \a j as an argument to Enqueue and adding the
///   constraint \a k != \a j to its definition.
///  Source: <em>A Generic Arc-Consistency Algorithm and its
///   Specializations,</em> by P.\ van Hentenryck et al.
///
///
///\note
///
///\code
///NsIntVar  V1(pm,0,0), V2(pm,0,1);
///
///for (int i=0;  i < 640;  ++i)
///	pm.add( V1  <  V2 );
///
///pm.nextSolution();
///\endcode
///
///  The following, commented-out code did not work with the above code
///   (that a Solver's programmer could develop).  We think that the culprit
///   is an STL bug...
///
///\code
///Ns_Constraint  *temp;
///
///for ( ;  currentConstr != varFired->constraints_begin();  ++currentConstr)
///{
///	temp  =  *currentConstr;
///
///	if ( temp  !=  constrFired )    {
///
///		++currentConstr;
///		return  temp;
///	}
///}
///\endcode


Ns_Constraint* Ns_QueueItem::getNextConstraint()
{
	////  The k-bounds-consistency level:
	//const NsUInt  k = 25;


	for ( ;  currentConstr < varFired->constraints.size();  ++currentConstr)   {

		switch ( varFired->constraints[currentConstr].constr->revisionType )   {

			case  Ns_Constraint::VALUE_CONSISTENCY :

				do {
					++currentRemovedValue;

				} while ( currentRemovedValue - 1 < removedValues.size()
					&&  removedValues[currentRemovedValue-1].constrFired
						== varFired->constraints[currentConstr].constr );

				if ( currentRemovedValue - 1  ==  removedValues.size() )
					currentRemovedValue  =  0;
				else
					return  varFired->constraints[currentConstr].constr;

				break;


			case  Ns_Constraint::BOUNDS_CONSISTENCY :
				
				if ( removedBoundRec.removedBound
					&& varFired->constraints[currentConstr].constr  !=
								removedBoundRec.constrFired  )
				{	//  No need to check the constraint that initiated the propagation.
					return  varFired->constraints[currentConstr++].constr;
				}

				break;


			case  Ns_Constraint::BIDIRECTIONAL_CONSISTENCY :

				if ( removedBoundRec.removedBound
					&& removedBoundRec.removalTime  >=
						varFired->constraints[currentConstr].constr->lastConstraintCheckTime )
				{
					return  varFired->constraints[currentConstr++].constr;
				}

				break;

			default:

				FEATHER_THROW("Ns_QueueItem::getNextConstraint: Invalid `constr->revisionType'");
				break;
		};
	}

	return  0;
}




namespace  {

		void
	destroy_goal (NsGoal* g)
	{
		if ( g  !=  0 )    {

			if ( g->isGoalAND()  ||  g->isGoalOR() )   {

				destroy_goal( g->getFirstSubGoal() );

				destroy_goal( g->getSecondSubGoal() );
			}

			delete  g;
		}
	}

}  // end namespace





Ns_StackSearch::Ns_StackSearch (void)
	: nSearchTreeNodes(0), recordObjective(false)
{    }



	void
Ns_StackSearch::clear (void)
{
	while ( ! empty() )   {

		destroy_goal( top().goalNextChoice );

		pop();
	}
}


Ns_StackSearch::~Ns_StackSearch (void)
{
	clear();


	if ( fileSearchGraph )    {

		fileSearchGraph << "}\n";

		fileSearchGraph.close();
	}
}


Ns_GoalStack::~Ns_GoalStack (void)
{
	while ( ! empty() )   {

		destroy_goal( top() );

		pop();
	}
}

///  Imposes arc consistency.

bool Naxos::imposeArcConsistency() {

	if ( foundInconsistency )   {

		foundInconsistency  =  false;
		getQueue().clear();
		++nFailures;
		return  false;
	}

	Ns_Constraint  *c;
	NsIntVar  *vFired;

	while  ( ! getQueue().empty() )   {

		vFired  =  getQueue().front().getVarFired();


		//  To avoid changing the queue item `Q.front()'
		//   during this iteration...
		vFired->queueItem  =  0;

		while ( (c = getQueue().front().getNextConstraint())  !=  0 )    {

			//  Change the following for AC-3.
			// c->ArcCons();
			c->LocalArcCons( getQueue().front() );

			c->lastConstraintCheckTime  =  ++nConstraintChecks;

			if ( foundInconsistency )   {

				foundInconsistency  =  false;
				getQueue().clear();
				++nFailures;
				return  false;
			}
		}

		getQueue().pop();

	}

	return  true;
}


/* 
 * Backtracks the search to the previous choice point.
 * Each OR-goal has two sub-goals that either of them can be satisfied.
 * If either of them has been completely searched, we can pick the other one
 * to expand.
 *
 * If it is not possible to pick another goal (either because we have
 * exhausted the search space or the limit of backtracks has been reached)
 * the function returns false - otherwise, it returns true
 */

bool Naxos::backtrack() {

	while(1) {

		NsGoal  *goalNextChoice = NULL;


		/* Has the user-set limit for the maximum number of backtracks been exceeded?
		   Return false and terminate the search */
		if ( backtrackLim != 0  &&  nBacktracks >= backtrackLim )
			return  false;
		
		++nBacktracks;

		FEATHER_ASSERT(!searchNodes.empty());
		goalNextChoice = searchNodes.top().goalNextChoice;

		/* Not possible to backtrack */
		if ( goalNextChoice  ==  0 && searchNodes.size() == 1)
			return  false;

		/* Remove the top search node */
		searchNodes.top().domainStore.restore();

		while(searchNodes.top().numDecisions != 0) {
			decisions.pop_back();
			searchNodes.top().numDecisions -= 1;
		}

		searchNodes.pop();

		if( goalNextChoice != NULL) {
			searchNodes.top().stackAND.push( goalNextChoice );
			searchNodes.top().numDecisions += 1;
			decisions.push_back(true);
		}
		else
			continue;

		if(vMinObj  !=  0) {

			vMinObj->remove(bestMinObjValue, kPlusInf);

			if ( foundInconsistency )   {

				foundInconsistency  =  false;
				getQueue().clear();
				continue;
			}
		}

		return  true;
	}
}


/* Reverts the domains of the constrained variables (except for the `objective' variable) 
   in the state that they were after the first arcConsistent() has been called */

void Naxos::restart (void) {
	firstNextSolution  =  true;

	// For any case, we clear the propagation engine's members.
	foundInconsistency  =  false;
	getQueue().clear();


	bool  foundSecondFrame  =  false;

	NsGoal  *goalNextChoice;

	FEATHER_ASSERT(!searchNodes.empty());

	do {
		goalNextChoice  =  searchNodes.top().goalNextChoice;
		if ( goalNextChoice  ==  0 )
			foundSecondFrame  =  true;

		//Ns_StackDomains_t&  domainsOriginal  =  searchNodes.top().domainsStore.domainsOriginal;
		//while ( ! domainsOriginal.empty() )   {
		//	domainsOriginal.top().Var->setDomain( domainsOriginal.top().Dom );
		//	domainsOriginal.pop();
		//}

		searchNodes.top().domainStore.restore();

		searchNodes.pop();
		searchNodes.top().stackAND.push( goalNextChoice );
		// We keeped the above line because of Memory Management
		//  reasons (in order to delete the `goalNextChoice').

		FEATHER_ASSERT(!searchNodes.empty());

	} while ( ! foundSecondFrame );

	searchNodes.pop();
	searchNodes.reset();
	searchNodes.push( Ns_SearchNode( 0, searchNodes.gbegin() ) );

	if(vMinObj != 0)
		vMinObj->remove( bestMinObjValue, kPlusInf);
}

void Naxos::start() {
	firstNextSolution = false;

	/* If we have soft constraints, finalize them and store them as an objective to minimize */
	// if (vMinObj == 0 && !vSoftConstraintsTerms.empty())
	// 	minimize( - NsSum( vSoftConstraintsTerms ));

	//===========================================================================
	//  The following part can be commented out without effects...
	//  (when not using restart, at least)
	//===========================================================================

	//  Throwing away unnesessary `domainStore' in the first frame.
	searchNodes.top().domainStore.clear();

	//  (A) Cutting from the stackAND of the base frame...
	Ns_GoalStack  tempStackAND;

	while ( ! searchNodes.top().stackAND.empty() )   {

		tempStackAND.push( searchNodes.top().stackAND.top() );

		searchNodes.top().stackAND.pop();
	}

	// A push of frame, for the purposes of NsProblemManager::restart().
	//  We took care placing it *after* the arcConsistent() call (because
	//  in future, we will not be able to revert to the current `Q').
	// searchNodes.push( Ns_SearchNode( 0, searchNodes.gbegin() ) );

	//  (B) ...and pasting to the stackAND of the new frame.
	while ( ! tempStackAND.empty() )   {

		searchNodes.top().stackAND.push( tempStackAND.top() );

		tempStackAND.pop();
	}

}

/* Prunes the search tree using these decisions */
pruneResult_t Naxos::prune(std::vector<bool> *decisions) {

	if( firstNextSolution ) {
		/* Search has not started yet - initialize it */
		start();

		/* Is the initial state inconsistent? Don't even bother - the search has already failed */
		if( !imposeArcConsistency() )
			return NO_SOLUTIONS;

		/* .. no goals to execute? Don't even bother - just return a single solution, the current state of the variables */
		if ( searchNodes.top().stackAND.empty() && searchNodes.top().delayedGoal == searchNodes.gend() )
			return ONLY_ONE_SOLUTION;
	}

	if(decisions == NULL)
		return OK;

	/* Start the search using only the specified decisions */
	NsGoal  *CurrGoal, *NewGoal;
	bool  popped_a_goal;
	std::vector<bool>::iterator it = decisions->begin();

	while ( it != decisions->end() )
	{

		popped_a_goal  =  false;

		if ( ! searchNodes.top().stackAND.empty() )   {

			CurrGoal  =  searchNodes.top().stackAND.top();
			searchNodes.top().stackAND.pop();
			popped_a_goal  =  true;

		}  else  {
			FEATHER_THROW("prune: no goal to execute");
			// assert_Ns( searchNodes.top().delayedGoal  !=  searchNodes.gend() ,
			// 		"NsProblemManager::nextSolution: No goal to execute");
			// CurrGoal  =  *searchNodes.top().delayedGoal;
			// ++searchNodes.top().delayedGoal;
		}

		FEATHER_ASSERT(CurrGoal != 0);


		if ( CurrGoal->isGoalAND() )   {

			searchNodes.top().stackAND.push( CurrGoal->getSecondSubGoal() );
			searchNodes.top().stackAND.push( CurrGoal->getFirstSubGoal() );
			if ( popped_a_goal )
				delete  CurrGoal;
			//cout << "--- AND ---\n";


		}  else if ( CurrGoal->isGoalOR())   {

			/* Choose just one sub-goal of OR and discard the other */
			if( *it == false )
				searchNodes.top().stackAND.push( CurrGoal->getFirstSubGoal() );
			else
				searchNodes.top().stackAND.push( CurrGoal->getSecondSubGoal() );

			it++;

			if ( popped_a_goal )
				delete  CurrGoal;

		}  else  {

			++nGoals;

			NewGoal  =  CurrGoal->GOAL();

			if ( popped_a_goal )
				delete  CurrGoal;

			if ( ! imposeArcConsistency() )   {

				FEATHER_THROW("Error: An invalid decision vector was given to NsProblemManager::prune that results in an inconsistent state");

				// destroy_goal( NewGoal );

				// if ( ! backtrack() )
				// 	return OK;

			} else if ( NewGoal  !=  0 )   {

				searchNodes.top().stackAND.push( NewGoal );

			} else if ( searchNodes.top().stackAND.empty()
					&&  searchNodes.top().delayedGoal == searchNodes.gend() )
			{
				if ( vMinObj  !=  0 )   {
					FEATHER_ASSERT(bestMinObjValue > vMinObj->max());
					
					bestMinObjValue  =  vMinObj->max();
					//  We have taken care about the rare (and odd) case
					//   where the domain of vMinObj has been augmented.
				}

				searchNodes.solutionNode(vMinObj);

				/* I've found a solution before exhausting the decision vector... */
				FEATHER_THROW("Error: Found a solution before using all decisions");
			}
		}
	}

	return OK;
}

/* Give up a part of your search space to another, new thread */
void Naxos::giveupWork() {
	LOG("Giving up a part of my search space to a new thread");

	/* Build a vector out of the stack since it doesn't allow to access it backwards >.< */
	std::vector<Ns_SearchNode*> nodes;

	NsStack<Ns_SearchNode>::iterator itstack = searchNodes.begin();
	while(itstack != searchNodes.end() ) {
		nodes.push_back( & (*itstack)  );
		++itstack;
	}


	Int decisionCount = initialDecisions.size();
	std::vector<Ns_SearchNode*>::reverse_iterator it = nodes.rbegin();

	while(it != nodes.rend() ) {

		/*
		 * We found an unused goalNextChoice! Produce a vector
		 * that represents the decisions to reach that goal,
		 * send it off to another thread and remove it from
		 * my search space
		 */
		
		if( (*it)->goalNextChoice != NULL ) {

			if(vMinObj != NULL) {

				/*
			 	 * Wait a second... What if we now know a better global best min objective value?
			 	 * If this node can only produce worse solutions that already known, it would be
			 	 * ridiculous to make all that effort to produce a new thread, just for it to die
			 	 * in a couple of iterations 
				 */

				 if( (*it)->bestObjValuePossible > parent->getMinObjValue() ) {
				 	delete (*it)->goalNextChoice;
				 	(*it)->goalNextChoice = NULL;
				 	LOG("ELIMINATED would-be donated goal");
				 	continue;
				 }

			}

			std::vector<bool> decisionsForNewThread = std::vector<bool>(decisions.begin(), decisions.begin()+decisionCount );
			decisionsForNewThread.push_back(true);

			delete (*it)->goalNextChoice;
			(*it)->goalNextChoice = NULL;
			parent->newInstance(decisionsForNewThread);
			donatedGoals++;
			return;
		}

		decisionCount += (*it)->numDecisions;

		++it;
	}

	LOG("Warning: NsProblemManager::giveupWork() was not able to find any OR-goal to delegate");
}

/* Finds next solution of the problem.  Returns false when no solution is found */
bool Naxos::nextSolution() {
	if ( firstNextSolution )   {
		/*  
		 * Search has not started yet - prune the search tree
		 * according to how Solver is dictating
		 */

		 //TODO: get decision vector from Solver
		 pruneResult_t result = prune(&initialDecisions);

		 if( result == NO_SOLUTIONS )
		 	return false;
		 if ( result == ONLY_ONE_SOLUTION )
		 	return true;
		 if ( result != OK )
		 	FEATHER_THROW("unrecognized prune result");
	}

	if ( calledTimeLimit && timeLim  !=  0 )   {

		calledTimeLimit  =  false;

		if ( isRealTime )   {

			startRealTime  =  time(0);
			FEATHER_ASSERT(startRealTime != -1);

		}  else  {
			startTime  =  clock();
			FEATHER_ASSERT(startTime != -1);
		}
	}


	if ( ( ! imposeArcConsistency() )
			|| (searchNodes.top().stackAND.empty()
				&&  searchNodes.top().delayedGoal == searchNodes.gend()) )
	{

		if ( ! backtrack() ) {
			return  false;
		}
	}

	NsGoal  *CurrGoal, *NewGoal;
	bool  popped_a_goal;

	while ( timeLim == 0   ||  (   isRealTime  &&  difftime(time(0),startRealTime)  <= timeLim)
	                       ||  ( ! isRealTime  &&  static_cast<unsigned long>(clock()-startTime) <= timeLim*static_cast<unsigned long>(CLOCKS_PER_SEC) ) )
	{

		iterations++;

		/* Should I give up some of my work to another thread? */
		if( parent != NULL && iterations > 20000 && iterations%256 == 0) {

			if( searchNodes.size() - donatedGoals >= 8 && parent->needMoreWork() )
				giveupWork();

		}

		/* Is there a better global min value? */
		if( parent != NULL && iterations%8 == 0 ) {

			Int globalBest = parent->getMinObjValue();

			if(globalBest < bestMinObjValue) {
				bestMinObjValue = globalBest;
				
				/*				
				 * Remove all values worse than the global minimum and
				 * check if state is still consistent
				 */ 
				
				vMinObj->remove(bestMinObjValue, kPlusInf);

				if( foundInconsistency )
					if( !backtrack() ) {
						return false;
					}
			}

		}

		popped_a_goal  =  false;

		if ( ! searchNodes.top().stackAND.empty() )   {

			CurrGoal  =  searchNodes.top().stackAND.top();
			searchNodes.top().stackAND.pop();
			popped_a_goal  =  true;

		}  else  {
			FEATHER_ASSERT(searchNodes.top().delayedGoal  !=  searchNodes.gend() );
			CurrGoal  =  *searchNodes.top().delayedGoal;
			++searchNodes.top().delayedGoal;
		}

		FEATHER_ASSERT(CurrGoal != 0);

		if ( CurrGoal->isGoalAND() )   {

			searchNodes.top().stackAND.push( CurrGoal->getSecondSubGoal() );
			searchNodes.top().stackAND.push( CurrGoal->getFirstSubGoal() );
			if ( popped_a_goal )
				delete  CurrGoal;
			//cout << "--- AND ---\n";


		}  else if ( CurrGoal->isGoalOR())   {

			if( CurrGoal->isMandatoryParallelOR() ) {

				if( parent == NULL ) {
					FEATHER_THROW("Error: Encountered mandatory parallel or but no solver has been defined");
				}

				/* 
				 * I need to fork the search tree - send the decisions so far
				 * to the parent so another ProblemManager takes over the right
				 * branch of this OR
				 */

				 decisions.push_back(true);
				 parent->newInstance(decisions);
				 decisions.pop_back();

				searchNodes.top().stackAND.push( CurrGoal->getFirstSubGoal() );
				decisions.push_back(false);
				searchNodes.top().numDecisions += 1;
			}
			else {

				searchNodes.push( Ns_SearchNode( CurrGoal->getSecondSubGoal(), searchNodes.gbegin() ) );
				
				if(vMinObj != NULL)
					searchNodes.top().bestObjValuePossible = vMinObj->min();
				
				searchNodes.top().stackAND.push( CurrGoal->getFirstSubGoal() );
				// searchNodes.top().searchSpaceEstimate = estimateSearchSpace();

				decisions.push_back(false);
				searchNodes.top().numDecisions += 1;

				if ( popped_a_goal )
					delete  CurrGoal;

			}


		}  else  {

			++nGoals;

			NewGoal  =  CurrGoal->GOAL();

			if ( popped_a_goal )
				delete  CurrGoal;

			//for (unsigned i=0;  i<VarGoal->size();  ++i)
			//	cout << " " << (*VarGoal)[i];
			//cout << "\n";

			if ( ! imposeArcConsistency() )   {

				//cout << "<BACKTRACK>\n";
				destroy_goal( NewGoal );

				if ( ! backtrack() ) {
					return false;

				}

			} else if ( NewGoal  !=  0 )   {

				searchNodes.top().stackAND.push( NewGoal );

			} else if ( searchNodes.top().stackAND.empty()
					&&  searchNodes.top().delayedGoal == searchNodes.gend() )
			{
				if ( vMinObj  !=  0 )   {
					FEATHER_ASSERT(bestMinObjValue > vMinObj->max());
					
					bestMinObjValue  =  vMinObj->max();
					//  We have taken care about the rare (and odd) case
					//   where the domain of vMinObj has been augmented.

					/* Update global minimum? */
					if( parent != NULL) {
						Int globalBest = parent->getMinObjValue();

						if( bestMinObjValue < globalBest ) {
							parent->updateMinObjValue(bestMinObjValue);
						}

					}

				}

				searchNodes.solutionNode(vMinObj);
				// std::cout << "Found solution after " << iterations << " iterations" << std::endl;
				// cout << "Found solution, search space estimation is " << estimateSearchSpace() << endl;
				// if( estimateSearchSpace() != 1)
				// 	throw NsException("search space not 1");

				return  true;

			}
		}
	}
	return  false;
}

IntDomain* Naxos::getDomain(IntVarID id) {
	IntDomain const* domain = vars[id]->getDomain();
	IntDomain *newdomain = new IntRanges(domain->min(), domain->max() );
	newdomain->removeAllBut(domain);
	return newdomain;
}


void Naxos::supplyRepresentation(const Representation& repr) {
	/* Add all variables */
	{
		std::vector<RepresentationIntVar>::const_iterator it;
		for(it = repr.vars.begin(); it != repr.vars.end(); it++)
			addVar(*it);
	}
	/* Add all arrays */
	{
		std::vector<RepresentationIntVarArray*>::const_iterator it;
		for(it = repr.arrays.begin(); it != repr.arrays.end(); it++)
			addArray( *(*it) );
	}
	/* Add all constraints */
	{
		std::vector<const Constraint*>::const_iterator it;
		for(it = repr.constraints.begin(); it != repr.constraints.end(); it++) {
			Ns_Constraint *constr = addConstraint( *(*it) );
			constraints.push_back(constr);
		}
	}
	/* Add all goals */
	{
		std::vector<const Goal*>::const_iterator it;
		for(it = repr.goals.begin(); it != repr.goals.end(); it++) {
			NsGoal *goal = convertGoal( *(*it) );
			searchNodes.top().stackAND.push( goal );
		}
	}
	/* Set minObj, if one has been supplied */
	if(repr.minObj != -1) {
		minimize( *vars[repr.minObj] );
	}
}

Naxos::Naxos()
			:  foundInconsistency(false),
			   vMinObj(0),
			   timeLim(0),
			   firstNextSolution(true),
			   calledTimeLimit(false),
			   nFailures(0),
			   nBacktracks(0),
			   nGoals(0),
			   nConstraintChecks(0),
			   backtrackLim(0),
			   parent(NULL),
			   iterations(0),
			   donatedGoals(0) {

	searchNodes.push( Ns_SearchNode( 0, searchNodes.gbegin() ) );
}

namespace {
	void addTwoVars(NsIntVar *varx, NsIntVar *vary, Ns_Constraint *constr) {
		varx->addConstraint(constr);
		vary->addConstraint(constr);
		constr->ArcCons();
	}

	void addThreeVars(NsIntVar *varx, NsIntVar *vary, NsIntVar *varz, Ns_Constraint *constr) {
		varx->addConstraint(constr);
		vary->addConstraint(constr);
		varz->addConstraint(constr);
		constr->ArcCons();
	}

	void addArr(NsIntVarArray &arr, UInt start, UInt length, Ns_Constraint *constr) {
		NsIndex i = 0;
		for(i = start; i < start+length; i++)
			arr[i].addConstraint(constr);
		constr->ArcCons();
	}


}

Ns_Constraint* Naxos::addConstraint(const Constraint& con) {
	switch(con.fType) {
		#include "constraint-glue.icc"

		case Constraints::AllDiff: {
			const Constr_AllDiff &scon = static_cast<const Constr_AllDiff&>(con);
			Ns_ConstrAllDiff *nscon = new Ns_ConstrAllDiff(vararrays[scon.fArr]);
			addArr(*vararrays[scon.fArr], 0, (*vararrays[scon.fArr]).size(), nscon);
			return nscon;
		}

		default:
			FEATHER_THROW("Unrecognized constraint");
	}
}

NsGoal* Naxos::convertGoal(const Goal& goal) {
	switch(goal.fType) {
		case Goals::InDomain: {
			const Goal_InDomain &sgoal = static_cast<const Goal_InDomain&>(goal);
			return new NsgInDomain( *vars[sgoal.fVar] );
		}
		case Goals::Labeling: {
			const Goal_Labeling &sgoal = static_cast<const Goal_Labeling&>(goal);
			return new NsgLabeling( *vararrays[sgoal.fArr] );
		}
		default:
			FEATHER_THROW("Unrecognized goal with type " << goal.fType);
	}
}

void Naxos::addVar(const RepresentationIntVar& var) {
	FEATHER_ASSERT(vars.find(var.id) == vars.end());

	IntDomain *domain = var.domain;
	Bitset *newdomain = new Bitset(domain->min(), domain->max() );
	newdomain->removeAllBut(domain);

	NsIntVar *newvar = new NsIntVar(newdomain, *this);
	vars[var.id] = newvar;

	FEATHER_ASSERT( newdomain->toString() == domain->toString() );
}

void Naxos::addArray(const RepresentationIntVarArray& array) {
	FEATHER_ASSERT(vararrays.find(array.id) == vararrays.end());

	NsIntVarArray *nsarray = new NsIntVarArray();
	std::deque<IntVarID>::const_iterator it;
	for(it = array.vars.begin(); it != array.vars.end(); it++) {
		FEATHER_ASSERT( vars[*it] != NULL );
		nsarray->push_back( *vars[*it] );
	}

	vararrays[array.id] = nsarray;
}

void Naxos::clearRepresentation() {
	/* Clear queues */
	while(!searchNodes.empty())
		searchNodes.pop();
	/* Clear vars */
	{
		std::map<IntVarID,NsIntVar*>::iterator it;
		for(it = vars.begin(); it != vars.end(); it++)
			delete it->second;

		vars.clear();
	}
	/* Clear arrays */
	{
		std::map<IntVarArrayID,NsIntVarArray*>::iterator it;
		for(it = vararrays.begin(); it != vararrays.end(); it++)
			delete it->second;

		vararrays.clear();
	}
	/* Clear constraints */
	{
		std::vector<Ns_Constraint*>::iterator it;
		for(it = constraints.begin(); it != constraints.end(); it++)
			delete *it;

		constraints.clear();
	}
}

Naxos::~Naxos() {
	clearRepresentation();
}

}
