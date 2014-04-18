#ifndef ___FEATHER_BASE_INTRANGES_H
#define ___FEATHER_BASE_INTRANGES_H

#include <feather/types.h>
#include <feather/int-domain.h>
#include <map>
#include <stack>

namespace feather {

class IntRangesPosition : public IntDomainPosition {

};

class IntRanges : public IntDomain {

	private:
		/*
		 * Where the actual ranges are
		 * stored - a key-value pair
		 * means that the values [key, value]
		 * are inside the domain
		 */

		std::map<Int, Int> intervals;
		
		/* Structures that hold all history so far */
		std::stack<Range> history;
		std::stack<Int> setcountHistory;


	public:
		IntRanges(Int minval, Int maxval);
		virtual ~IntRanges() {};

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
		virtual IntDomainPosition* getPosition(const Int value) const;
		virtual Int advance(IntDomainPosition &pos) const;
		virtual Int moveback(IntDomainPosition &pos) const;



};

} // namespace feather

#endif