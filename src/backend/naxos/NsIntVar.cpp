#include "NsIntVar.h"

namespace feather {

NsIntVar::NsIntVar(IntDomain *domain, Naxos &naxos_) : naxos(naxos_) {
	this->domain = domain;
}

NsIntVar::~NsIntVar() {
	delete domain;
}

} // namespace feather



