#ifndef FEATHER_BACKEND_NSINTVAR_H
#define FEATHER_BACKEND_NSINTVAR_H

#include <feather/int-domain.h>
#include <backend/naxos/naxos.h>

namespace feather {

class Ns_Constraint;

class NsIntVar {

	private:
		/* Pointer to the domain holding the values */
		IntDomain *domain;

		/* Reference to the naxos engine */
		Naxos &naxos;

	public:
		NsIntVar(IntDomain *domain, Naxos&);
		~NsIntVar();

		/*
		 * These methods allow the removal of a value plus the recording
		 * of the constraint that triggered the removal.
		 *
		 * If given constraint == NULL, no constraint is recorded
		 */

		void removeAll();
		
		// void remove(const Int val) {
		// 	remove(val, val);
		// }

		// void remove(const Int first, const Int last) {
		// 	remove(first, last, NULL);
		// }

		void save() {
			domain->save();
		}

		void restore() {
			domain->restore();
		}

		UInt size() const {
			return domain->size();
		}

		Naxos& getNaxos() const {
			return naxos;
		}








};




} // namespace feather

#endif