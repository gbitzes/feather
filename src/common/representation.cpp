#include "representation.h"
#include <feather/utils.h>
#include <common/serializer.h>
#include <vector>

namespace feather {

IntVarID Representation::addVar(Int min, Int max) {
	IntVarID id = vars.size();
	IntRanges *domain = new IntRanges(min, max);

	RepresentationIntVar r;
	r.id = id;
	r.domain = domain;

	vars.push_back(r);

	return id;
}

IntVarArrayID Representation::addArray() {
	IntVarArrayID id = arrays.size();
	RepresentationIntVarArray *r = new RepresentationIntVarArray();
	r->id = id;
	arrays.push_back(r);
	return id;
}

IntDequeID Representation::addDeque(UInt size) {
	IntDequeID id = deques.size();
	RepresentationIntDeque *r = new RepresentationIntDeque(size);
	r->id = id;
	deques.push_back(r);
	return id;
}

Representation::~Representation() {
    //RepresentationSerializer ser;
    //std::string s = ser.serialize(*this);
    //std::cout << s << std::endl;
    //std::cout << "SECOND PASS" << std::endl;
    //std::cout << ser.serialize(*ser.deserialize(s)) << std::endl;
	/* Delete vars */
	{
		std::vector<RepresentationIntVar>::iterator it;
		for(it = vars.begin(); it != vars.end(); it++)
			delete it->domain;
	}
	/* Delete constraints */
	{
		std::vector<const Constraint*>::iterator it;
		for(it = constraints.begin(); it != constraints.end(); it++)
			delete *it;
	}
	/* Delete var arrays */
	{
		std::vector<RepresentationIntVarArray*>::iterator it;
		for(it = arrays.begin(); it != arrays.end(); it++)
			delete *it;
	}
	/* Delete deques */
	{
		std::vector<RepresentationIntDeque*>::iterator it;
		for(it = deques.begin(); it != deques.end(); it++)
			delete *it;
	}
    /* Delete goals */
    {
        std::vector<const Goal*>::iterator it;
        for(it = goals.begin(); it != goals.end(); it++)
            delete *it;
    }

}

} // namespace feather
