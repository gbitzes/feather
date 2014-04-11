#ifndef FEATHER_COMMON_PROBLEMMANAGER_H
#define FEATHER_COMMON_PROBLEMMANAGER_H

#include <feather/types.h>

namespace feather {

class Representation;

/*
 * The basic interface from which middle managers 
 * and backends inherit
 *
 * It defines methods with which to supply the representation
 * of the problem and receive all results
 */

class ProblemManager {

	public:
		virtual void supplyRepresentation(const Representation &) = 0;

};




} // namespace feather



#endif