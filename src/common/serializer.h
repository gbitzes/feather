#include <common/representation.h>
#include <feather/common/problem-manager.h>
#include <string>

namespace feather {

class StateSerializer {
public:
    static std::string serialize(const SearchState&, bool);
    static SearchState deserialize(const std::string&, bool);
};

class RepresentationSerializer {
public:
    static std::string serialize(const Representation&);
    static Representation* deserialize(std::string&);
};

} // namespace feather
