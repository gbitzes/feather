#include <feather/int-domain.h>
#include <feather/utils.h>

namespace feather {

void IntDomain::removeAllBut(IntDomain const* source) {
	Int val, gap;
	gap = kMinusInf;

	while( (val = source->next(gap)) != kPlusInf) {
		this->removeRange(gap, val-1);
		gap = source->nextGap(val);
	}

	this->removeRange(kMinusInf, source->min()-1);
	this->removeRange(source->max()+1, kPlusInf);
}

} // namespace feather

