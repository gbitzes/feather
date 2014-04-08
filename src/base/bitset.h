#ifndef ___FEATHER_BASE_BITSET_H
#define ___FEATHER_BASE_BITSET_H

#include <feather/types.h>
#include <feather/int-domain.h>
#include <vector>

namespace feather {

class Bitset : public IntDomain {

	private:

		struct BitsetState {
			Int minval, maxval, setcount, minDom, nbits;
			std::vector<Word> *machw;

			BitsetState(Int minval, Int maxval, Int setcount) {
				this->minval = minval;
				this->maxval = maxval;
				this->setcount = setcount;

				machw = NULL;
			}
		};

		/*
		 * An array consisting of machine words in
		 * in which the bits of the bitset are stored
		 */

		std::vector<Word> *machw;

		/*
		 * Stores clones of the state of the bitset 
		 * Used by save() and restore()
		 */	

		 std::vector<BitsetState> clones;

		/*
		 * The 0-th bit of the bitset corresponds
		 * to the value stored in offset.
		 *
		 * (as opposed to the 0-th bit of the bitset
		 *  corresponding to value 0 - this way we
		 *  can store negative or very large values)
		 */

		Int offset;

		/*
		 * Number of bits (active or inactive) contained
		 * in the bitset
		 */
		UInt nbits;

	public:
		Bitset(Int minval, Int maxval);
		virtual ~Bitset() {};

		virtual bool contains(const Int value) const;
		virtual Int previous(const Int value) const;
		virtual Int next(const Int value) const;
		virtual Int nextGap(const Int value) const;
		virtual bool containsRange(const Int rangeMin, const Int rangeMax) const;
		virtual std::string toString() const;

		virtual enum IntDomain::RemovalResult removeRange(Int rangeMin, Int rangeMax, bool saveOnChange);
		virtual void save();
		virtual void restore();

	protected:
		virtual IntDomainPosition* getPosition(const Int value) const {};
		virtual Int advance(IntDomainPosition &pos) const {};
		virtual Int moveback(IntDomainPosition &pos) const {};
};

}


#endif