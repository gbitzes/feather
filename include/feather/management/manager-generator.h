#ifndef FEATHER_MANAGEMENT_MANAGERGENERATOR_H
#define FEATHER_MANAGEMENT_MANAGERGENERATOR_H

#include <feather/common/problem-manager.h>
#include <feather/management/buffer-manager.h>

namespace feather {

class ChildGenerator {
	public:
		virtual ChildManager* generate() = 0;
};

class NaxosGenerator : public ChildGenerator {
	public:
		virtual ChildManager* generate();
};

class BufferedNaxosGenerator : public ChildGenerator {
       private:
                Int bufsize;
       public:
                virtual ChildManager* generate();
		BufferedNaxosGenerator(Int bufsize) {
		    this->bufsize = bufsize;
		}
   
};

} // namespace feather
#endif
