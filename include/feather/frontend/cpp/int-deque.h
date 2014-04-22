#ifndef FEATHER_FRONTEND_CPP_INTDEQUE_H
#define FEATHER_FRONTEND_CPP_INTDEQUE_H

#include <deque>
#include <feather/frontend/cpp/solver.h>

class RepresentationIntDeque;

namespace feather {

class IntDeque {
	private:
		Solver &slv;
		IntDequeID id;
		RepresentationIntDeque &deque;
	public:

		IntDeque(Solver &slv, UInt size = 0);

		typename std::deque<Int>::reference 
		operator[](const typename std::deque<Int>::size_type i);

		typename std::deque<Int>::const_reference
		operator[](const typename std::deque<Int>::size_type i) const;

		void push_front(const Int&);
		void push_back(const Int&);

		UInt size() const;
		bool empty() const;

		typedef std::deque<Int>::iterator iterator;
		typedef std::deque<Int>::iterator const_iterator;

		iterator begin();
		iterator end();

		IntVar operator[](IntVar index);

		// Ns_ExprElement operator  [] (NsIntVar& VarIndex)  const {
		// 	return  Ns_ExprElement(VarIndex, *this);
		// }

		// Ns_ExprElement operator  [] (const Ns_Expression& VarIndexExpr)  const {
		// 	return  (*this)[VarIndexExpr.post()];
		// }


		// // Ns_ExprElement operator  [] (NsIntVar& VarIndex)  const {
		// // 	return  Ns_ExprElement(VarIndex, *this);
		// // }

		// // Ns_ExprElement operator  [] (const Ns_Expression& VarIndexExpr)  const {
		// // 	return  (*this)[VarIndexExpr.post()];
		// // }
		
		// typename std::deque<Int>::reference front (void) {
		// 	if ( std::deque<Int>::empty() )
		// 		throw  std::out_of_range("NsDeque::front: Empty deque");

		// 	return  std::deque<Int>::front();
		// }

		// typename std::deque<Int>::const_reference front (void)  const {
		// 	if ( std::deque<Int>::empty() )
		// 		throw  std::out_of_range("NsDeque::front: Empty deque");

		// 	return  std::deque<Int>::front();
		// }

		// typename std::deque<Int>::reference back (void) {
		// 	if ( std::deque<Int>::empty() )
		// 		throw  std::out_of_range("NsDeque::back: Empty deque");

		// 	return  std::deque<Int>::back();
		// }

		// typename std::deque<Int>::const_reference back (void)  const {
		// 	if ( std::deque<Int>::empty() )
		// 		throw  std::out_of_range("NsDeque::back: Empty deque");

		// 	return  std::deque<Int>::back();
		// } 
};

} // namespace feather

#endif
