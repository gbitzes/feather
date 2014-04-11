#include "naxos.h"
#include <common/representation.h>
#include <base/utils.h>
#include <base/bitset.h>
#include <vector>
#include "NsIntVar.h"

namespace feather {

void Naxos::supplyRepresentation(const Representation& repr) {
	/* Add all variables */
	std::vector<RepresentationIntVar>::const_iterator it;
	for(it = repr.vars.begin(); it != repr.vars.end(); it++)
		addVar(*it);
}

void Naxos::addVar(const RepresentationIntVar& var) {
	FEATHER_ASSERT(vars.find(var.id) == vars.end());

	IntDomain *domain = var.domain;
	Bitset *newdomain = new Bitset(domain->min(), domain->max() );
	newdomain->removeAllBut(domain);	

	NsIntVar *newvar = new NsIntVar(newdomain, *this);
	vars[var.id] = newvar;
}

}
