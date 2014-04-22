#include <gtest/gtest.h>
#include <feather/frontend/cpp/solver.h>
#include <feather/frontend/cpp/expressions.h>
#include <feather/frontend/cpp/int-var-array.h>
#include <backend/naxos/naxos.h>
#include <feather/feather.h>

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

TEST(frontend_Solver, IntDeque) {
	Solver slv(new Naxos());
	IntDeque deque(slv);

	deque.push_back(5);
	deque.push_back(3);
	deque.push_back(1);
	deque.push_back(4);

	ASSERT_EQ(deque[0], 5);
	ASSERT_EQ(deque[1], 3);
	ASSERT_EQ(deque[2], 1);
	ASSERT_EQ(deque[3], 4);

	deque[2] = 50;
	ASSERT_EQ(deque[2], 50);

	ASSERT_FALSE(deque.empty());
	ASSERT_EQ(deque.size(), 4);

	IntDeque::iterator it = deque.begin();
	ASSERT_EQ(*it, 5);
	it++;
	ASSERT_EQ(*it, 3);
	it++;
	ASSERT_EQ(*it, 50);
	it++;
	ASSERT_EQ(*it, 4);
	it++;
	ASSERT_TRUE(it == deque.end());
}
