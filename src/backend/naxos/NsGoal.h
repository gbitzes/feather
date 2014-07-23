#ifndef FEATHER_BACKEND_NSGOAL_H
#define FEATHER_BACKEND_NSGOAL_H

#include <feather/utils.h>
#include <backend/naxos/NsIntVar.h>
#include <backend/naxos/NsIntVarArray.h>
#include <backend/naxos/NsDataStructures.h>

namespace feather {

/*
 * An abstract class representing a goal
 *
 * Each time a goal is processed by the solver, its
 * method GOAL() is called. This method can make
 * assignments or remove values from constrained
 * variables
 */

class NsGoal {
	public:
		virtual bool isGoalAND() const { 
			return false; 
		}
		
		virtual bool isGoalOR() const {
			return false;
		}

		virtual bool isParallelOR() const {
			return false;
		}

		virtual NsGoal* getFirstSubGoal() const {
			FEATHER_THROW("Not an AND/OR goal");
		}

		virtual NsGoal* getSecondSubGoal() const {
			FEATHER_THROW("Not an AND/OR goal");
		}

		virtual NsGoal* GOAL() = 0;

		/* The destructor of an abstract class should be virtual */
		virtual ~NsGoal() {}
};

class NsgAND : public NsGoal {
	private:
		NsGoal *firstSubGoal;
		NsGoal *secondSubGoal;

	public:
		NsgAND(NsGoal *first, NsGoal *second) {
			FEATHER_ASSERT(first != 0 && second != 0);

			firstSubGoal = first;
			secondSubGoal = second;
		}

		virtual bool isGoalAND() const {
			return true;
		}

		virtual NsGoal* getFirstSubGoal() const {
			return firstSubGoal;
		}

		virtual NsGoal* getSecondSubGoal() const {
			return secondSubGoal;
		}

		NsGoal* GOAL() {
			FEATHER_THROW("This is a meta-goal (i.e. not a real goal");
		}
};

class NsgOR : public NsGoal {
	private:
		NsGoal *firstSubGoal;
		NsGoal *secondSubGoal;
	public:
		NsgOR(NsGoal *first, NsGoal *second) {
			FEATHER_ASSERT(first != 0 && second != 0);

			firstSubGoal = first;
			secondSubGoal = second;
		}

		virtual bool isGoalOR() const {
			return true;
		}

		virtual NsGoal* getFirstSubGoal() const {
			return firstSubGoal;
		}

		virtual NsGoal* getSecondSubGoal() const {
			return secondSubGoal;
		}

		NsGoal* GOAL() {
			FEATHER_THROW("This is a meta-goal (i.e. not a real goal");
		}
};

class NsgParallelOR : public NsGoal  {

	private:

		NsGoal  *firstSubGoal;
		NsGoal  *secondSubGoal;

	public:

		NsgParallelOR (NsGoal *firstSubGoal_init, NsGoal *secondSubGoal_init)
		 : firstSubGoal(firstSubGoal_init), secondSubGoal(secondSubGoal_init)
		{
			assert_Ns( firstSubGoal != 0  &&  secondSubGoal != 0 ,  "NsgOR::NsgOR: A subgoal is zero");
		}


		virtual bool  isGoalOR (void)  const
			{  return  true;  }

		virtual bool isParallelOR() const {
			return true;
		}

		virtual NsGoal*  getFirstSubGoal  (void)  const
			{  return  firstSubGoal;  }

		virtual NsGoal*  getSecondSubGoal (void)  const
			{  return  secondSubGoal;  }


		NsGoal*  GOAL (void) {
			FEATHER_THROW("NsgOR::GOAL: This is a meta-goal (i.e. not a real goal)");
		}
};




/* A simple goal that assigns a value to a constrained variable */
class NsgSetValue : public NsGoal  {
	private:
		NsIntVar& Var;
		const Int value;
	public:
		NsgSetValue(NsIntVar& Var_init, const Int value_init)
			: Var(Var_init), value(value_init) {}

		NsGoal* GOAL() {
			Var.set(value);
			return 0;
		}
};

/* A goal that removes a value from the domain of a given constrained variable */
class NsgRemoveValue : public NsGoal {
	private:
		NsIntVar& Var;
		const Int value;
	public:
		NsgRemoveValue(NsIntVar& Var_init, const Int value_init)
			: Var(Var_init), value(value_init) {}

		NsGoal* GOAL() {
			Var.remove(value);
			return 0;
		}
};

/*
 * A goal that tries to instantiate a constrained variable
 *
 * It assigns a value from the domain of the variable;
 * if this assignment results to an inconsistency, it removes the value
 * from the domain, and continues trying the rest of the values.
 */

class NsgInDomain : public NsGoal  {
	private:
		NsIntVar& Var;
	public:
		NsgInDomain(NsIntVar& Var_init)
			: Var(Var_init) {}

		NsGoal* GOAL() {
			if (Var.isBound())
				return 0;

			Int value = Var.min();

			return  ( new NsgOR( new NsgSetValue(Var,value) ,
			                     new NsgAND( new NsgRemoveValue(Var,value) ,
			                                 new NsgInDomain(*this) ) ) );
		}
};

/*
 * The generalization of NsgInDomain that applies to arrays.
 *
 * It uses NsgInDomain to iteratively instantiate each variable of the 
 * array. The next variable that is chosen is the one having the
 * minimum domain size (according to the `first-fail' heuristic)
 */

class NsgLabeling : public NsGoal {
	private:
		NsIntVarArray& VarArr;
	public:
		NsgLabeling(NsIntVarArray& VarArr_init)
			: VarArr(VarArr_init) {}

		NsGoal* GOAL() {
			bool flag = false;
			NsIndex index = 0;
			UInt minDom = kPlusInf;

			// std::cout << "pointer of vararr" << &VarArr << std::endl;

			for(NsIndex i = 0; i < VarArr.size(); ++i) {
				if(!VarArr[i].isBound() && VarArr[i].size() < minDom) {
					minDom = VarArr[i].size();
					index = i;
					flag = true;
				}
			}

			// std::cout << "ns labeling" << std::endl;
			// std::cout << VarArr[0]->toString() << std::endl;
			
			if(flag == false)
				return NULL;

			return (new NsgAND(new NsgInDomain(VarArr[index]), new NsgLabeling(*this) ));
		}
};

class NsgInDomainParallel : public NsGoal  {

	private:

		NsIntVar&  Var;
		Int limit;

	public:

		NsgInDomainParallel (NsIntVar& Var_init, Int limit_init)
			: Var(Var_init), limit(limit_init)    {    }


		NsGoal*  GOAL (void)
		{
			//std::cout << "  NsgInDomain:    ";
			if (Var.isBound())
				return  0;

			Int value = Var.min();

			/* limit has been reached! Don't parallelize any more */
			if(limit == 0)
				return  ( new NsgOR( new NsgSetValue(Var,value) ,
			                     new NsgAND( new NsgRemoveValue(Var,value) ,
			                                 new NsgInDomain(Var) ) ) );


			return  ( new NsgParallelOR( new NsgSetValue(Var,value) ,
			                     new NsgAND( new NsgRemoveValue(Var,value) ,
			                                 new NsgInDomainParallel(Var, limit-1) ) ) );
		}
};

class NsgLabelingParallel : public NsGoal  {

	private:

		NsIntVarArray&  VarArr;
		Int varlimit, valuelimit;

	public:

		NsgLabelingParallel (NsIntVarArray& VarArr_init, Int varlim, Int valuelim)
			: VarArr(VarArr_init), varlimit(varlim), valuelimit(valuelim) {    }


		NsGoal*  GOAL (void)
		{
			//std::cout << "  NsgLabeling:       ";
			// NsIndex  index = NsINDEX_INF;
			bool flag = false;
		    Int index = 0;
			UInt  minDom = kPlusInf;

			for (NsIndex i = 0;   i < VarArr.size();   ++i)  {
				if ( !VarArr[i].isBound()   &&   VarArr[i].size() < minDom )   {
					minDom = VarArr[i].size();
					index = i;
					flag = true;
				}
			}

			if(flag == false)
				return 0;

			// if (index == NsINDEX_INF)
				// return  0;

			/* limit has been reached! do not parallelize any more */
			if(varlimit <= 0)
				return  ( new NsgAND( new NsgInDomain(VarArr[index]) , new NsgLabeling(VarArr) ) );

			return  ( new NsgAND( new NsgInDomainParallel(VarArr[index], valuelimit) , new NsgLabelingParallel(VarArr, varlimit-1, valuelimit) ) );
		}
};


/* 
 * Normally used for describing the stack 
 * holding AND-goals that have to be satisfied.
 * (This stack is also called `stackAND'.)
 */

class Ns_GoalStack : public NsStack<NsGoal*> {
	public:
		~Ns_GoalStack (void);
};


} // namespace feather

#endif
