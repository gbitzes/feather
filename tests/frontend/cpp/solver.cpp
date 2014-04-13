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
	ASSERT_EQ(*iter, a);
	++iter;
	ASSERT_EQ(*iter, b);
	++iter;
	ASSERT_EQ(*iter, c);
	++iter;
	ASSERT_THROW(*iter, FeatherException);

	ASSERT_EQ( arr[0], a );
	ASSERT_EQ( arr[1], b );
	ASSERT_EQ( arr[2], c );

	ASSERT_EQ( arr.size(), 3);
	ASSERT_FALSE( arr.empty() );

	slv.addGoal(Labeling(arr));

	slv.finalize();

	while( slv.nextSolution() ) {
		std::cout << "solution" << std::endl;
		std::cout << a.value() << std::endl; 
		std::cout << b.value() << std::endl; 
		std::cout << c.value() << std::endl;
	}

}
