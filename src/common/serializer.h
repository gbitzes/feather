#include <common/representation.h>
#include <string>

namespace feather {
class RepresentationSerializer {
	
	public:
		static std::string serialize(Representation&);
		static Representation* unserialize(std::string&);

};
} // namespace feather
