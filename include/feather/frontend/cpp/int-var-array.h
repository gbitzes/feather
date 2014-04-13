#ifndef FEATHER_FRONTEND_CPP_INTVARARRAY_H
#define FEATHER_FRONTEND_CPP_INTVARARRAY_H

#include <feather/types.h>
#include <base/utils.h>
#include "solver.h"
#include "types.h"
#include <deque>

namespace feather {

class RepresentationIntVarArray;

class IntVarArray {
	private:
		/* 
		 * Order of declaration is super-important here.
		 * As all members are initialized in the initialization-list,
		 * the order of initialization depends not on the order
		 * of the initialization-list but the order in which
		 * they were declared!
		 */
	
		Solver &slv;
		IntVarArrayID id;
		RepresentationIntVarArray &array;

	public:
		IntVarArray(Solver &solver) 
			: slv(solver), id(slv.makeIntVarArray()), 
			  array(slv.getArray(id)) { }

		void push_front(const IntVar&);
		void push_back(const IntVar&);

		IntVarArrayID getID() { 
			return id;
		}

		class const_iterator;

		/* 
		 * Iterator that iterates through the 
		 * variables of the array 
		 */
		class iterator {
			private:
				bool initialized;
				// Ns_PointArray_t  *PointArr;
				Solver* slv;
				std::deque<IntVarID> *arr;
				std::deque<IntVarID>::iterator it;
			public:
				// friend class const_iterator;
				
				iterator() : initialized(false) {}
				iterator(Solver& slv_init, std::deque<IntVarID> *arr_init)
					: initialized(true), slv(&slv_init), arr(arr_init), it(arr->begin()) { }

				bool operator==(const iterator& b) const {
					if(!initialized) FEATHER_THROW("Uninitialized iterator");
					return (&slv == &b.slv && it == b.it );
				}

				bool operator!=(const iterator& b) const {
					return !(*this == b);
				}

				IntVar operator*() const {
					if(!initialized) FEATHER_THROW("Uninitialized iterator");
					if(it == arr->end() ) FEATHER_THROW("Cannot dereference iterator - reached end of container");
					return IntVar(*it, *slv );
				}

				iterator end() {
					if(!initialized) FEATHER_THROW("Uninitialized iterator");
					iterator newiter = *this;
					newiter.it = arr->end();
					return newiter;
				}

				iterator& operator++() {
					if(!initialized) FEATHER_THROW("Uninitialized iterator");
					if(it == arr->end() ) FEATHER_THROW("Cannot dereference iterator - reached end of container");
					
					++it;
					return *this;
				}
		};

		iterator begin(); 
		iterator end();

		UInt size() const;
		bool empty() const;

		/*  Iterator that iterates through the constrained variables of the array (without modifying them) */

		// class  const_iterator  {

		// 	private:

		// 		const Ns_PointArray_t  *PointArr;
		// 		Ns_PointArray_t::const_iterator  it;

		// 	public:

		// 		const_iterator (void) : PointArr(0) {

		// 		}


		// 		const_iterator (const Ns_PointArray_t& PointArr_init) 
		// 			: PointArr(&PointArr_init),
		// 			it(PointArr->begin()) {

		// 		}


		// 		const_iterator (const iterator iter) : PointArr(iter.PointArr), it(iter.it) {

		// 		}

		// 		const_iterator& operator = (const iterator iter) {
		// 			PointArr = iter.PointArr;
		// 			it = iter.it;
		// 			return  *this;
		// 		}


		// 		bool  operator == (const const_iterator& b)  const {
		// 			assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::==: Uninitialized `*this'");

		// 			return  ( it == b.it );
		// 		}


		// 		bool  operator != (const const_iterator& b)  const {
		// 			return  ! ( *this == b );
		// 		}


		// 		const NsIntVar&  operator * (void)  const {
		// 			assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::*: Uninitialized `*this'");
		// 			assert_Ns(it != PointArr->end(), "NsIntVarArray::const_iterator::*: Bad request `*(something.end())'");

		// 			return  **it;
		// 		}


		// 		const NsIntVar*  operator -> (void)  const {
		// 			assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::*: Uninitialized `*this'");
		// 			assert_Ns(it != PointArr->end(), "NsIntVarArray::const_iterator::*: Bad request `*(something.end())'");

		// 			return  *it;
		// 		}


		// 		const_iterator&  end (void) {
		// 			assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::end: Uninitialized `*this'");

		// 			it = PointArr->end();
		// 			return  *this;
		// 		}


		// 		const_iterator&  operator ++ (void) {
		// 			assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::++: Uninitialized `*this'");
		// 			assert_Ns(it != PointArr->end(), "NsIntVarArray::const_iterator::end: Bad request `++(something.end())'");

		// 			++it;
		// 			return  *this;
		// 		}
		// };


		// const_iterator begin (void)  const {
		// 	return  const_iterator(PointArray);
		// }


		// const_iterator end (void)  const {
		// 	const_iterator  iterEnd(PointArray);
		// 	return  iterEnd.end();
		// }

		IntVar operator[](const UInt index);

		// NsIntVar& operator  [] (const NsIndex i) {
		// 	return  *PointArray[i];
		// }


		// const NsIntVar& operator  [] (const NsIndex i)  const {
		// 	return  *PointArray[i];
		// }


		// NsIntVar& front (void) {
		// 	return  *PointArray.front();
		// }


		// NsIntVar& back (void) {
		// 	return  *PointArray.back();
		// }


		// NsIndex size (void)  const {
		// 	return  PointArray.size();
		// }


		// bool empty (void)  const {
		// 	return  PointArray.empty();
		// }

	//};


};


} // namespace feather

#endif
