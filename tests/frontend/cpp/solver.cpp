#include <gtest/gtest.h>
#include <feather/frontend/cpp/solver.h>
#include <feather/frontend/cpp/expressions.h>
#include <feather/frontend/cpp/int-var-array.h>
#include <backend/naxos/naxos.h>

using namespace feather;
using namespace std;

TEST(frontend_Solver, Basic) {
	Solver slv( new Naxos() );
	IntVar a(slv, 0, 10);
	IntVar b = a - 3;
	IntVar c = a + 3;

	IntVarArray arr(slv);
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);

	IntVarArray::iterator iter = arr.begin();
	ASSERT_TRUE( (*iter).equals(a) );
	++iter;
	ASSERT_TRUE( (*iter).equals(b) );
	++iter;
	ASSERT_TRUE( (*iter).equals(c) );
	++iter;
	ASSERT_THROW(*iter, FeatherException);

	ASSERT_TRUE( arr[0].equals(a) );
	ASSERT_TRUE( arr[1].equals(b) );
	ASSERT_TRUE( arr[2].equals(c) );

	ASSERT_EQ( arr.size(), 3);
	ASSERT_FALSE( arr.empty() );

	slv.addGoal(Labeling(arr));

	Int nsolutions = 0;
	while( slv.nextSolution() ) {
		nsolutions++;
		std::cout << "solution" << std::endl;
		std::cout << a.value() << std::endl; 
		std::cout << b.value() << std::endl; 
		std::cout << c.value() << std::endl;
	}
}

TEST(frontend_Solver, Basic2) {
	Solver slv( new Naxos() );
	IntVar a(slv, 0, 30);
	IntVar b = a + 3;
	IntVar c = a*2 + b;

	slv.add( a > 10 );
	slv.add( b <= 20 );
	slv.add( c != 48 );

	IntVarArray arr(slv);
	arr.push_back(a);
	arr.push_back(b);
	arr.push_back(c);

	slv.addGoal(Labeling(arr));

	Int nsolutions = 0;
	while( slv.nextSolution() ) {
		nsolutions++;
		ASSERT_TRUE(a.value() > 10);
		ASSERT_TRUE(b.value() <= 20);
		ASSERT_TRUE(c.value() != 48);
	}
	ASSERT_EQ(nsolutions, 6);
}

int nqueens(int N) {
	Solver slv( new Naxos() );
	
	IntVarArray var(slv), varPlus(slv), varMinus(slv);	

	for (int i=0;  i<N;  ++i)   {
		var.push_back( IntVar(slv, 0, N-1) );
		varPlus.push_back(var[i] + i);
		varMinus.push_back(var[i] - i);
	}

	slv.add(AllDiff(var));
	slv.add(AllDiff(varPlus));
	slv.add(AllDiff(varMinus));

	slv.addGoal(Labeling(var));

	Int nsolutions = 0;
	while(slv.nextSolution() ) {
		nsolutions++;
	}
	return nsolutions;
}

TEST(frontend_Solver, nqueens4) {
	std::cout << nqueens(4) << std::endl;
}

TEST(frontend_Solver, nqueens7) {
	std::cout << nqueens(7) << std::endl;
}

TEST(frontend_Solver, nqueens8) {
	std::cout << nqueens(8) << std::endl;
}

TEST(frontend_Solver, nqueens11) {
	std::cout << nqueens(11) << std::endl;
}

TEST(frontend_Solver, nqueens12) {
	std::cout << nqueens(12) << std::endl;
}
