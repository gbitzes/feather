#include "NsIntVar.h"
#include <backend/naxos/naxos.h>
#include <backend/naxos/Ns_Constraint.h>
#include <base/bitset.h>

namespace feather {

///  A bound (min or max) of the domain has been changed by the constraint constr.

	void
Ns_QueueItem::boundChangedBy (const Ns_Constraint *constr)
{
	removedBoundRec.boundChangedBy(constr ,
		varFired->manager().numConstraintChecks() );
}




///  Adds the tuple (removedValue,constraintThatRemovedIt) to the removedValues collection.

void Ns_QueueItem::add(const Int removedVal, const Ns_Constraint *constrThatRemovedIt) {
	removedValues.push_back(
		RemovedValueRecord_t( removedVal, constrThatRemovedIt) );
}

void NsIntVar::addConstraint (Ns_Constraint* constr) {
	constraints.push_back( ConstraintAndFailure(constr) );

	FEATHER_ASSERT( constr->varsInvolvedIn() >= 1);
	arcsConnectedTo  +=  constr->varsInvolvedIn() - 1;

	if ( constr->revisionType  ==  Ns_Constraint::VALUE_CONSISTENCY )
		constraintNeedsRemovedValues  =  true;
}


bool NsIntVar::removeRange(const Int first, const Int last, const Ns_Constraint *constr, bool& modified) {
	
	/* Is the domain going to be cleared? Return false and don't modify anything */
	if ( first <= min()  &&  max() <= last ) {
		naxos.foundAnInconsistency();
		return false;
	}


	bool rangeEmpty = true;


	/* Are the bounds of the domain about to change? Record it */
	if ( ( first <= min() && min() <= last ) || ( first <= max() && max() <= last ) )
	{
		rangeEmpty  =  false;

		if ( queueItem  ==  0 )    {
			naxos.getQueue().push( Ns_QueueItem(this) );
			queueItem  =  & naxos.getQueue().back();
		}

		queueItem->boundChangedBy(constr);
	}


	//  Check for modifications of the intermediate values of the
	//   domain, if necessary.  (E.g. when they must be stored for arc
	//   consistency constraints checks.)

	if ( rangeEmpty  || storeRemovedValues() )     {

		Int newFirst = ( first  !=  kMinusInf ) ?
					  first - 1
					: first;

		Int newLast  = ( last  !=  kPlusInf ) ?
					  last + 1
					: last;

		newFirst = domain->next(newFirst);

		/* Are there any values to be stored? */
		if(newFirst < newLast) {
			rangeEmpty = false;

			if ( queueItem  ==  0 )    {
				naxos.getQueue().push( Ns_QueueItem(this) );
				queueItem  =  & naxos.getQueue().back();
			}
		}

		/* Store all values that are about to be removed */
		while ( newFirst <  newLast )    {
			queueItem->add(newFirst, constr);
			newFirst = domain->next(newFirst);
		}
	}

	if (!rangeEmpty && !isTransparent)    {
		
		IntDomain::RemovalResult result;
		bool saveonchange = false;

		/* Not current save - should save on modification */
		if(!naxos.isCurrentHistoryId(lastSaveHistoryId()))
			saveonchange = true;

		result = domain->removeRange(first, last, saveonchange);
		
		modified = result == IntDomain::CHANGE;
		
		if(modified) {
			naxos.domainModified(domain);
			lastSaveId = naxos.getCurrentHistoryId();
		}

		// modified = true;

		// if ( ! bitset.removeRange(first,last) )    {

		// 	pm->foundAnInconsistency();
		// 	return  false;

		// }  else  {

		// 		modified  =  true;
		// }
	}

	return  true;
}

void NsIntVar::removeAll() {
	naxos.foundAnInconsistency();
}

NsIntVar::NsIntVar(IntDomain *domain, Naxos &naxos_) : naxos(naxos_) {
	this->domain = domain;
	arcsConnectedTo = 0;
	constraintNeedsRemovedValues = false;
	queueItem = 0;
	isTransparent = false;

	/* Making lastSaveId dirty as the domain has not been saved */
	lastSaveId.id     =  kUPlusInf;
	lastSaveId.level  =  0;
}

NsIntVar::NsIntVar(Naxos& naxos_, Int min, Int max) : naxos(naxos_) {
	this->domain = new Bitset(min, max);
	arcsConnectedTo = 0;
	constraintNeedsRemovedValues = false;
	queueItem = 0;
	isTransparent = false;

	/* Making lastSaveId dirty as the domain has not been saved */
	lastSaveId.id     =  kUPlusInf;
	lastSaveId.level  =  0;	
}

NsIntVar::~NsIntVar() {
	delete domain;
}

} // namespace feather



