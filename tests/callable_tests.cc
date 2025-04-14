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
