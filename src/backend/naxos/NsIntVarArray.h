#ifndef FEATHER_BACKEND_NSINTVARARRAY_H
#define FEATHER_BACKEND_NSINTVARARRAY_H

#include <deque>
#include <backend/naxos/NsIntVar.h>

namespace feather {

class NsIntVarArray : public std::deque<NsIntVar*> {};


} // namespace feather

#endif