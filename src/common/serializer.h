#include <common/representation.h>
#include <string>

namespace feather {
class RepresentationSerializer {
	public:
		static std::string serialize(const Representation&);
		static Representation* deserialize(std::string&);

};
} // namespace feather
