#ifndef FEATHER_BACKEND_INTERNAL_H
#define FEATHER_BACKEND_INTERNAL_H


#include "basics.h"

namespace feather {

bool intersectionEmpty (const NsIntVar *VarY, const NsIntVar *VarZ);

void YmodC_min_max (const NsIntVar *VarY, const Int C, Int& min, Int& minmax);

void product_min_max (const NsIntVar *VarY, const NsIntVar *VarZ, Int& min, Int& max);

void quotient_min_max (const NsIntVar *VarY, NsIntVar *VarZ, Int& min, Int& max);

void array_min_minmax (const NsIntVarArray *VarArr, Int& min, Int& minmax);

void array_maxmin_max (const NsIntVarArray *VarArr, Int& maxmin, Int& max);

void array_sum_min_max (const NsIntVarArray *VarArr, const UInt start, const UInt length, Int& summin, Int& summax);


/* Returns floor( x / y ) - overrides C problems when an operand is negative */
inline Int xDIVy (const Int x, const Int y) {
	if ( x < 0  &&  y < 0 )
		return  (-x / -y);
	else if ( x  <  0 )
		return  -((-x+y-1) / y);
	else if ( y  <  0 )
		return  -((x-y-1) / -y);
	else
		return  (x / y);
}

} // namespace feather

#endif