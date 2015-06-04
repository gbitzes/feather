#ifndef FEATHER_BUILDENVMANAGER_H
#define FEATHER_BUILDENVMANAGER_H

#include <feather/types.h>

namespace feather {
	ProblemManager* buildEnvManager(bool logging = true);
	ChildManager* buildChildManager(bool logging = true);
} // namespace feather

#endif
