#ifndef FEATHER_FRONTEND_CPP_INTVAR_EXPRESSIONS_H
#define FEATHER_FRONTEND_CPP_INTVAR_EXPRESSIONS_H

#include <feather/types.h>

namespace feather {
	
	class Expression {
		public:

			/*
			 * 
			 */

			virtual IntVar post() const = 0;

			/*
			 * The destructor of an abstract class should
			 * be virtual
			 */

			virtual ~Expression() { }
	};

}


#endif


	// class  Ns_Expression  {

	// 	public:

	// 		///  Posts the constraint \a VarX \c == \c *this
	// 		///   (\c *this represents an Ns_Expression).
	// 		virtual void       post (NsIntVar &VarX)  const = 0;

	// 		///  Produces/returns a variable to represent the
	// 		///   Ns_Expression (\c *this).
	// 		virtual NsIntVar&  post (void)  const = 0;

	// 		// The destructor of an abstract class should be virtual.
	// 		virtual ~Ns_Expression (void)  {   }
	// };
