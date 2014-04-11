#ifndef FEATHER_BACKEND_NAXOS_H
#define FEATHER_BACKEND_NAXOS_H

#include <common/representation.h>
#include <feather/common/problem-manager.h>
#include <map>

namespace feather {

class NsIntVar;

class Naxos : public ProblemManager {

	private:
		/* Specifies whether a representation has already been provided */
		bool hasRepresentation;

		std::map<IntVarID, NsIntVar*> vars;

		void addVar(const RepresentationIntVar&);


	public:
		virtual void supplyRepresentation(const Representation&);

		Naxos() { }

		virtual ~Naxos() { }

};


} // namespace feather


#endif
