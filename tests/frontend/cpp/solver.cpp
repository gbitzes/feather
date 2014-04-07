#include <gtest/gtest.h>
#include <feather/frontend/cpp/solver.h>
#include <feather/frontend/cpp/expressions.h>

using namespace feather;
using namespace std;

TEST(frontend_Solver, Basic) {
	Solver slv;
	IntVar a(slv, 0, 10);
}
