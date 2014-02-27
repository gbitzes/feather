#include <gtest/gtest.h>
#include <base/int-ranges.h>
#include <iostream>

using namespace feather;

template<class T>
IntDomain *intDomainFactory(Int start, Int end) {
	return new T(start, end);
}

template <class T>
class base_IntDomain : public testing::Test {
	protected:
		base_IntDomain() {
			this->factory = &intDomainFactory<T>;
		}
		
		virtual ~base_IntDomain() {}
		IntDomain* (*factory)(Int,Int);
};

using testing::Types;
typedef Types<IntRanges> Implementations;
TYPED_TEST_CASE(base_IntDomain, Implementations);

TYPED_TEST(base_IntDomain, Construction) {
	ASSERT_THROW( TypeParam(5, 1), FeatherException );
}

TYPED_TEST(base_IntDomain, Stress1) {
	IntDomain *domain = this->factory(0, 63);
	
	domain->removeRange(5, 6);
	domain->removeRange(16, 17);
	domain->removeRange(20, 20);
	domain->removeRange(26, 26);

	ASSERT_EQ( 1, domain->next(0));
	ASSERT_EQ( 3, domain->next(2));
	ASSERT_EQ( 7, domain->next(4));
	ASSERT_EQ( 32, domain->next(31));

	domain->removeRange(28, 35);
	ASSERT_EQ( 36, domain->next(30));
	ASSERT_EQ( 62, domain->next(61));
	ASSERT_EQ( kPlusInf, domain->next(63));

	delete domain;
}

TYPED_TEST(base_IntDomain, Stress2) {
	IntDomain *domain = this->factory(0, 100000);
	domain->removeRange(20, 50);

	int i;
	for(i = 51; i <= 90000; i++)
		ASSERT_EQ( i+1, domain->next(i) );

	ASSERT_EQ(19, domain->previous(40) );

	domain->removeRange(1000, 2000);
	ASSERT_EQ(1000, domain->nextGap(90) );
	ASSERT_EQ(1002, domain->nextGap(1001) );

	delete domain;
}

TYPED_TEST(base_IntDomain, Stress3) {
	IntDomain *domain = this->factory(0, 10);

	domain->removeRange(5, 10);
	ASSERT_EQ(kPlusInf, domain->next(4) );
	ASSERT_EQ(4, domain->previous(5));
	delete domain;
}

TYPED_TEST(base_IntDomain, Stress4) {
	IntDomain *domain = this->factory(100, 200);
	domain->removeRange(150, 160);
	ASSERT_EQ(161, domain->next(149));
	delete domain;
}
	
TYPED_TEST(base_IntDomain, Stress5) {
	IntDomain *domain = this->factory(10, 20);

	domain->removeRange(11, 14);
	domain->removeRange(16, 18);

	for(int i = 8; i <= 24; i++) {

		if( i == 10 || i == 15 || i == 19 || i == 20)
			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));
	}
	delete domain;
}


TYPED_TEST(base_IntDomain, Stress6) {
	IntDomain *domain = this->factory(1, 60000);

	domain->removeRange(200, 400);
	domain->removeRange(200, 400);
	domain->removeRange(402, 405);

	ASSERT_EQ( 401, domain->next(250) );
	ASSERT_EQ( 401, domain->next(200) );
	ASSERT_EQ( 401, domain->next(199) );
	ASSERT_EQ( 199, domain->previous(200) );
	ASSERT_EQ( 199, domain->previous(201) );


	domain->removeRange(64, 64);
	ASSERT_EQ( 65, domain->next(63) );
	delete domain;
}

