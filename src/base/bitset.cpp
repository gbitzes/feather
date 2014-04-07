#include "bitset.h"
#include "utils.h"

namespace feather {

Bitset::Bitset(Int minval, Int maxval) {

	if(minval > maxval)
		FEATHER_THROW("Invalid range " << minval << "-" << maxval );

	this->minval = minval;
	this->maxval = maxval;
	this->setcount = maxval - minval + 1;

	offset = minval;
}

void Bitset::save() {
	clones.push_back( BitsetState(minval, maxval, setcount) );

	/* 
	 * We are being lazy and don't copy the
	 * actual biset until absolutely necessary.
	 *
	 * It will be copied during the next call to removeRange()
	 * that changes the bitset
	 */

}

void Bitset::restore() {
	BitsetState& state = clones.back();

	/* Restore minVal and maxVal from the previous revision */
	minval = state.minval;
	maxval = state.maxval;
	setcount = state.setcount;

	/* Restore machw array */
	if(state.machw != NULL) {
		delete machw;
		machw = state.machw;
	}

	clones.pop_back();

	/* 
	 * If the bitset has been restored to its initial
	 * state, we add one more save() to guarantee that
	 * subsequent calls to restore() won't segfault
	 *
	 * Subsequent restore()'s will be no-ops.
	 */

	if( clones.size() == 0 )
		save();
}

enum IntDomain::RemovalResult 
Bitset::removeRange(Int rangeMin, Int rangeMax, bool saveOnChange, bool dontClear) {


}

}
