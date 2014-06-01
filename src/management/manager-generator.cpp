#include <feather/management/manager-generator.h>
#include <backend/naxos/naxos.h>

namespace feather {

ChildManager* NaxosGenerator::generate() {
	return new Naxos();
}

} // namespace feather