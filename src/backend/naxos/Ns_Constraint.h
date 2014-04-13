#ifndef FEATHER_BACKEND_NSCONSTRAINT_H
#define FEATHER_BACKEND_NSCONSTRAINT_H

#include <backend/naxos/NsIntVar.h>
#include <base/utils.h>

namespace feather {

class Ns_QueueItem;

class Ns_Constraint {

	public:
		/* 
		 * The timestamp (current removal ID) when the last check
		 * of this constraint took place
		 */

		unsigned long lastConstraintCheckTime;

		Ns_Constraint() {
			lastConstraintCheckTime = 0;
			revisionType = BOUNDS_CONSISTENCY;
		}

		virtual void ArcCons() = 0;
		virtual void LocalArcCons(Ns_QueueItem&) = 0;

		/* The number of the variables involved in the constraint */
		virtual Int varsInvolvedIn() const = 0;

		/*
		 * Description of the consistency type that a revision function
		 * for a constraint can impose
		 */

		enum ConsistencyType {
			/*
			 * The revision function needs to know the value that has been
			 * removed from the domain (see NsConstrInverse)
			 */

			VALUE_CONSISTENCY = 1,

			/*
			 * The revision function does not need to know the removed values
			 */

			BOUNDS_CONSISTENCY,

			/*
			 * Like BOUNDS_CONSISTENCY, but revision is imposed in both directions
			 */

			BIDIRECTIONAL_CONSISTENCY
		};

		ConsistencyType revisionType;

		virtual ~Ns_Constraint() {}
};

class Ns_ConstrXlessthanY : public Ns_Constraint {

	private:
		NsIntVar *varX, *varY;

	public:
		Ns_ConstrXlessthanY(NsIntVar *x, NsIntVar *y) : varX(x), varY(y) {
			FEATHER_ASSERT(  &x->manager() == &y->manager() );
		}

		virtual Int varsInvolvedIn() const { return 2; }

		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);

};

class Ns_ConstrXlesseqthanY : public Ns_Constraint {
	private:
		NsIntVar *varX, *varY;

	public:
		Ns_ConstrXlesseqthanY(NsIntVar *x, NsIntVar *y) : varX(x), varY(y) {
			FEATHER_ASSERT( &x->manager() == &y->manager() );
		}

		virtual Int varsInvolvedIn() const { return 2; }

		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};

class Ns_ConstrXeqYplusC : public Ns_Constraint { 
	private:
		NsIntVar *VarX, *VarY;
		Int C;

	public:
		Ns_ConstrXeqYplusC(NsIntVar *x, NsIntVar *y, Int c_) : VarX(x), VarY(y), C(c_) {
			revisionType = BIDIRECTIONAL_CONSISTENCY;
			FEATHER_ASSERT( &x->manager() == &y->manager() );
		}

		virtual Int varsInvolvedIn() const { return 2; }

		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};






} // namespace feather

#endif