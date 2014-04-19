#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace std;
using namespace feather;

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

TEST(Examples, nqueens4) {
	ASSERT_EQ(nqueens(4), 2);
}

TEST(Examples, nqueens7) {
	ASSERT_EQ(nqueens(7), 40);
}

TEST(Examples, nqueens8) {
	ASSERT_EQ(nqueens(8), 92);
}

TEST(Examples, nqueens11) {
	ASSERT_EQ(nqueens(11), 2680);
}

TEST(Examples, nqueens12) {
	ASSERT_EQ(nqueens(12), 14200);
}
