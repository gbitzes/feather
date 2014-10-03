#include <gtest/gtest.h>
#include <feather/feather.h>

using namespace std;
using namespace feather;
			
int autocorrelation(int N) {
	Solver slv(new ThreadManager(new NaxosGenerator(), 48, 1000));
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
    
    int best;
	while( slv.nextSolution() ) {
        best = objective.value();
		cout << "Found solution with cost = " << objective.value() << ": ";

		for(int i = 0; i < N; i++)
			cout << s[i].value() << " ";

		cout << endl;
	}
    return best;
}

TEST(Examples, autocorrelation17) {
    ASSERT_EQ(autocorrelation(17), 32);
}

TEST(Examples, autocorrelation18) {
	ASSERT_EQ(autocorrelation(18), 25);
}

TEST(Examples, autocorrelation19) {
	ASSERT_EQ(autocorrelation(19), 29);
}

TEST(Examples, autocorrelation20) {
	ASSERT_EQ(autocorrelation(20), 26);
}

TEST(Examples, autocorrelation21) {
	ASSERT_EQ(autocorrelation(21), 26);
}

TEST(Examples, autocorrelation22) {
	ASSERT_EQ(autocorrelation(22), 39);
}

TEST(Examples, autocorrelation23) {
	ASSERT_EQ(autocorrelation(23), 47);
}

TEST(Examples, autocorrelation24) {
	ASSERT_EQ(autocorrelation(24), 36);
}

TEST(Examples, autocorrelation25) {
	ASSERT_EQ(autocorrelation(25), 36);
}

TEST(Examples, autocorrelation26) {
	ASSERT_EQ(autocorrelation(26), 45);
}

TEST(Examples, autocorrelation27) {
	ASSERT_EQ(autocorrelation(27), 37);
}

