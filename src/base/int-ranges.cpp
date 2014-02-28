#include "int-ranges.h"
#include "utils.h"

using namespace std;

namespace feather {

IntRanges::IntRanges(const Int minval, const Int maxval) {
	
	if(minval > maxval)
		FEATHER_THROW("Invalid range " << minval << "-" << maxval );

	this->minval = minval;
	this->maxval = maxval;
	this->setcount = maxval - minval + 1;

	/*
	 * Initially the structure contains only one
	 * range: minval to maxval
	 */

	intervals[minval] = maxval;

	/* Stores the initial save() of the bitset */
	// IntRanges::save();
}

void IntRanges::save() {
	/* Put a special mark in history that signifies a save */
	history.push( Range(1, -1) );
	setcountHistory.push( setcount );
}

void IntRanges::restore() {
	Range r = history.top();

	while( !( r.start == 1 && r.end == -1  ) ) {

		/* Erase any ranges between r.start and r.end */
		map<Int,Int>::iterator it = intervals.upper_bound(r.start);
		if(it != intervals.begin() )
			it--;

		if( r.start >= it->second )
			it++;

		map<Int,Int>::iterator it2 = intervals.upper_bound(r.end);
		// if(it2 != intervals.end() )
			// it2++;

		intervals.erase(it, it2);
		intervals[r.start] = r.end;

		history.pop();
		r = history.top();

	}

	setcount = setcountHistory.top();
	setcountHistory.pop();

	history.pop();

	/* Restore minval, maxval */
	minval = intervals.begin()->first;
	maxval = (--intervals.end())->second;

	if( history.empty() )
		save();
}

enum IntDomain::RemovalResult 
IntRanges::removeRange(Int rangeMin, Int rangeMax, bool saveOnChange, bool dontClear) {

	// cout << *this << endl;
	// cout << "removeRange called with " << rangeMin << ".." << rangeMax << endl;

	/* Invalid range? */
	if( rangeMax < rangeMin )
		FEATHER_THROW("Invalid range: " << rangeMin << "-" << rangeMax);

	/* Is the domain about to be cleared? */
	// TODO: FIX
	if ( rangeMin <= minval  &&  maxval <= rangeMax ) {
		return cleared;
	}

	/* Will the domain remain intact? Just return, we have nothing more to do.. */
	if ( rangeMax < minval  ||  maxval < rangeMin ) {
		return nochange;
	}

	/*
	 * The domain will be altered - need to save?
	 */

	 if(saveOnChange) {
	 	save();
	 }

	if ( rangeMin  <=  minval )
		rangeMin  =  minval;

	if ( maxval  <=  rangeMax )
		rangeMax  =  maxval;

	map<Int,Int>::iterator itFirst = intervals.upper_bound(rangeMin);
	if( itFirst != intervals.begin() )
		itFirst--;

	map<Int,Int>::iterator itLast = intervals.upper_bound(rangeMax);
	itLast--;

	bool oneBlock = (itFirst == itLast);

	map<Int,Int>::iterator temp = itFirst;
	bool twoBlocks = (++temp == itLast);

	// cout << "oneBlock: " << oneBlock << endl;

	// cout << "rangeMin = " << rangeMin << " - " << "rangeMax = " << rangeMax << endl;
	// cout << "itFirst is " << itFirst->first << ".." << itFirst->second << endl;
	// cout << "itLast is " << itLast->first << ".." << itLast->second << endl;



	/* Split the first block into left and right */
	Range firstLeft( itFirst->first, std::min(rangeMin-1, itFirst->second) );
	Range firstRight( rangeMax+1, itFirst->second );

	// cout << "firstLeft is " << firstLeft.start << ".." << firstLeft.end << endl;
	// cout << "firstRight is " << firstRight.start << ".." << firstRight.end << endl;


	/* If the left block is not equal to the original, it means the first block has changed */
	if( !( firstLeft.start == itFirst->first && firstLeft.end == itFirst->second  )  ) {

		/* Store the original first block in history */
		history.push( Range(itFirst->first, itFirst->second) );

		/* Is firstLeft a valid range? If not, it means the left part is empty and the original should be removed */
		if( !firstLeft.isValid() ) {
			// cout << "left part is empty" << endl;
			// cout << "itFirst is " << itFirst->first << ".." << itFirst->second << endl;
			setcount -= (itFirst->second - itFirst->first)+1;
			// cout << "condition: " << *this << endl;
			intervals.erase(itFirst++);
			// cout << "condition: " << *this << endl;
		}
		/* Just change the original interval */
		else {
			setcount -= (itFirst->second - firstLeft.end);
			itFirst->second = firstLeft.end;
			++itFirst;
		}

		// cout << "itFirst is " << itFirst->first << ".." << itFirst->second << endl;


		/* Is firstRight a valid range? If yes, it meens the original block was split into two and firstRight
		   needs to be inserted */

		if( firstRight.isValid() ) {
			setcount += (firstRight.end - firstRight.start)+1;
			intervals[firstRight.start] = firstRight.end;
		}

	}
	/* The first block has not changed */
	else {
		itFirst++;
	}



	/* If firstRight is not valid AND oneBlock is not true, it means the range spans over multiple intervals 
	   and I need to truncate the last block, too */
	if( !firstRight.isValid() && !oneBlock ) {

		// cout << "firstRight is not valid!" << endl;

		Range lastNew( std::max(itLast->first, rangeMax+1) , itLast->second );

		// cout << "lastNew: " << lastNew.start << ".." << lastNew.end << endl;

		/* If lastNew is not equal to the original last, it means the last block has changed */
		if( !( lastNew.start == itLast->first && lastNew.end == itLast->second )  ) {
			
			/* Store the last block in history */
			history.push( Range(itLast->first, itLast->second));

			setcount -= (itLast->second - itLast->first)+1;
			intervals.erase(itLast--);
			
			/* Is the lastNew interval valid? If it's not, that means the whole last block has been wiped out
			   and no need to insert anything new */

			if( lastNew.isValid() ) {
				setcount += (lastNew.end - lastNew.start)+1;
				intervals[lastNew.start] = lastNew.end;
			}

		}
	}


	// cout << "itFirst is " << itFirst->first << ".." << itFirst->second << endl;
	// cout << "itLast is " << itLast->first << ".." << itLast->second << endl;
	// cout << "current condition: " << *this << endl;

	/* Is there anything between itFirst and itLast to delete? */
	if( !twoBlocks && !oneBlock && itLast != intervals.end() && itLast->first >= itFirst->first ) {

		// cout << "there's stuff to delete" << endl;

		/* Yes - delete the blocks between itFirst and itLast except itLast */
		while( itFirst != intervals.end() && itLast->first >= itFirst->first ) {

			/* Store to history */
			history.push( Range(itFirst->first, itFirst->second) );

			setcount -= (itFirst->second - itFirst->first)+1;
			intervals.erase(itFirst++);
		}
	}

	/* Update minval, maxval */
	minval = intervals.begin()->first;
	maxval = (--intervals.end())->second;

	// cout << "result: " << *this << endl;
	return change;
}

Int IntRanges::next (const Int val) const {

	// cout << *this << endl;
	// cout << "next called with " << val << endl;
	// exit(0);

	/* Out of bounds */
	if( maxval <= val )
		return kPlusInf;

	if( val < minval )
		return minval;

	map<Int,Int>::const_iterator it = intervals.upper_bound(val);
	it--;

	// cout << "it is " << it->first << ".." << it->second << endl;

	/* Is this the last value in a block? */
	if( it->second == val ) {
		it++;
		return it->first;
	}

	if( it->first <= val+1 && val+1 <= it->second )
		return val+1;
	else {
		it++;
		return it->first;
	}
}

Int IntRanges::previous (const Int val) const {

	// cout << "previous called with " << val << endl;

	/* Out of bounds */
	if( val <= minval )
		return kMinusInf;

	if( maxval < val ) 
		return maxval;

	map<Int,Int>::const_iterator it = intervals.upper_bound(val);
	it--;

	/* Is this the first value in a block? */
	if( it->first == val ) {
		it--;
		return it->second;
	}

	if( it->first <= val-1 && val-1 <= it->second )
		return val-1;
	else
		return it->second;
}

bool IntRanges::contains(const Int value) const {

	if( value < minval || maxval < value )
		return false;

	map<Int,Int>::const_iterator it = intervals.upper_bound(value);
	
	/* This means value is smaller than min(), so return false */
	if( it == intervals.begin() )
		return false;

	it--;

	if( it->first <= value && value <= it->second )
		return true;
	else
		return false;
}

Int IntRanges::nextGap (const Int val) const {

	/* Out of bounds */
	if( maxval-1 <= val )
		return kPlusInf;

	if( val <= minval-1 )
		return nextGap(minval);

	map<Int,Int>::const_iterator it = intervals.upper_bound(val);
	it--;

	if( it->first <= val && val <= it->second ) {
		return it->second+1;
	}
	else {
		/* Make sure it's not the last gap */
		it++;

		if( it->first == val+1 )
			return it->second+1;
		else
			return val+1;
	}

}
// TODO: replace contains() with this
bool IntRanges::containsRange(const Int rangeMin, const Int rangeMax) const {
	if(rangeMin > rangeMax)
		FEATHER_THROW("Invalid range: " << rangeMin << "," << rangeMax);

	/* Out of bounds */
	if( rangeMin < minval )
		return false;

	map<Int,Int>::const_iterator it = intervals.upper_bound(rangeMin);
	it--;

	if( it->first <= rangeMin && rangeMin <= it->second ) {
		return (rangeMax <= it->second);
	}
	else
		return false;

}









} // namespace feather

