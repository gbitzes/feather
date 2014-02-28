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

// sentinel

TYPED_TEST(base_IntDomain, Stress7) {
	IntDomain *domain = this->factory(-1, 4);

	int i;
	for(i = -1; i <= 4; i++)
		ASSERT_TRUE(domain->contains(i));

	domain->removeRange(0, 0);

	for(i = -1; i <= 4; i++) {
		if(i != 0)
			ASSERT_TRUE(domain->contains(i));
		else	
			ASSERT_FALSE(domain->contains(i));
	}

	domain->removeRange(2, 3);

	for(i = -1; i <= 4; i++) {
		if(i != 0 && i != 2 && i != 3)
			ASSERT_TRUE(domain->contains(i));
		else	
			ASSERT_FALSE(domain->contains(i));
	}

	domain->removeRange(1, 4);

	for(i = -1; i <= 4; i++) {
		if(i == -1 )
			ASSERT_TRUE(domain->contains(i));
		else	
			ASSERT_FALSE(domain->contains(i));
	}

}

TYPED_TEST(base_IntDomain, Stress8) {
	IntDomain *domain = this->factory(-5, -2);
	
	int i;
	for(i = -5; i <= -2; i++)
		ASSERT_TRUE(domain->contains(i));

	domain->removeRange(-4, -4);

	for(i = -5; i <= -2; i++) {
		if(i != -4)
			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));
	}

	domain->removeRange(-5, -3);

	for(i = -5; i <= -2; i++) {
		if(i == -2)
			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));
	}

}

TYPED_TEST(base_IntDomain, Stress9) {
	IntDomain *domain = this->factory(7, 11);

	int i;
	for(i = 7; i <= 11; i++)
		ASSERT_TRUE(domain->contains(i));

	domain->removeRange(8, 8);
	
	for(i = 7; i <= 11; i++) {
		if( i == 8 )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));
	}

	domain->removeRange(7, 10);

	for(i = 7; i <= 11; i++) {
		if( i == 11 )
			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));
	}

}

TYPED_TEST(base_IntDomain, Stress10) {
	IntDomain *domain = this->factory(0, 100);

	domain->save();
	domain->removeRange(9, 10);

	int i;
	for(i = 0; i <= 100; i++) {

		if( i == 9 || i == 10 )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));
	}

	domain->save();
	domain->removeRange(90, 91);

	for(i = 0; i <= 100; i++) {
		if( i == 9 || i == 10 || i == 90 || i == 91 )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));
	}

	domain->save();
	domain->removeRange(8, 92);

	for(i = 0; i <= 100; i++) {
		if( (0  <= i && i <= 7)  ||
			(93 <= i && i <= 100) )
			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));
	}

	domain->restore();

	for(i = 0; i <= 100; i++) {
		if( i == 9 || i == 10 || i == 90 || i == 91 )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));
	}

	domain->restore();

	for(i = 0; i <= 100; i++) {
		if( i == 9 || i == 10 )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));
	}

	domain->restore();

	for(i = 0; i <= 100; i++)
		ASSERT_TRUE(domain->contains(i));

}

TYPED_TEST(base_IntDomain, Stress11) {
	IntDomain *domain = this->factory(3, 3);
	ASSERT_EQ(3, domain->next(1));
	ASSERT_EQ(3, domain->previous(5));
}

TYPED_TEST(base_IntDomain, Stress12) {
	IntDomain *domain = this->factory(0, 100);

	domain->save();
	domain->removeRange(50, 60);
	domain->save();
	domain->removeRange(0, 0);

	int i;
	for(i = 0; i <= 100; i++) {
		if( (1  <= i && i <= 49)  ||
			(61 <= i && i <= 100) )
			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));
	}

	domain->restore();

	for(i = 0; i <= 100; i++) {
		if( (0  <= i && i <= 49)  ||
			(61 <= i && i <= 100) )
			ASSERT_TRUE(domain->contains(i)) << i;
		else
			ASSERT_FALSE(domain->contains(i)) << i;
	}

	domain->restore();

	for(i = 0; i <= 100; i++) 
		ASSERT_TRUE(domain->contains(i));

	// exit(0);

}

TYPED_TEST(base_IntDomain, Stress13) {
	IntDomain *domain = this->factory(0, 100);

	domain->removeRange(10, 20);
	domain->removeRange(20, 30);
	domain->removeRange(90, 100);

	int i;
	for(i = 0; i <= 100; i++) {
		if( (10  <= i && i <= 30)  ||
			(90 <= i && i <= 100) )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));

	}
}

TYPED_TEST(base_IntDomain, Stress14) {
	IntDomain *domain = this->factory(0, 98);
	ASSERT_EQ(99, domain->size() );

	domain->removeRange(-10, -5);
	ASSERT_EQ(99, domain->size());

	domain->removeRange(15, 25);
	ASSERT_EQ(88, domain->size());

	domain->removeRange(40, 55);
	ASSERT_EQ(72, domain->size());

	domain->removeRange(28, 28);
	ASSERT_EQ(71, domain->size());

	/*
	 *  The bitset should now have the following form: 
	 *  [0..14 26..27 29..39 56..98]
	 */

	int i;
	for(i = -5; i <= 105; i++) {
		if( (0  <= i && i <= 14)  ||
			(26 <= i && i <= 27)  ||
			(29 <= i && i <= 39)  ||
			(56 <= i && i <= 98)  )

			ASSERT_TRUE(domain->contains(i));
		else
			ASSERT_FALSE(domain->contains(i));

	}

	ASSERT_EQ(57, domain->previous(58));
	ASSERT_EQ(39, domain->previous(45));
	ASSERT_EQ(14, domain->previous(20));
	ASSERT_EQ(kMinusInf, domain->previous(-1));
	ASSERT_EQ(kMinusInf, domain->previous(0));
	ASSERT_EQ(14, domain->previous(26));

	ASSERT_EQ(0, domain->min());
	ASSERT_EQ(98, domain->max());

	ASSERT_EQ(31, domain->next(30));
	ASSERT_EQ(26, domain->next(24));
	ASSERT_EQ(56, domain->next(39));
	ASSERT_EQ(30, domain->next(29));

	ASSERT_EQ(15, domain->nextGap(13));
	ASSERT_EQ(17, domain->nextGap(16));
	ASSERT_EQ(15, domain->nextGap(-1));
	ASSERT_EQ(15, domain->nextGap(-2));
	ASSERT_EQ(kPlusInf, domain->nextGap(97));
	ASSERT_EQ(kPlusInf, domain->nextGap(98));
	ASSERT_EQ(28, domain->nextGap(25));
	ASSERT_EQ(45, domain->nextGap(44));

	ASSERT_TRUE(domain->containsRange(29, 39));
	ASSERT_FALSE(domain->containsRange(26, 28));
	ASSERT_TRUE(domain->containsRange(0, 0));
	ASSERT_TRUE(domain->containsRange(90, 98));
	ASSERT_FALSE(domain->containsRange(10, 30));
}

TYPED_TEST(base_IntDomain, Stress15) {
	IntDomain *domain = this->factory(0, 100);

	domain->save();

	domain->removeRange(51, 52);

	int i;
	for(i = 0; i <= 100; i++) {
		if( i == 51 || i == 52 )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));
	}

	domain->restore();

	for(i = 0; i <= 100; i++)
		ASSERT_TRUE(domain->contains(i));

	domain->save();

	domain->removeRange(10, 20);
	domain->save();

	domain->removeRange(20, 30);
	domain->save();

	domain->removeRange(90, 100);

	for(i = 0; i <= 100; i++) {
		if( (10  <= i && i <= 30)  ||
			(90 <= i && i <= 100) )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));

	}

	domain->restore();

	for(i = 0; i <= 100; i++) {
		if( (10  <= i && i <= 30) )
			ASSERT_FALSE(domain->contains(i)) << i;
		else
			ASSERT_TRUE(domain->contains(i)) << i;

	}
	
	domain->restore();

	for(i = 0; i <= 100; i++) {
		if( (10  <= i && i <= 20) )
			ASSERT_FALSE(domain->contains(i));
		else
			ASSERT_TRUE(domain->contains(i));

	}

	domain->restore();

	for(i = 0; i <= 100; i++)
		ASSERT_TRUE(domain->contains(i));

}
