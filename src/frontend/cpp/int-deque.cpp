#include <feather/frontend/cpp/int-deque.h>
#include <feather/frontend/cpp/solver.h>
#include <common/representation.h>

namespace feather {

IntDeque::IntDeque(Solver &solver, UInt size)
	: slv(solver), id(slv.makeIntDeque(size)), deque(slv.getDeque(id)) {}

std::deque<Int>::reference 
IntDeque::operator[](const std::deque<Int>::size_type i) {
	return deque.contents[i];
}

std::deque<Int>::const_reference
IntDeque::operator[](const std::deque<Int>::size_type i) const {
	return deque.contents[i];
}

void IntDeque::push_front(const Int& val) {
	return deque.contents.push_front(val);
}

void IntDeque::push_back(const Int& val) {
	return deque.contents.push_back(val);
}

UInt IntDeque::size() const {
	return deque.contents.size();
}

bool IntDeque::empty() const {
	return deque.contents.empty();
}

IntDeque::iterator IntDeque::begin() {
	return deque.contents.begin();
}

IntDeque::iterator IntDeque::end() {
	return deque.contents.end();
}

IntDequeID IntDeque::getID() const {
	return id;
}

// IntDeque::IntDeque(Solver &solver, UInt size) : slv(Solver), id(slv.makeIntDeque(size)), 
// 				  deque(slv.getDeque(id)) {

// }


} // namespace feather
