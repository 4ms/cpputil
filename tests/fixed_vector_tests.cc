#include "doctest.h"
#include "util/fixed_vector.hh"

TEST_CASE("Basic usage: operator[] reads") {
	FixedVector<int, 4> a{1, 2, 3, 4};
	CHECK(a[0] == 1);
	CHECK(a[1] == 2);
	CHECK(a[2] == 3);
	CHECK(a[3] == 4);
}

TEST_CASE("Basic usage: operator[] writes") {
	FixedVector<int, 4> a{1, 2, 3, 4};
	a[0] = 5;
	CHECK(a[0] == 5);
	a[1] = 6;
	CHECK(a[1] == 6);
	a[2] = 7;
	CHECK(a[2] == 7);
	a[3] = 8;
	CHECK(a[3] == 8);

	SUBCASE("Can write to any element, even past size()") {
		FixedVector<int, 4> b;
		b[3] = 9;
		CHECK(b.size() == 0);
		CHECK(b[3] == 9);
	}
}

TEST_CASE("Two element construction makes two elements (not an element repeated N times)") {
	FixedVector<int, 7> a{4, 7};
	CHECK(a[1] == 7);
}

TEST_CASE("Push back creates elements in the right order") {

	FixedVector<unsigned, 5> a;
	CHECK(a.size() == 0);

	a.push_back(3);
	CHECK(a[0] == 3);
	CHECK(a.size() == 1);

	a.push_back(4);
	CHECK(a[1] == 4);
	CHECK(a.size() == 2);

	a.push_back(5);
	CHECK(a[2] == 5);
	CHECK(a.size() == 3);

	SUBCASE("Pop back does the opposite of push back") {
		auto x = a.pop_back();
		CHECK(x == 5);
		CHECK(a.size() == 2);

		x = a.pop_back();
		CHECK(x == 4);
		CHECK(a.size() == 1);

		x = a.pop_back();
		CHECK(x == 3);
		CHECK(a.size() == 0);

		SUBCASE("Popping an empty vector returns default-constructed type") {
			x = a.pop_back();
			CHECK(x == 0);

			x = a.pop_back();
			CHECK(x == 0);
		}
	}
}

//TODO: Test with custom types and count number of ctors/dtors in custom type
