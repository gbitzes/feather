#include <gtest/gtest.h>
#include <feather/frontend/cpp/expressions.h>
#include <backend/naxos/naxos.h>

using namespace feather;
using namespace std;


TEST(frontend_Expressions, T1) {
	Solver slv( new Naxos() );
	IntVar a(slv, 0, 10);

	ASSERT_EQ( a.min(), 0 );
	ASSERT_EQ( a.max(), 10 );

	/*
	 * Y + C 
	 */
	
	IntVar b = a + 5;

	ASSERT_EQ( a.min(), 0 );
	ASSERT_EQ( a.max(), 10 );

	ASSERT_EQ( b.min(), 5 );
	ASSERT_EQ( b.max(), 15 );

	IntVar c = 5 + a;
	IntVar d = a - 5;

	ASSERT_EQ( c.min(), 5 );
	ASSERT_EQ( c.max(), 15 );

	ASSERT_EQ( d.min(), -5 );
	ASSERT_EQ( d.max(), 5 );

	/*
	 * Y / C
	 */
	
	IntVar e = a / 5;

	/*
	 * Y + Z
	 */

	IntVar f = a + b;

	/* 
	 * Y - Z
	 */

	IntVar g = a - b;
	IntVar h = ( (g + a) - (b + c ) ) - e + f - c;

	/*
	 * C - Z
	 */

	IntVar i = 5 - a;
	IntVar j = -a;

	/*
	 * Y * Z
	 */

	IntVar k = a * b;

	/*
	 * Y * C
	 */

	IntVar l = a * 5;
	IntVar m = 5 * a;

	//nopqrstuvxyz


	IntVar n = a < 5;

}

TEST(frontend_Expressions, T2) {
	Solver slv( new Naxos() );
	IntVar a(slv, 0, 10);
	IntVar n = a < 5;
	std::cout << "next one" << std::endl;
	IntVar k = !(a < 5);
}

TEST(frontend_Expressions, T3 ) {
	Solver slv( new Naxos() );
	IntVar a(slv, 0, 100);
	slv.addConstraint(a > 50);
	ASSERT_EQ(a.min(), 51);
	ASSERT_EQ(a.max(), 100);

	slv.addConstraint(a <= 80);
	ASSERT_EQ(a.min(), 51);
	ASSERT_EQ(a.max(), 80);

	IntVar b(slv, 0, 100);
	slv.addConstraint( !(b > 90) );
	ASSERT_EQ(b.min(), 0);
	ASSERT_EQ(b.max(), 90);

	IntVar c(slv, 0, 10);
	slv.addConstraint(c == 5);
	ASSERT_EQ(c.min(), 5);
	ASSERT_EQ(c.max(), 5);
	ASSERT_EQ(c.value(), 5);

	IntVar d(slv, 0, 10);
	slv.addConstraint( !(d == 5) );
	ASSERT_FALSE(d.contains(5));

	ASSERT_THROW(slv.addConstraint(d==5), FeatherException);

}