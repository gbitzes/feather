#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace std;
using namespace feather;
			
int autocorellation(int N) {
	Solver slv(new Naxos());
	IntVarArray s(slv), c(slv), c2(slv);

	// vector<IntVarArray> partial;
	// for(int i = 0; i < N; i++)
	// 	partial.push_back(IntVarArray(slv));

	IntVarArray* partial[N];
	for(int i = 0; i < N; i++)
		partial[i] = new IntVarArray(slv);
	// vector<IntVarArray> partial(N, slv);

	for(int i = 0; i < N; i++) {
		s.push_back( IntVar(slv, -1, 1) );
		s[s.size()-1].remove(0);
	}

	for(int k = 1; k < N; k++) {
		for(int i = 0; i+k < N; i++) 
			partial[k]->push_back( s[i] * s[ (i+k)%N ] );

		c.push_back( Sum(*partial[k]) );
		c2.push_back( c[c.size()-1] * c[c.size()-1] );
	}

	IntVar objective = Sum(c2);
	slv.minimize(objective);
	slv.addGoal(Labeling(s));

	while( slv.nextSolution() ) {
		cout << "Found solution with cost = " << objective.value() << ": ";

		for(int i = 0; i < N; i++)
			cout << s[i].value() << " ";

		cout << endl;
	}
}

TEST(Examples, autocorrelation) {
	autocorellation(20);
}
