#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace feather;
using namespace std;

TEST(ThreadManager, T1) {
	int N = 14;

	ThreadManager *tm = new ThreadManager(new NaxosGenerator(), 3, 1000);
	Solver slv(tm);

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
	std::cout << "solutions: " << nsolutions << std::endl;
}
