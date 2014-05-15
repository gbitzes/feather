#ifndef FEATHER_INTDOMAIN_H
#define FEATHER_INTDOMAIN_H

#include <feather/types.h>
#include <string>

namespace feather {

/*
 * A class that holds additional information
 * as to the position of a value inside an
 * IntDomain.
 *
 * Depends on the implementation. A bitset could
 * hold which chunk and which bit inside the chunk
 * corresponds to value. A range set could store
 * the iterator to the range that contains some
 * particular value. 
 * 
 * This is in order to speed up things like next().
 * Knowing the exact chunk in a bitset will save us
 * the division and modulo to locate it each time.
 */
class IntDomainPosition {
	public:
		virtual ~IntDomainPosition() {};
		Int value;
};

class IntDomain {

	protected:
		Int minval, maxval;
		UInt setcount;

	public:

		IntDomain() { }

		virtual ~IntDomain() { }

		UInt size() const {
			return setcount;
		}

		Int min() const {
			return minval;
		}

		Int max() const {
			return maxval;
		}

		bool isBound() const {
			return min() == max();
		}

		virtual bool contains(const Int value) const = 0;
		virtual Int previous(const Int value) const = 0;
		virtual Int next(const Int value) const = 0;
		virtual Int nextGap(const Int value) const = 0;
		virtual bool containsRange(const Int rangeMin, const Int rangeMax) const = 0;
		virtual std::string toString() const = 0;

		
		enum RemovalResult {
			CHANGE = 1,
			NOCHANGE,
			WOULDCLEAR
		};

		virtual enum RemovalResult removeRange(Int rangeMin, Int rangeMax, bool saveOnChange = false) = 0;
		virtual void save() = 0;
		virtual void restore() = 0;

		/* 
		 * Removes all values inside the domain
		 * _except_ those inside the given argument
		 */
		 
		void removeAllBut(IntDomain const*);

	protected:

		/*
		 * Optimized functions that receive extra information
		 * about the internal state - should only be touched by
		 * iterators, not users
		 */
		
		/* 
		 * Returns a new object with implementation-specific information
		 * about the position of value.
		 * 
		 * If not found, returns NULL. The pointer points to a newly created
		 * object - responsibility to free its memory lays on the caller.
		 */
		virtual IntDomainPosition* getPosition(const Int value) const = 0;
		
		/* Returns the next value (similarly to next() ) and also advances pos by +1 */
		virtual Int advance(IntDomainPosition &pos) const = 0;

		/* Opposite of advance - moves pos by -1 */
		virtual Int moveback(IntDomainPosition &pos) const = 0;

	public:

		class Iterator {
			private:
				const IntDomain *domain;
				IntDomainPosition *pos;
				Int value;
			public:
				Iterator() {
					domain = NULL;
					pos = NULL;
					value = kPlusInf;
				}

				~Iterator() {
					if(pos != NULL)
						delete pos;
				}

				Iterator(const IntDomain &_domain) {
					domain = &_domain;
					value = domain->min();
					pos = domain->getPosition(value);
				}

				bool operator==(const Iterator& b) const {
					return value == b.value;
				}

				bool operator!=(const Iterator& b) const {
					return !(*this == b);
				}

				void operator=(const Iterator& b) {
					if(pos != NULL)
						delete pos;

					domain = b.domain;
					value = b.value;
					pos = domain->getPosition(value);
				}



				// TODO: throw exception if Iterator has reached the end?
				Int operator*() const {
					return pos->value;
				}

				Iterator& operator++() {
					value = domain->advance(*pos);
					return *this;
				}

				Iterator& operator--() {
					value = domain->moveback(*pos);
					return *this;
				}

				Iterator& end() {
					pos->value = kPlusInf;
					value = kPlusInf;
					return *this;
				}
		};

		class ReverseIterator {
			private:
				const IntDomain *domain;
				IntDomainPosition *pos;
				Int value;
			public:
				ReverseIterator() {
					domain = NULL;
					pos = NULL;
					value = kPlusInf;
				}

				~ReverseIterator() {
					if(pos != NULL)
						delete pos;
				}

				ReverseIterator(const IntDomain &_domain) {
					domain = &_domain;
					value = domain->max();
					pos = domain->getPosition(value);
				}

				bool operator==(const ReverseIterator& b) const {
					return value == b.value;
				}

				bool operator!=(const ReverseIterator& b) const {
					return !(*this == b);
				}

				// TODO: throw exception if ReverseIterator has reached the end?
				Int operator*() const {
					return pos->value;
				}

				ReverseIterator& operator++() {
					value = domain->moveback(*pos);
					return *this;
				}

				ReverseIterator& operator--() {
					value = domain->advance(*pos);
					return *this;
				}

				ReverseIterator& end() {
					pos->value = kMinusInf;
					value = kMinusInf;
					return *this;
				}
		};

		class GapIterator {
			private:
				const IntDomain *domain;
				Int value;
			public:
				GapIterator() {
					domain = NULL;
					value = kPlusInf;
				}

				~GapIterator() {
				}

				GapIterator(const IntDomain &_domain) {
					domain = &_domain;
					value = domain->nextGap(domain->min());
				}

				bool operator==(const GapIterator& b) const {
					return value == b.value;
				}

				bool operator!=(const GapIterator& b) const {
					return !(*this == b);
				}

				// TODO: throw exception if GapIterator has reached the end?
				Int operator*() const {
					return value;
				}

				GapIterator& operator++() {
					value = domain->nextGap(value);
					return *this;
				}

				GapIterator& end() {
					value = kPlusInf;
					return *this;
				}
		};


		friend class Iterator;
		friend class ReverseIterator;
		friend class GapIterator;

		const Iterator begin() const {
			return Iterator(*this);
		}

		const Iterator end() const {
			return Iterator();
		}

		const ReverseIterator rbegin() const {
			return ReverseIterator(*this);
		}

		const ReverseIterator rend() const {
			return ReverseIterator();
		}

		const GapIterator gap_begin() const {
			return GapIterator(*this);
		}

		const GapIterator gap_end() const {
			return GapIterator();
		}


};

} // namespace feather

#endif
