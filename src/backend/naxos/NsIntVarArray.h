#ifndef FEATHER_BACKEND_NSINTVARARRAY_H
#define FEATHER_BACKEND_NSINTVARARRAY_H

#include <deque>
#include <backend/naxos/NsIntVar.h>
#include <backend/naxos/NsDataStructures.h>
#include <backend/naxos/basics.h>

namespace feather {

typedef  NsDeque<NsIntVar*>    Ns_PointArray_t;

	/* A flexible array data type that holds constrained variables (NsIntVars) */
	class  NsIntVarArray  {

		private:

			Ns_PointArray_t  PointArray;
			bool  addedConstraint;

		public:

			NsIntVarArray (void) : addedConstraint(false) {

			}

			// NsIntVarArray (const Ns_ExpressionArray& expr);
			
			// NsIntVarArray& operator = (const Ns_ExpressionArray& expr);

			/* Signifies that a constraint has been imposed on the array */
			void addConstraint (void) {
				addedConstraint = true;
			}

			void  push_front (const NsIntVar& Var);

			void  push_front (NsIntVar& Var) {
				assert_Ns( ! addedConstraint ,  "NsIntVarArray::push_front: Cannot add another variable, because a constraint has been already imposed on the array");
				PointArray.push_front( &Var );
			}

			// void  push_front (const Ns_Expression& expr);
			void  push_back (const NsIntVar& Var);

			void  push_back (NsIntVar& Var) {
				assert_Ns( ! addedConstraint ,  "NsIntVarArray::push_back: Cannot add another variable, because a constraint has been already imposed on the array");
				PointArray.push_back( &Var );
			}


			// void  push_back (const Ns_Expression& expr);

			class  const_iterator;

			/* Iterator that iterates through the constrained variables of the array */

			class  iterator  {
				
				private:
					Ns_PointArray_t  *PointArr;
					Ns_PointArray_t::iterator  it;

				public:
					
					friend class const_iterator;
					
					iterator (void) : PointArr(0) {

					}


					iterator (Ns_PointArray_t& PointArr_init) : PointArr(&PointArr_init), it(PointArr->begin()) {

					}

					bool operator == (const iterator& b)  const {
						assert_Ns(PointArr != 0, "NsIntVarArray::iterator::==: Uninitialized `*this'");
						return  ( it  ==  b.it );
					}


					bool  operator != (const iterator& b)  const {
						return  ! ( *this  ==  b );
					}

					NsIntVar&  operator * (void)  const {
						assert_Ns(PointArr != 0, "NsIntVarArray::iterator::*: Uninitialized `*this'");
						assert_Ns(it != PointArr->end(), "NsIntVarArray::iterator::*: Bad request `*(something.end())'");

						return  **it;
					}


					NsIntVar*  operator -> (void)  const {
						assert_Ns(PointArr != 0, "NsIntVarArray::iterator::*: Uninitialized `*this'");
						assert_Ns(it != PointArr->end(), "NsIntVarArray::iterator::*: Bad request `*(something.end())'");

						return  *it;
					}


					iterator&  end (void) {
						assert_Ns(PointArr != 0, "NsIntVarArray::iterator::end: Uninitialized `*this'");

						it = PointArr->end();
						return  *this;
					}

					iterator&  operator ++ (void) {
						assert_Ns(PointArr != 0, "NsIntVarArray::iterator::++: Uninitialized `*this'");
						assert_Ns(it != PointArr->end(), "NsIntVarArray::iterator::end: Bad request `++(something.end())'");

						++it;
						return  *this;
					}
			};


			iterator begin (void) {
				return  iterator(PointArray);
			}


			iterator end (void) {
				iterator  iterEnd(PointArray);
				return  iterEnd.end();
			}

			/*  Iterator that iterates through the constrained variables of the array (without modifying them) */

			class  const_iterator  {

				private:

					const Ns_PointArray_t  *PointArr;
					Ns_PointArray_t::const_iterator  it;

				public:

					const_iterator (void) : PointArr(0) {

					}


					const_iterator (const Ns_PointArray_t& PointArr_init) 
						: PointArr(&PointArr_init),
						it(PointArr->begin()) {

					}


					const_iterator (const iterator iter) : PointArr(iter.PointArr), it(iter.it) {

					}

					const_iterator& operator = (const iterator iter) {
						PointArr = iter.PointArr;
						it = iter.it;
						return  *this;
					}


					bool  operator == (const const_iterator& b)  const {
						assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::==: Uninitialized `*this'");

						return  ( it == b.it );
					}


					bool  operator != (const const_iterator& b)  const {
						return  ! ( *this == b );
					}


					const NsIntVar&  operator * (void)  const {
						assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::*: Uninitialized `*this'");
						assert_Ns(it != PointArr->end(), "NsIntVarArray::const_iterator::*: Bad request `*(something.end())'");

						return  **it;
					}


					const NsIntVar*  operator -> (void)  const {
						assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::*: Uninitialized `*this'");
						assert_Ns(it != PointArr->end(), "NsIntVarArray::const_iterator::*: Bad request `*(something.end())'");

						return  *it;
					}


					const_iterator&  end (void) {
						assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::end: Uninitialized `*this'");

						it = PointArr->end();
						return  *this;
					}


					const_iterator&  operator ++ (void) {
						assert_Ns(PointArr != 0, "NsIntVarArray::const_iterator::++: Uninitialized `*this'");
						assert_Ns(it != PointArr->end(), "NsIntVarArray::const_iterator::end: Bad request `++(something.end())'");

						++it;
						return  *this;
					}
			};


			const_iterator begin (void)  const {
				return  const_iterator(PointArray);
			}


			const_iterator end (void)  const {
				const_iterator  iterEnd(PointArray);
				return  iterEnd.end();
			}


			NsIntVar& operator  [] (const NsIndex i) {
				return  *PointArray[i];
			}


			const NsIntVar& operator  [] (const NsIndex i)  const {
				return  *PointArray[i];
			}


			NsIntVar& front (void) {
				return  *PointArray.front();
			}


			NsIntVar& back (void) {
				return  *PointArray.back();
			}


			NsIndex size (void)  const {
				return  PointArray.size();
			}


			bool empty (void)  const {
				return  PointArray.empty();
			}

	};


} // namespace feather

#endif