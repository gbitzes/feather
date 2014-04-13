#ifndef FEATHER_FRONTEND_CPP_INTVAR_EXPRESSIONS_H
#define FEATHER_FRONTEND_CPP_INTVAR_EXPRESSIONS_H

#include <feather/types.h>
#include "int-var.h"

namespace feather {
	
	/*
	 * Y + C
	 */

	IntVar operator+ (IntVar Y, Int C);
	IntVar operator+ (Int C, IntVar Y);
	IntVar operator- (IntVar Y, Int C);

	/*
	 * Y / C
	 */

	IntVar operator/ (IntVar Y, Int C);

	/*
	 * Y + Z
	 */

	IntVar operator+ (IntVar Y, IntVar Z);

	/*
	 * Y - Z
	 */

	IntVar operator- (IntVar Y, IntVar Z);

	/*
	 * C - Z
	 */

	IntVar operator- (Int C, IntVar Z);
	IntVar operator- (IntVar Z);

	/*
	 * Y * Z
	 */

	IntVar operator* (IntVar Y, IntVar Z);

	/*
	 * Y * C
	 */

	IntVar operator* (IntVar Y, Int C);
	IntVar operator* (Int C, IntVar Y);


	// TODO? 
	// IntVar operator- (Int C, IntVar Y);





	/*
	 * Represents an expression
	 * formed by variables.
	 * For example, a + b, b*c + a,
	 * a*a, a+b>2 can all be represented 
	 * as an Expression object
	 */

	// class Expression {
	// 	public:

	// 		/*
	// 		 * Produces a new variable that
	// 		 * represents this expression,
	// 		 * attaching to it all necessary
	// 		 * constraints
	// 		 */

	// 		virtual IntVar post() = 0;

			
	// 		 * Implicitly convert an Expression
	// 		 * into an IntVar
			 

	// 		operator IntVar() const {
	// 			return post();
	// 		}

	// 		/*
	// 		 * The destructor of an abstract class should
	// 		 * be virtual
	// 		 */

	// 		virtual ~Expression() { }
	// };



	// class ExprYplusC : public Expression {
	// 	private:
	// 		IntVar varY;
	// 		Int C;
	// 	public:
	// 		ExprYplusC(IntVar Y, Int _C) : varY(Y), C(_C) {}
	// 		virtual IntVar post() const;
	// };

	// inline ExprYplusC operator+ (IntVar Y, const Int C) {
	// 	return ExprYplusC(Y, C);
	// }

	// inline ExprYplusC operator+ (const Int C, IntVar Y) {
	// 	return Y+C;
	// }



}


#endif
