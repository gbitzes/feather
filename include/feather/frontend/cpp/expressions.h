#ifndef FEATHER_FRONTEND_CPP_INTVAR_EXPRESSIONS_H
#define FEATHER_FRONTEND_CPP_INTVAR_EXPRESSIONS_H

#include <feather/types.h>
#include <feather/frontend/cpp/int-var.h>
#include <feather/frontend/cpp/int-var-array.h>


namespace feather {
	
	/*
	 * Represents an expression
	 * formed by variables.
	 * For example, a + b, b*c + a,
	 * a*a, a+b>2 can all be represented 
	 * as an Expression object
	 */

	class Expression {
		public:

			/*
			 * Produces a new variable that
			 * represents this expression,
			 * attaching to it all necessary
			 * constraints
			 */

			virtual IntVar post() = 0;

			/*
			 * Implicitly convert an Expression
			 * into an IntVar
			 */ 

			operator IntVar() {
				return post();
			}

			/*
			 * The destructor of an abstract class should
			 * be virtual
			 */

			virtual ~Expression() { }
	};

	/*
	 * Y + C
	 */

	class ExprYplusC : public Expression {
		private:
			IntVar Y;
			Int C;
		public:
			ExprYplusC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual IntVar post();
	};

	inline ExprYplusC operator+ (IntVar Y, Int C) {
		return ExprYplusC(Y, C);
	}

	inline ExprYplusC operator+ (Int C, IntVar Y) {
		return Y+C;
	}

	inline ExprYplusC operator- (IntVar Y, Int C) {
		return Y + (-C);
	}

	/*
	 * Y / C
	 */

	class ExprYdivC : public Expression {
		private:
			IntVar Y;
			Int C;
		public:
			ExprYdivC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual IntVar post();
	};


	inline ExprYdivC operator/ (IntVar Y, Int C) {
		return ExprYdivC(Y, C);
	}

	/*
	 * Y + Z
	 */

	class ExprYplusZ : public Expression {
		private:
			IntVar Y, Z;
		public:
			ExprYplusZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual IntVar post();
	};
 
	inline ExprYplusZ operator+ (IntVar Y, IntVar Z) {
		return ExprYplusZ(Y, Z);
	}

	/*
	 * Y - Z
	 */

	class ExprYminusZ : public Expression {
		private:
			IntVar Y, Z;
		public:
			ExprYminusZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprYminusZ operator- (IntVar Y, IntVar Z) {
		return ExprYminusZ(Y, Z);
	}

	/*
	 * C - Z
	 */

	class ExprCminusZ : public Expression {
		private:
			Int C;
			IntVar Z;
		public:
			ExprCminusZ(Int C_, IntVar Z_) : C(C_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprCminusZ operator- (Int C, IntVar Z) {
		return ExprCminusZ(C, Z);
	}
	inline ExprCminusZ operator- (IntVar Z) {
		return 0 - Z;
	}

	/*
	 * Y * Z
	 */

	class ExprYtimesZ : public Expression {
		private:
			IntVar Y, Z;
		public:
			ExprYtimesZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprYtimesZ operator* (IntVar Y, IntVar Z) {
		return ExprYtimesZ(Y, Z);
	}

	/*
	 * Y * C
	 */

	class ExprYtimesC : public Expression {
		private:
			IntVar Y;
			Int C;
		public:
			ExprYtimesC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual IntVar post();
	};

	inline ExprYtimesC operator* (IntVar Y, Int C) {
		return ExprYtimesC(Y, C);
	}
	inline ExprYtimesC operator* (Int C, IntVar Y) {
		return Y*C;
	}


	// TODO? 
	// IntVar operator- (Int C, IntVar Y);

	/*
	 * Y / Z
	 */

	class ExprYdivZ : public Expression {
		private:
			IntVar Y, Z;
		public:
			ExprYdivZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprYdivZ operator/ (IntVar Y, IntVar Z) {
		return ExprYdivZ(Y, Z);
	}

	/*
	 * C / Z
	 */

	class ExprCdivZ : public Expression {
		private:
			Int C;
			IntVar Z;
		public:
			ExprCdivZ(Int C_, IntVar Z_) : C(C_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprCdivZ operator/ (Int C, IntVar Z) {
		return ExprCdivZ(C, Z);
	}

	/*
	 * Y % Z
	 */

	class ExprYmodZ : public Expression {
		private:
			IntVar Y, Z;
		public:
			ExprYmodZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprYmodZ operator% (IntVar Y, IntVar Z) {
		return ExprYmodZ(Y, Z);
	}

	/*
	 * Y % C
	 */

	class ExprYmodC : public Expression {
		private:
			IntVar Y;
			Int C;
		public:
			ExprYmodC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual IntVar post();
	};

	inline ExprYmodC operator% (IntVar Y, Int C) {
		return ExprYmodC(Y, C);
	}

	/*
	 * C % Z
	 */

	class ExprCmodZ : public Expression {
		private:
			Int C;
			IntVar Z;
		public:
			ExprCmodZ(Int C_, IntVar Z_) : C(C_), Z(Z_) {}
			virtual IntVar post();
	};

	inline ExprCmodZ operator% (Int C, IntVar Z) {
		return ExprCmodZ(C, Z);
	}

	/*
	 * |Y|
	 */

	class ExprAbsY : public Expression {
		private:
			IntVar Y;
		public:
			ExprAbsY(IntVar Y_) : Y(Y_) {}
			virtual IntVar post();
	};

	/*
	 * min
	 */

	class ExprMin : public Expression {
		private:
			IntVarArray Arr;
		public:
			ExprMin(IntVarArray Arr_) : Arr(Arr_) {}
			virtual IntVar post();
	};

	inline ExprMin Minimum(IntVarArray Arr) {
		return ExprMin(Arr);
	}

	/*
	 * max
	 */

	class ExprMax : public Expression {
		private:
			IntVarArray Arr;
		public:
			ExprMax(IntVarArray Arr_) : Arr(Arr_) {}
			virtual IntVar post();
	};

	inline ExprMax Maximum(IntVarArray Arr) {
		return ExprMax(Arr);
	}

	/*
	 * sum
	 */

	class ExprSum : public Expression {
		private:
			IntVarArray Arr;
			UInt start, length;
		public:
			ExprSum(IntVarArray Arr_) : Arr(Arr_), start(0), length(Arr_.size()) {}
			ExprSum(IntVarArray Arr_, UInt start_, UInt length_) : Arr(Arr_), start(start_), length(length_) {
				if(start+length <= Arr.size())
					FEATHER_THROW("start+length exceeds bounds of array");
			}
			virtual IntVar post();
	};

	inline ExprSum Sum(IntVarArray Arr) {
		return ExprSum(Arr);
	}

	inline ExprSum Sum(IntVarArray Arr, UInt start, UInt length) {
		return ExprSum(Arr, start, length);
	}

	/*
	 * Ns_Expression subcategory describing constraints.
     * The following abstract class represents the expressions 
     * category that can be viewed both as a constraint (e.g. X < Y) 
     * and as an expression/meta-constraint (e.g. Z == X < Y) 
     */

    class ExprConstr : public Expression {
    	protected:
    		/*
    		 * Positive or negative constraint declaration
    		 * If isPositive == false then the semantics of
    		 * the constraint-expression is inverted.
    		 * e.g. X < Y becomes X >= Y
    		 */

    		bool isPositive;
    	public:
    		ExprConstr() : isPositive(true) {}
    		void flip() { isPositive = !isPositive; }
    		virtual IntVar postC() = 0;
    		virtual Constraint* postConstraint() = 0;
    		virtual IntVar post() {
    			return postC();
    		}

    		ExprConstr& operator!() {
    			flip();
    			return *this;
    		}
    };

    /*
     * !(ExprConstr)
     */

	// inline ExprConstrYlessthanC operator!(ExprConstrYlessthanC expr) {
 //    	expr.flip();
 //    	return expr;
 //    }



    // class ExprConstrNegation : public ExprConstr {
    // 	private:
    // 		ExprConstr& expr;
    // 	public:
    // 		ExprConstrNegation(ExprConstr& expr_) : ExprConstr(true), expr(expr_) {}
    // 		virtual IntVar postC(bool positively = true) {
    // 			return expr.postC(!positively);
    // 		}
    // 		virtual Constraint* postConstraint(bool positively = true) {
    // 			return expr.postConstraint(!positively);
    // 		}
    // };

    // inline ExprConstrNegation operator!(Expression& expr) {
    // 	return ExprConstrNegation()
    // }


	/*
	 * Y < C
	 */

	class ExprConstrYlessthanC : public ExprConstr {
		private:
			IntVar Y;
			Int C;
		public:
			ExprConstrYlessthanC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual Constraint* postConstraint();
			virtual IntVar postC();
	};

	inline ExprConstrYlessthanC operator<(IntVar Y, Int C) {
		return ExprConstrYlessthanC(Y, C);
	}

	inline ExprConstrYlessthanC operator>(Int C, IntVar Y) {
		return Y<C;
	}

	/*
	 * Y <= C
	 */

	class ExprConstrYlessthaneqC : public ExprConstr {
		private:
			IntVar Y;
			Int C;
		public:
			ExprConstrYlessthaneqC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual Constraint* postConstraint();
			virtual IntVar postC();
	};

	inline ExprConstrYlessthaneqC operator<=(IntVar Y, Int C) {
		return ExprConstrYlessthaneqC(Y, C);
	}

	inline ExprConstrYlessthaneqC operator>=(Int C, IntVar Y) {
		return Y<=C;
	}

	/*
	 * Y == C
	 */

	class ExprConstrYeqC : public ExprConstr {
		private:
			IntVar Y;
			Int C;
		public:
			ExprConstrYeqC(IntVar Y_, Int C_) : Y(Y_), C(C_) {}
			virtual Constraint* postConstraint();
			virtual IntVar postC();
	};

	inline ExprConstrYeqC operator==(IntVar Y, Int C) {
		return ExprConstrYeqC(Y, C);
	}

	/*
	 * Y < Z
	 */

	class ExprConstrYlessthanZ : public ExprConstr {
		private:
			IntVar Y, Z;
		public:
			ExprConstrYlessthanZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual Constraint* postConstraint();
			virtual IntVar postC();
	};

	/*
	 * Y <= Z
	 */

	class ExprConstrYlessthaneqZ : public ExprConstr {
		private:
			IntVar Y, Z;
		public:
			ExprConstrYlessthaneqZ(IntVar Y_, IntVar Z_) : Y(Y_), Z(Z_) {}
			virtual Constraint* postConstraint();
			virtual IntVar postC();
	};

	/*
	 * Y == Z
	 */

	



}


#endif
