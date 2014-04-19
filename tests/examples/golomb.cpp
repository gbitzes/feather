#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace std;
using namespace feather;

int golomb(int N) {

	Solver slv(new Naxos() );
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

	while( slv.nextSolution() ) {
		cout << "Found solution with length " << ruler[N-1].value() << ": ";

		for(int i = 0; i < N; i++)
			cout << ruler[i].value() << " ";

		cout << endl;
	}
}

TEST(Examples, golomb10) {
	golomb(10);
}
