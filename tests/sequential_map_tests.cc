#include "doctest.h"
#include "util/sequential_map.hh"

TEST_CASE("Basic use") {

	SequentialMap<int, float, 4> sm;

	// Pre-conditions:
	CHECK(sm.size() == 0);
	CHECK(sm.is_full() == false);

	// Add a value
	sm[2] = 3.f;
	CHECK(sm.size() == 1);
	CHECK(sm.is_full() == false);
	CHECK(sm.contains(2));
	CHECK(sm[2] == 3.f);

	sm[9] = 99.f;
	CHECK(sm.size() == 2);
	CHECK(sm.is_full() == false);
	CHECK(sm.contains(9));
	CHECK(sm[9] == 99.f);

	// Change a value
	sm[2] = 5.f;
	CHECK(sm.size() == 2);
	CHECK(sm.is_full() == false);
	CHECK(sm.contains(2));
	CHECK(sm[2] == 5.f);

	// Fill it up
	sm[20] = 20.20f;
	CHECK(sm.size() == 3);
	CHECK(sm.is_full() == false);

	sm[30] = 30.30f;
	CHECK(sm.size() == 4);
	CHECK(sm.is_full() == true);

	// Overflow is handled by overwriting the last value added
	CHECK(sm.contains(30));
	sm[99] = 10.010f;
	CHECK_FALSE(sm.contains(30));
	CHECK(sm.contains(99));
}

TEST_CASE("Erase") {
	SequentialMap<char, unsigned, 4> sm;

	sm['a'] = 1;
	sm['b'] = 10;
	sm['c'] = 100;
	sm['d'] = 1000;

	auto it_b = sm.find('b');

	SUBCASE("Erase first") {
		CHECK(sm.erase('a') == 1);
		CHECK(sm.size() == 3);
		CHECK(sm.contains('a') == false);
		CHECK(sm.contains('b') == true);
		CHECK(sm.contains('c') == true);
		CHECK(sm.contains('d') == true);

		// Iterator is now invalid!
		// This breaks with std::map
		CHECK_FALSE(it_b->first == 'b');
		CHECK_FALSE(it_b->second == 10);
	}

	SUBCASE("Erase middle") {
		CHECK(sm.erase('b') == 1);
		CHECK(sm.size() == 3);
		CHECK(sm.contains('a') == true);
		CHECK(sm.contains('b') == false);
		CHECK(sm.contains('c') == true);
		CHECK(sm.contains('d') == true);
	}

	SUBCASE("Erase last") {
		CHECK(sm.erase('d') == 1);
		CHECK(sm.size() == 3);
		CHECK(sm.contains('a') == true);
		CHECK(sm.contains('b') == true);
		CHECK(sm.contains('c') == true);
		CHECK(sm.contains('d') == false);

		// Iterator is still valid
		CHECK(it_b->first == 'b');
		CHECK(it_b->second == 10);
	}

	// Can still add a new one
	sm['e'] = 10000;
	CHECK(sm.contains('e'));
	CHECK(sm['e'] == 10000);
}
