#include "bitset.h"
#include <feather/utils.h>
#include <stdio.h>

using namespace std;

namespace feather {

namespace {

	/*
	 * Returns a chunk with all bits set
	 */

	inline Chunk allBitsON() {
		return ~Chunk(0);
	}

	inline Chunk allBitsOFF() {
		return Chunk(0);
	}

	inline Chunk firstBitON() {
		return Chunk(1);
	}

	/*
	 * Returns a chunk with its bits set to 1 from start to end
	 * Example: mwWithSetBits(2, 5) should return something like 011110000...
	 */
	inline Chunk mwWithBits(Int start, Int end) {
		Chunk all = allBitsON();
		Chunk left = all << start-1;
		Chunk right = (all >> ( CHUNK_BITS - end) );
		return left & right;
	}

	/*
	 * Returns the number of bits that are turned on inside a chunk 
	 */

	inline UInt popcount(Chunk w) {
		return __builtin_popcountll(w);
	}

	/*
	 * Return the number of leading zeroes
	 */

	inline UInt bitscan(Chunk w) {
		return __builtin_ffsll(w);
	}

	/*
	 * Return the number of trailing zeroes
	 */

	inline UInt bitscanReverse(Chunk w) {
		return __builtin_clzll(w)+1;
	}

	/* Print a chunk of the bitset in binary - useful for debugging */
	void printChunk(Chunk mw) {
		static char buffer[CHUNK_BITS];		
		char* ptr = buffer;

		ptr += (CHUNK_BITS - 1);

		Chunk original = mw;

		for(int i = CHUNK_BITS - 1; i >= 0; i--) {
			*ptr-- = (mw & 1) + '0';

			mw >>= 1;
		}

		printf("%s\n", buffer);
		// printf("%ul\n", original );
	}


}


Bitset::Bitset(Int minval, Int maxval) {

	if(minval > maxval)
		FEATHER_THROW("Invalid range " << minval << "-" << maxval );

	this->minval = minval;
	this->maxval = maxval;
	this->setcount = maxval - minval + 1;
	this->nbits = setcount;

	offset = minval;

	/*
	 * Initially the bitset is contiguous - no need to
	 * explicitly store the values, so machw is empty
	 */
	
	machw = new std::vector<Chunk>();
	save();
}

Bitset::~Bitset() {
	if(machw != NULL) {
		delete machw;
		machw = NULL;
	}

	for(int i = 0; i < clones.size(); i++)
		if( clones[i].machw != NULL)
			delete clones[i].machw;

	clones.clear();
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
Bitset::removeRange(Int rangeMin, Int rangeMax, bool saveOnChange) {
	/* Invalid range? */
	FEATHER_ASSERT(rangeMin <= rangeMax);

	/* Domain to be cleared? */
	if(rangeMin <= minval  &&  maxval <= rangeMax) {
		return WOULDCLEAR;
	}

	/* Domain remains intact? */
	if(rangeMax < minval  ||  maxval < rangeMin) {
		return NOCHANGE;
	}

	/* Domain will be altered - need to save? */
	if(saveOnChange) {
		save();
	}

	if(rangeMin <= minval)
		rangeMin = minval;

	if(maxval <= rangeMax)
		rangeMax = maxval;

	if(machw->empty()) {

		/*
		 * machw has not been initialized yet - if the range to be
		 * removed falls at the boudns, there's still no need to
		 * initialize machw - just change the bounds
		 */

		 if(rangeMin <= minval) {
		 	setcount -= rangeMax + 1 - minval;
		 	minval = rangeMax + 1;
		 	return CHANGE;
		 }
		 else if(maxval <= rangeMax) {
		 	setcount -= maxval - (rangeMin - 1);
		 	maxval = rangeMin - 1;
		 	return CHANGE;
		 }
		 else {
		 	/*
		 	 * Nope, no luck. It's necessary to initialize the machw array this time
		 	 * Initially, each position just contains chunks with all their bits set
		 	 */

		 	machw->resize((nbits-1)/CHUNK_BITS + 1, allBitsON() );
		 }
	}
	/* machw will change, record it if you haven't already */
	if(clones.back().machw == NULL)
		clones.back().machw = new std::vector<Chunk>(*machw);

	/* Calculate the chunks that correspond to rangeMin and rangeMax */
	UInt minbit = rangeMin - offset;
	UInt minmw = minbit / CHUNK_BITS;

	UInt maxbit = rangeMax - offset;
	UInt maxmw = maxbit / CHUNK_BITS;

	/* Case 1: All values reside in the same chunk */
	if(minmw == maxmw) {
		Int start = minbit % CHUNK_BITS + 1;
		Int end = maxbit % CHUNK_BITS + 1;

		Chunk w = mwWithBits(start, end);
		setcount -= popcount( (*machw)[minmw] & w );
		(*machw)[minmw] = (*machw)[minmw] & ~w;
	}
	/* Case 2: More than one chunks */
	else {
		/* Take care of the first */
		Int start = minbit % CHUNK_BITS + 1;
		Chunk chunk = allBitsON() << start-1;
		setcount -= popcount((*machw)[minmw] & chunk);
		(*machw)[minmw] = (*machw)[minmw] & ~chunk;

		/* Take care of the last */
		Int end = maxbit % CHUNK_BITS + 1;
		chunk = ~(allBitsON() << end);
		setcount -= popcount((*machw)[maxmw] & chunk);
		(*machw)[maxmw] = (*machw)[maxmw] & ~chunk;

		/* Take care of the in-between chunks, if they exist */
		for(int i = minmw+1; i <= maxmw-1; i++) {
			setcount -= popcount((*machw)[i]);
			(*machw)[i] = allBitsOFF();
		}
	}

	if (rangeMin == minval)
		minval = next(minval);
	if (rangeMax == maxval)
		maxval = previous(maxval);

	return CHANGE;
}

bool Bitset::contains(const Int val) const {
	/* Is value out of bounds? If so, it's impossible that it's active */
	if ( maxval < val || val < minval )
		return false;

	/* Has machw been initialized? If not, the value is certainly inside the domain */
	if( machw->empty() )
		return true;

	UInt bit = val - offset;
	UInt mw = bit / CHUNK_BITS;
	Chunk chunk = firstBitON() << bit % CHUNK_BITS;

	return (*machw)[mw] & chunk;
}

Int Bitset::next(const Int val) const {
	if(val >= maxval)
		return kPlusInf;
	if(val < minval)
		return minval;

	if(machw->empty())
		return val+1;

	UInt nbit = val - offset + 1;
	UInt mw = nbit / CHUNK_BITS;
	Chunk chunk = (*machw)[mw] >> (nbit%CHUNK_BITS);

	/* Our next value is in the same chunk */
	if(chunk != allBitsOFF()) {
		/* Calculate the number of leading zeroes */
		UInt zeroes = bitscan(chunk);
		return val + zeroes;
	}
	/* Our next value is not in the same chunk.. need to keep looking */
	else {
		Int size = machw->size();
		for(Int i = mw+1; i < machw->size(); i++) {
			chunk = machw->at(i);

			if(chunk != allBitsOFF()) {
				Int zeroes = bitscan(chunk);
				return val + (CHUNK_BITS - nbit%CHUNK_BITS) + (CHUNK_BITS*(i-mw-1)) + zeroes;
			}
		}
	}

	FEATHER_THROW("Should never reach this point");
}

Int Bitset::previous(const Int val) const {
	// if ( val  <=  minval )
	// 	return  kMinusInf;

	// if ( val  >   maxval )
	// 	return  maxval;

	// if(machw->empty())
	// 	return  (val - 1);	// Bounds Consistarrayency

	// UInt     nbit  =  val - offset - 1;
	// Int        mw  =  nbit / CHUNK_BITS;
	// unsigned long  mwbit  =  1l<<(nbit%CHUNK_BITS);

	// // assert_Ns( nbit < nbits ,
	// // 	"NsBitset::previous: chunk out of `*this' range");

	// if ( (*machw)[mw]  ==  allBitsOFF() )    {
	// 	// speedup by means of comparing the whole chunk
	// 	nbit  =  mw * CHUNK_BITS -1;

	// }  else  {

	// 	for ( ;   mwbit != 0;   mwbit>>=1)    {

	// 		if ( (*machw)[mw] & mwbit )
	// 			return  (nbit + offset);

	// 		--nbit;
	// 	}
	// }

	// --mw;

	// for ( ;   mw >= 0;   --mw)    {

	// 	if ( (*machw)[mw]  ==  allBitsOFF() )    {
	// 		// speedup by means of comparing the whole chunk
	// 		nbit  -=  CHUNK_BITS;

	// 	}  else  {

	// 		for (mwbit=1l<<(CHUNK_BITS-1);  mwbit!=0;  mwbit>>=1)  {

	// 			if ( (*machw)[mw] & mwbit )
	// 				return  (nbit + offset);

	// 			--nbit;
	// 		}
	// 	}
	// }

	// FEATHER_THROW("Should never reach this point");


	if(val <= minval)
		return kMinusInf;
	if(val > maxval)
		return maxval;

	if(machw->empty())
		return val-1;

	UInt nbit = val - offset;
	UInt mw = nbit / CHUNK_BITS;
	Chunk chunk = (*machw)[mw] << CHUNK_BITS - (nbit%CHUNK_BITS);

	/* Our next value is in the same chunk */
	if(chunk != allBitsOFF() ) {
		/* Calculate the number of leading zeroes */
		UInt zeroes = bitscanReverse(chunk);
		//printChunk(chunk);
		return val - zeroes;
	}
	/* Our next value is not in the same chunk.. need to keep looking */
	else {
		Int size = machw->size();
		for(Int i = mw-1; i >= 0; i--) {
			chunk = machw->at(i);

			if(chunk != allBitsOFF() ) {
				Int zeroes = bitscanReverse(chunk);
				return val - (nbit%CHUNK_BITS) - (CHUNK_BITS*(mw-i-1)) - zeroes;
			}
		}
	}
	
	FEATHER_THROW("Should never reach this point");
}

Int Bitset::nextGap(const Int val) const {
	if ( val >= maxval-1 || machw->empty()  )
	{	// || Bounds Consistency
		return  kPlusInf;
	}

	UInt  nbit;
	if ( val  <  minval )
		nbit  =  minval - offset + 1;
	else
		nbit  =  val - offset + 1;

	UInt       mw  =  nbit / CHUNK_BITS;
	Chunk mwbit  =  firstBitON() <<(nbit%CHUNK_BITS);
	UInt   maxbit  =  maxval-offset;

	// assert_Ns( maxbit < nbits  &&  nbit < nbits ,
	// 	"NsBitset::nextGap: Machine chunk out of `*this' range");

	if ( (*machw)[mw]  ==  allBitsON() )    {
		// speedup by means of comparing the whole chunk
		nbit  =  (mw + 1) * CHUNK_BITS;

	}  else  {

		for ( ;   mwbit!= allBitsOFF()  &&  nbit <= maxbit;   mwbit<<=1)    {

			if ( ! ( (*machw)[mw] & mwbit ) )
				return  (nbit + offset);

			++nbit;
		}
	}

	++mw;

	for ( ;   mw < machw->size();   ++mw)    {

		if ( (*machw)[mw]  ==  allBitsON() )    {
			// speedup by means of comparing the whole chunk
			nbit  +=  CHUNK_BITS;

		}  else  {

			for (mwbit= allBitsON() ;  mwbit!= allBitsOFF() && nbit<=maxbit;  mwbit<<=1)
			{
				if ( ! ( (*machw)[mw] & mwbit ) )
					return  (nbit + offset);

				++nbit;
			}
		}
	}

	return  kPlusInf;

}

bool Bitset::containsRange(const Int rangeMin, const Int rangeMax) const {
	if ( rangeMin < minval  ||  rangeMax > maxval )
		return  false;

	if(machw->empty())
		return  true;

	for (Int val=rangeMin;   val <= rangeMax;  ++val)   {

		//std::cout << "val = " << val << "\n";

		UInt  nbit  =  val - offset;
		UInt  mw    =  nbit / CHUNK_BITS;

		if ( (*machw)[mw]  ==  allBitsON() )    {
			// speedup by means of comparing the whole chunk

			if ( CHUNK_BITS - nbit % CHUNK_BITS  >
				static_cast<UInt>( rangeMax - val ) )
			{
				break;
			}

			val  +=  CHUNK_BITS - nbit % CHUNK_BITS - 1;
			// `-1' above is used to compensate
			//  the for's `++val' increment.

		}  else  {

			Chunk mwbit  =  firstBitON() <<(nbit%CHUNK_BITS);

			if ( ! ( (*machw)[mw] & mwbit ) )
				return  false;
		}
	}

	return  true;


}

IntDomainPosition* Bitset::getPosition(const Int value) const {
	BitsetPosition *pos = new BitsetPosition();
	pos->value = value;
	return pos;
};
Int Bitset::advance(IntDomainPosition &pos) const {
	BitsetPosition *spos = static_cast<BitsetPosition*>(&pos);
	spos->value = next(spos->value);
	return spos->value;
};
Int Bitset::moveback(IntDomainPosition &pos) const {
	BitsetPosition *spos = static_cast<BitsetPosition*>(&pos);
	spos->value = previous(spos->value);
	return spos->value;
};


string Bitset::toString() const {
	ostringstream ss;
	ss << "[";

	Int val, gap;
	gap = kMinusInf;

	while((val = next(gap)) != kPlusInf) {
		ss << val;
		if((gap = nextGap(val)) == kPlusInf) {
			if(max() != val)
				ss << ".." << max();
			break; 
		}

		if(gap == val+1)
			ss << " ";
		else
			ss << ".." << gap-1 << " ";
	}
	ss << "]";

	return ss.str();
}


}
