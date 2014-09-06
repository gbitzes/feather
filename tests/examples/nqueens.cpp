#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace std;
using namespace feather;

int nqueens(int N) {
    std::vector<SolverAddress> addresses;
    { SolverAddress addr("localhost", "7879");
    addresses.push_back(addr); }
    
    { SolverAddress addr("localhost", "7878");
    addresses.push_back(addr); }

    { SolverAddress addr("localhost", "7877");
    addresses.push_back(addr); }

    { SolverAddress addr("localhost", "7876");
    addresses.push_back(addr); }

    Solver slv(new SocketClient(addresses));
	//Solver slv( new ThreadManager(new NaxosGenerator(), 1, 1000) );
    //Solver slv( new SocketHelper(new Naxos(), 5));
	
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
        std::cout << "WE HAVE A SOLUTION, BITCHES" << std::endl;
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
