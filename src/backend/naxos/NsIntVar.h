#ifndef FEATHER_BACKEND_NSINTVAR_H
#define FEATHER_BACKEND_NSINTVAR_H

#include <feather/int-domain.h>
#include <backend/naxos/basics.h>
#include "NsDataStructures.h"

namespace feather {

class Ns_Constraint;
class Naxos;
class Ns_QueueItem;

class NsIntVar {

	private:
		/* Pointer to the domain holding the values */
		IntDomain *domain;

		/* Reference to the naxos engine */
		Naxos &naxos;

		/* The timestamp that can be used in chronological backtracking */
		Ns_HistoryId_t  lastSaveId;

		bool isTransparent;

	public:

		void transparent() {
			isTransparent = true;
		}

		typedef IntDomain::Iterator const_iterator;

		const const_iterator begin() const {
			return domain->begin();
		}

		const const_iterator end() const {
			return domain->end();
		}

		typedef IntDomain::ReverseIterator const_reverse_iterator;

		const const_reverse_iterator rbegin() const {
			return domain->rbegin();
		}

		const const_reverse_iterator rend() const {
			return domain->rend();
		}

		typedef IntDomain::GapIterator const_gap_iterator;

		const const_gap_iterator gap_begin() const {
			return domain->gap_begin();
		}

		const const_gap_iterator gap_end() const {
			return domain->gap_end();
		}



		NsIntVar(IntDomain *domain, Naxos&);
		NsIntVar(Naxos&, Int min, Int max);
		~NsIntVar();

		/*
		 * These methods allow the removal of a value plus the recording
		 * of the constraint that triggered the removal.
		 *
		 * If given constraint == NULL, no constraint is recorded
		 */

		void removeAll();
		
		bool removeSingle (const Int val, const Ns_Constraint *c) {
			return  removeRange(val, val, c);
		}

		void remove(const Int val) {
			remove(val, val);
		}

		void remove(const Int first, const Int last) {
			removeRange(first, last, NULL);
		}

		bool removeRange(const Int first, const Int last, const Ns_Constraint *c) {
			bool modifiedFoo;
			return removeRange(first, last, c, modifiedFoo);
		}

		bool removeRange(const Int first, const Int last, const Ns_Constraint *c, bool& modified);

		void save() {
			domain->save();
		}

		void restore() {
			domain->restore();
		}

		UInt size() const {
			return domain->size();
		}

		Int previous (const Int toVal)  const {
			return  domain->previous(toVal);
		}

		Int next (const Int toVal)  const {
			return  domain->next(toVal);
		}

		Naxos& manager() const {
			return naxos;
		}

		Int value() const {
			FEATHER_ASSERT( isBound() );
			return domain->min();
		}


		Int min() const {
			return domain->min();
		}

		Int max() const {
			return domain->max();
		}

		bool contains(const Int val) const {
			return contains(val, val);
		}

		bool contains (const Int first, const Int last)  const {
			return domain->containsRange(first, last);
		}



		IntDomain const* getDomain() const {
			return domain;
		}

		void set(Int val) {
			remove(kMinusInf, val-1);
			remove(val+1, kPlusInf);
		}

		bool isBound() const {
			return min() == max();
		}

		std::string toString() const {
			return domain->toString();
		}

		Ns_HistoryId_t& lastSaveHistoryId (void) {
			return  lastSaveId;
		}

		///  Adds a constraint to the collection of constraints of the variable.
		void  addConstraint (Ns_Constraint* c);

	public:
		/* 
		 * Pair of a constraint and the 
		 * inconsistencies that has provoked
		 */
		struct ConstraintAndFailure {

			/* The constraint */
			Ns_Constraint *constr;

			/* Constructor */
			ConstraintAndFailure(Ns_Constraint *constr_init) {
				constr = constr_init;
			}
		};

	private:
		///  True, if the variable is involved in an `Inverse' constraint, or another constraint that needs to know the values that have been removed from the variable (the w 's in the AC-5 Algorithm).
		bool  constraintNeedsRemovedValues;

		///  The number of the variables connected to this instance, via constraints.
		int  arcsConnectedTo;

	public:

		Ns_QueueItem *queueItem;

		///  An array of the constraints that the variable is involved in.
		std::deque<ConstraintAndFailure>  constraints;



		///  Returns true if the variable is involved in an `Inverse' constraint, or another constraint that needs to know the values that have been removed from the variable.
		bool storeRemovedValues (void)  const {
			return  constraintNeedsRemovedValues;
		}







};




} // namespace feather

#endif