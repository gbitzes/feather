#include "bitset.h"
#include "utils.h"
#include <stdio.h>

using namespace std;

namespace feather {

namespace {

	/*
	 * Returns a machine word with its bits set to 1 from start to end
	 * Example: mwWithSetBits(2, 5) should return something like 011110000...
	 */
	inline Word mwWithBits(Int start, Int end) {
		Word all = ~0ull;
		Word left = all << start-1;
		Word right = (all >> ( (sizeof(Word)*CHAR_BIT) -end) );
		return left & right;
	}

	/*
	 * Returns the number of bits that are turned on inside a word 
	 */

	inline UInt popcount(Word w) {
		return __builtin_popcountll(w);
	}

	/*
	 * Return the number of leading zeroes
	 */

	inline UInt bitscan(Word w) {
		return __builtin_ffsll(w);
	}

	/*
	 * Return the number of trailing zeroes
	 */

	inline UInt bitscanReverse(Word w) {
		return __builtin_clzll(w)+1;
	}

	/* Print a machine word of the bitset in binary - useful for debugging */
	void printWord(Word mw) {
		static char buffer[MW_BITS];		
		char* ptr = buffer;

		ptr += (MW_BITS - 1);

		Word original = mw;

		for(int i = MW_BITS - 1; i >= 0; i--) {
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
	
	machw = new std::vector<Word>();
	save();
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
		return wouldclear;
	}

	/* Domain remains intact? */
	if(rangeMax < minval  ||  maxval < rangeMin) {
		return nochange;
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
		 	return change;
		 }
		 else if(maxval <= rangeMax) {
		 	setcount -= maxval - (rangeMin - 1);
		 	maxval = rangeMin - 1;
		 	return change;
		 }
		 else {
		 	/*
		 	 * Nope, no luck. It's necessary to initialize the machw array this time
		 	 * Initially, each position just contains machine words with all their bits set
		 	 */

		 	machw->resize((nbits-1)/MW_BITS + 1, ~0ul);
		 }
	}
	/* machw will change, record it if you haven't already */
	if(clones.back().machw == NULL)
		clones.back().machw = new std::vector<Word>(*machw);

	/* Calculate the machine words that correspond to rangeMin and rangeMax */
	UInt minbit = rangeMin - offset;
	UInt minmw = minbit / MW_BITS;

	UInt maxbit = rangeMax - offset;
	UInt maxmw = maxbit / MW_BITS;

	/* Case 1: All values reside in the same machine word */
	if(minmw == maxmw) {
		Int start = minbit % MW_BITS + 1;
		Int end = maxbit % MW_BITS + 1;

		Word w = mwWithBits(start, end);
		setcount -= popcount( (*machw)[minmw] & w );
		(*machw)[minmw] = (*machw)[minmw] & ~w;
	}
	/* Case 2: More than one machine words */
	else {
		/* Take care of the first */
		Int start = minbit % MW_BITS + 1;
		Word word = ~0ull << start-1;
		setcount -= popcount((*machw)[minmw] & word);
		(*machw)[minmw] = (*machw)[minmw] & ~word;

		/* Take care of the last */
		Int end = maxbit % MW_BITS + 1;
		word = ~(~0ull << end);
		setcount -= popcount((*machw)[maxmw] & word);
		(*machw)[maxmw] = (*machw)[maxmw] & ~word;

		/* Take care of the in-between words, if they exist */
		for(int i = minmw+1; i <= maxmw-1; i++) {
			setcount -= popcount((*machw)[i]);
			(*machw)[i] = 0ull;
		}
	}

	if (rangeMin == minval)
		minval = next(minval);
	if (rangeMax == maxval)
		maxval = previous(maxval);

	return change;
}

bool Bitset::contains(const Int val) const {
	/* Is value out of bounds? If so, it's impossible that it's active */
	if ( maxval < val || val < minval )
		return false;

	/* Has machw been initialized? If not, the value is certainly inside the domain */
	if( machw->empty() )
		return true;

	UInt bit = val - offset;
	UInt mw = bit / MW_BITS;
	Word word = 1ll << bit % MW_BITS;

	return (*machw)[mw] & word;
}

Int Bitset::next(const Int val) const {
	if(val >= maxval)
		return kPlusInf;
	if(val < minval)
		return minval;

	if(machw->empty())
		return val+1;

	UInt nbit = val - offset + 1;
	UInt mw = nbit / MW_BITS;
	Word word = (*machw)[mw] >> (nbit%MW_BITS);

	/* Our next value is in the same machine word */
	if(word != 0ul) {
		/* Calculate the number of leading zeroes */
		UInt zeroes = bitscan(word);
		return val + zeroes;
	}
	/* Our next value is not in the same machine word.. need to keep looking */
	else {
		Int size = machw->size();
		for(Int i = mw+1; i < machw->size(); i++) {
			word = machw->at(i);

			if(word != 0ul) {
				Int zeroes = bitscan(word);
				return val + (MW_BITS - nbit%MW_BITS) + (MW_BITS*(i-mw-1)) + zeroes;
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
	// Int        mw  =  nbit / MW_BITS;
	// unsigned long  mwbit  =  1l<<(nbit%MW_BITS);

	// // assert_Ns( nbit < nbits ,
	// // 	"NsBitset::previous: Machine word out of `*this' range");

	// if ( (*machw)[mw]  ==  0ul )    {
	// 	// speedup by means of comparing the whole word
	// 	nbit  =  mw * MW_BITS -1;

	// }  else  {

	// 	for ( ;   mwbit != 0;   mwbit>>=1)    {

	// 		if ( (*machw)[mw] & mwbit )
	// 			return  (nbit + offset);

	// 		--nbit;
	// 	}
	// }

	// --mw;

	// for ( ;   mw >= 0;   --mw)    {

	// 	if ( (*machw)[mw]  ==  0ul )    {
	// 		// speedup by means of comparing the whole word
	// 		nbit  -=  MW_BITS;

	// 	}  else  {

	// 		for (mwbit=1l<<(MW_BITS-1);  mwbit!=0;  mwbit>>=1)  {

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
	UInt mw = nbit / MW_BITS;
	Word word = (*machw)[mw] << MW_BITS - (nbit%MW_BITS);

	/* Our next value is in the same machine word */
	if(word != 0ul) {
		/* Calculate the number of leading zeroes */
		UInt zeroes = bitscanReverse(word);
		//printWord(word);
		return val - zeroes;
	}
	/* Our next value is not in the same machine word.. need to keep looking */
	else {
		Int size = machw->size();
		for(Int i = mw-1; i >= 0; i--) {
			word = machw->at(i);

			if(word != 0ul) {
				Int zeroes = bitscanReverse(word);
				return val - (nbit%MW_BITS) - (MW_BITS*(mw-i-1)) - zeroes;
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

	UInt       mw  =  nbit / MW_BITS;
	unsigned long  mwbit  =  1l<<(nbit%MW_BITS);
	UInt   maxbit  =  maxval-offset;

	// assert_Ns( maxbit < nbits  &&  nbit < nbits ,
	// 	"NsBitset::nextGap: Machine word out of `*this' range");

	if ( (*machw)[mw]  ==  ~0ul )    {
		// speedup by means of comparing the whole word
		nbit  =  (mw + 1) * MW_BITS;

	}  else  {

		for ( ;   mwbit!=0  &&  nbit <= maxbit;   mwbit<<=1)    {

			if ( ! ( (*machw)[mw] & mwbit ) )
				return  (nbit + offset);

			++nbit;
		}
	}

	++mw;

	for ( ;   mw < machw->size();   ++mw)    {

		if ( (*machw)[mw]  ==  ~0ul )    {
			// speedup by means of comparing the whole word
			nbit  +=  MW_BITS;

		}  else  {

			for (mwbit=1;  mwbit!=0 && nbit<=maxbit;  mwbit<<=1)
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
		UInt  mw    =  nbit / MW_BITS;

		if ( (*machw)[mw]  ==  ~0ul )    {
			// speedup by means of comparing the whole word

			if ( MW_BITS - nbit % MW_BITS  >
				static_cast<UInt>( rangeMax - val ) )
			{
				break;
			}

			val  +=  MW_BITS - nbit % MW_BITS - 1;
			// `-1' above is used to compensate
			//  the for's `++val' increment.

		}  else  {

			unsigned long  mwbit  =  1l<<(nbit%MW_BITS);

			if ( ! ( (*machw)[mw] & mwbit ) )
				return  false;
		}
	}

	return  true;


}

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
