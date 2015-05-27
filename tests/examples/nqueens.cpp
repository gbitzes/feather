#include <gtest/gtest.h>
#include <feather/feather.h>
#include <iostream>

using namespace std;
using namespace feather;

int nqueens(int N) {
    char *cnthreads = getenv("NTHREADS");
    if(cnthreads == NULL) FEATHER_THROW("NTHREADS not set");
    int nthreads = atoi(cnthreads);
	Solver slv( new ThreadManager(new NaxosGenerator(), nthreads, 1000) );
    // Solver slv(new Naxos());
	
	IntVarArray var(slv), varPlus(slv), varMinus(slv);	

	for (int i=0;  i<N;  ++i)   {
		var.push_back( IntVar(slv, 0, N-1) );
		varPlus.push_back(var[i] + i);
		varMinus.push_back(var[i] - i);
	}

	slv.add(AllDiff(var));
	slv.add(AllDiff(varPlus));
	slv.add(AllDiff(varMinus));

	// slv.addGoal(ParallelLabeling(var, 1, 1));
	slv.addGoal(Labeling(var));

	Int nsolutions = 0;
	while(slv.nextSolution() ) {
	  //for(int i = 0; i < N; i++)
	  //  std::cout << var[i].value() << " ";
	  //std::cout << std::endl;
		nsolutions++;
	}
	return nsolutions;
}

TEST(Examples, nqueens4) {
	ASSERT_EQ(nqueens(4), 2);
}

TEST(Examples, nqueens5) {
	ASSERT_EQ(nqueens(5), 10);
}


TEST(Examples, nqueens6) {
	ASSERT_EQ(nqueens(6), 4);
}

TEST(Examples, nqueens7) {
	ASSERT_EQ(nqueens(7), 40);
}

TEST(Examples, nqueens8) {
	ASSERT_EQ(nqueens(8), 92);
}

TEST(Examples, nqueens9) {
	ASSERT_EQ(nqueens(9), 352);
}

TEST(Examples, nqueens10) {
	ASSERT_EQ(nqueens(10), 724);
}

TEST(Examples, nqueens11) {
	ASSERT_EQ(nqueens(11), 2680);
}

TEST(Examples, nqueens12) {
	ASSERT_EQ(nqueens(12), 14200);
}

TEST(Examples, nqueens13) {
	ASSERT_EQ(nqueens(13), 73712);
}

TEST(Examples, nqueens14) {
	ASSERT_EQ(nqueens(14), 365596);
}

TEST(Examples, nqueens15) {
    ASSERT_EQ(nqueens(15), 2279184);
}

TEST(Examples, nqueens16) {
    ASSERT_EQ(nqueens(16), 14772512);
}
