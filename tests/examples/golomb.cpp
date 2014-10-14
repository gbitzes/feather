#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace std;
using namespace feather;

int golomb(int N) {
    std::vector<SolverAddress*> addresses;

    { SolverAddress *addr = new SolverAddress("localhost", "7575");
    addresses.push_back(addr); }

    { SolverAddress *addr = new SolverAddress("olwork12", "7575");
    addresses.push_back(addr); }

    { SolverAddress *addr = new SolverAddress("olwork16", "7575");
    addresses.push_back(addr); }

    { SolverAddress *addr = new SolverAddress("olwork24", "7575");
    addresses.push_back(addr); }
    
    { SolverAddress *addr = new SolverAddress("olwork06", "7575");
    addresses.push_back(addr); }
    
    { SolverAddress *addr = new SolverAddress("olwork04", "7575");
    addresses.push_back(addr); }


    Solver slv(new SocketClient(addresses));

	//Solver slv(new ThreadManager(new NaxosGenerator(), 2, 1000) );
	IntVarArray ruler(slv), diffs(slv);

	ruler.push_back( IntVar(slv, 0, 0) );

	for(int i = 1; i < N; i++) {
		ruler.push_back( IntVar(slv, 0, 12000));
		slv.add(ruler[i] > ruler[i-1]);
	}

	slv.add(ruler[1]-ruler[0] < ruler[N-1]-ruler[N-2]);

	for(int i = 0; i < N; i++)
		for(int j = i+1; j < N; j++)
			diffs.push_back(ruler[j] - ruler[i]);

	slv.add(AllDiff(diffs));
	slv.minimize(ruler[N-1]);

	slv.addGoal(Labeling(ruler));
    
    int solution = -1;
	while( slv.nextSolution() ) {
		cout << "Found solution with length " << ruler[N-1].value() << ": ";
        solution = ruler[N-1].value();

		for(int i = 0; i < N; i++)
			cout << ruler[i].value() << " ";

		cout << endl;
	}
    return solution;
}

TEST(Examples, golomb10) {
    ASSERT_EQ(golomb(10), 55);
}

TEST(Examples, golomb13) {
    ASSERT_EQ(golomb(13), 106);
}
