#ifndef FEATHER_MANAGEMENT_MANAGERGENERATOR_H
#define FEATHER_MANAGEMENT_MANAGERGENERATOR_H

#include <feather/common/problem-manager.h>

namespace feather {

class ChildGenerator {
	public:
		virtual ChildManager* generate() = 0;
};

class NaxosGenerator : public ChildGenerator {
	public:
		virtual ChildManager* generate();
};

} // namespace feather
#endif