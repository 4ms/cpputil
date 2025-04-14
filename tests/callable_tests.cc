#include "doctest.h"
#include "util/callable.hh"

TEST_CASE("Callable Function") {
	auto capture{0};
	auto func = Function<int(int, int)>{[&capture](int a, int b) {
		capture += 10;
		return a + b + capture;
	}};

	CHECK(func(10, 10) == 30);
	CHECK(capture == 10);
	CHECK(func(10, 10) == 40);
	CHECK(capture == 20);
}

TEST_CASE("Callable Function mixed params") {
	auto capture{0};
	auto func = Function<int(int, float)>{[&capture](int a, float b) {
		capture += 10;
		return a + b + capture;
	}};

	CHECK(func(10, 10.f) == 30);
	CHECK(capture == 10);
	CHECK(func(10, 10.f) == 40);
	CHECK(capture == 20);
}

TEST_CASE("Callable Function lots of params") {
	struct A {
		int a;
		char b;
		float d;
		double e;
	};
	auto capture{0};
	auto func = Function<int(int, float, char, A, A)>{[&capture](int a, float b, char x, A aa, A bb) {
		capture += 10;
		if (x == 'a')
			return aa.a + bb.b + capture * 2.f;
		else
			return a + b + capture;
	}};

	CHECK(func(10, 10.f, 'x', {}, {2, 'b', 1.2f, 1.4}) == 30);
	CHECK(capture == 10);
	CHECK(func(10, 10.f, 'a', {}, {2, 'a', 1.2f, 1.4}) == 137);
	CHECK(capture == 20);
}
