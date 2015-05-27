#include <feather/management/manager-generator.h>
#include <backend/naxos/naxos.h>

namespace feather {

ChildManager* NaxosGenerator::generate() {
  return new Naxos();
}  

ChildManager* BufferedNaxosGenerator::generate() {
  ChildManager *naxos = new Naxos();
  BufferManager *buffer = new BufferManager(bufsize, naxos);
  return buffer;
}



} // namespace feather
