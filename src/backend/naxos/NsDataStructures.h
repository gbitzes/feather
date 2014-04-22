#ifndef FEATHER_BACKEND_NSDATASTRUCTURES_H
#define FEATHER_BACKEND_NSDATASTRUCTURES_H

#include <feather/utils.h>
#include <queue>

namespace feather {

	/* A flexible array data structure (like std::deque)
	   A safer version of \c std::deque that throws an exception when an
	   out_of_range request is being made.  (It takes advantage of the
	   internal at() method that does this work) 
	*/

	template <class TemplType> 
	class  NsDeque : public std::deque<TemplType>  {

		public:

			NsDeque (void) : std::deque<TemplType>() {    
			}

			NsDeque (const typename std::deque<TemplType>::size_type n) : std::deque<TemplType>(n) {    
			}

			typename std::deque<TemplType>::reference 
			operator  [] (const typename std::deque<TemplType>::size_type i) {
				return  this->at(i);
			}

			typename std::deque<TemplType>::const_reference
			operator  [] (const typename std::deque<TemplType>::size_type i)  const {
				return  this->at(i);
			}

			typename std::deque<TemplType>::reference front (void) {
				if ( std::deque<TemplType>::empty() )
					throw  std::out_of_range("NsDeque::front: Empty deque");

				return  std::deque<TemplType>::front();
			}

			typename std::deque<TemplType>::const_reference front (void)  const {
				if ( std::deque<TemplType>::empty() )
					throw  std::out_of_range("NsDeque::front: Empty deque");

				return  std::deque<TemplType>::front();
			}


			typename std::deque<TemplType>::reference back (void) {
				if ( std::deque<TemplType>::empty() )
					throw  std::out_of_range("NsDeque::back: Empty deque");

				return  std::deque<TemplType>::back();
			}


			typename std::deque<TemplType>::const_reference back (void)  const {
				if ( std::deque<TemplType>::empty() )
					throw  std::out_of_range("NsDeque::back: Empty deque");

				return  std::deque<TemplType>::back();
			} 
	};


template <class TemplType>
class NsQueue : public std::queue<TemplType> {
	public:
		void clear() {
			while(!std::queue<TemplType>::empty())
				std::queue<TemplType>::pop();
		}
};		


template <class TemplType>
class NsStack {
	protected:
		struct StackNode_t {
			TemplType theData;
			StackNode_t *next;
			StackNode_t(const TemplType& T)
			: theData(T), next(0) {}
		};

		StackNode_t *stackTopNode;
	public:
		typedef TemplType value_type;
		typedef unsigned long size_type;
	private:
		size_type nFrames;
	public:
		NsStack() : stackTopNode(0), nFrames(0) {}

		NsStack(const NsStack& stackOther) {
			*this = stackOther;
		}

		~NsStack() {
			clear();
		}

		size_type size() const {
			return nFrames;
		}

		void clear() {
			while(!empty() )
				pop();
		}

		bool empty() const {
			return (stackTopNode == 0);
		}

		TemplType& top() {
			FEATHER_ASSERT(stackTopNode != 0);
			return  stackTopNode->theData;
		}

		const TemplType&  top (void)  const {
			FEATHER_ASSERT(stackTopNode != 0);
			return  stackTopNode->theData;
		}
		
		/* TemplType&  top_MinusOne (void) {
			assert_Ns( stackTopNode!=0 && stackTopNode->next!=0 ,  "NsStack::top_MinusOne: There is no second item in the stackTopNode");
			return  stackTopNode->next->theData;
		} */

		void  pop (void) {
			FEATHER_ASSERT(!empty());
			StackNode_t  *current = stackTopNode;
			stackTopNode = current->next;
			delete current;

			--nFrames;
		}

		void  push (const TemplType newData) {
			StackNode_t  *newNode = new StackNode_t(newData);
			//newNode->theData = newData;
			newNode->next = stackTopNode;
			stackTopNode = newNode;

			++nFrames;
		}

		/* Declaration necessary so the following
		   'friend' statement sees this 'iterator'
		   instead of std::iterator */
		
		class iterator;
		friend class iterator;

		class iterator  {
			
			private:
				StackNode_t  *currNode;

			public:
				iterator (void) : currNode(0) {

				}
				
				iterator (StackNode_t *startNode) : currNode(startNode) {

				}

				bool operator == (const iterator& b)  const {
					return  ( currNode  ==  b.currNode );
				}

				bool  operator != (const iterator& b)  const {
					return  ( ! ( *this  ==  b ) );
				}

				TemplType&  operator * (void)  const {
					FEATHER_ASSERT(currNode != 0);
					return  currNode->theData;
				}

				TemplType*  operator -> (void)  const {
					FEATHER_ASSERT(currNode != 0);
					return  &currNode->theData;
				}

				iterator&  end (void) {
					currNode = 0;
					return  *this;
				}

				iterator&  operator ++ (void) {
					FEATHER_ASSERT(currNode != 0);
					currNode = currNode->next;
					return  *this;
				}
		};

		iterator  begin (void) {
			return  iterator(stackTopNode);
		}

		iterator  end (void) {
			iterator  iter_end(stackTopNode);
			return  iter_end.end();
		}
};

}


#endif