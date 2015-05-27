#include <feather/management/buffer-manager.h>
#include <feather/utils.h>
#include <common/representation.h>

namespace feather {

  BufferManager::BufferManager(Int bufsize, ChildManager *child) {
  this->bufsize = bufsize;
  this->child = child;
  child->setParent(this);
  parent = NULL;
  representation = NULL;
  firstSolution = true;
  endofsearch = false;
}

void BufferManager::cleanup() {

}

void BufferManager::restart() {
  if(parent == NULL)
    FEATHER_THROW("No parent set yet");

  child->restart();
  cleanup();
}

void BufferManager::supplyRepresentation(const Representation &representation) {
  this->representation = &representation;
  child->supplyRepresentation(representation);
}

void BufferManager::clearRepresentation() {
  this->representation = NULL;
}

BufferManager::~BufferManager() {
  delete child;
}

void BufferManager::updateMinObjValue(Int newBestValue) {
    if(parent) {
        parent->updateMinObjValue(newBestValue);
        return;
    }
	
}

IntDomain* BufferManager::getDomain(IntVarID var) {
  IntDomain *domain = safe.back()[var];
  IntDomain *newdomain = new IntRanges(domain->min(), domain->max() );
  newdomain->removeAllBut(domain);
  return newdomain;
  
  //return new IntRanges(safe.back()[var]);
}

Int BufferManager::getMinObjValue() {
  return parent->getMinObjValue();
}

void BufferManager::setInitialDecisions(std::vector<bool> decisions) {
    this->decisions = decisions;
    child->setInitialDecisions(decisions);
}

void BufferManager::setParent(ParentManager *parent) {
    this->parent = parent;
}

bool BufferManager::needMoreWork() {
  return parent->needMoreWork();
}

bool BufferManager::nextSolution() {
  if(!firstSolution) {
    for(auto var : safe.back() ) {
      delete var.second;
    }
    safe.pop_back();
  }

  firstSolution = false;

  if(safe.size() > 0)
    return true;

  if(endofsearch)
    return false;

  while(safe.size() < bufsize) {
    if(!child->nextSolution()) {
      endofsearch = true;
      break;
    }

    /* copy child's solutions */
    safe.push_back(std::map<Int, IntDomain*>());
    for(auto var : representation->vars)
      safe.back()[var.id] = child->getDomain(var.id);
  }

  if(safe.size() == 0) return false;
  return true;
}

void BufferManager::newInstance(std::vector<bool> decisions) {
  parent->newInstance(decisions);
}

} // namespace feather
