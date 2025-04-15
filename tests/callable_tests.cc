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

TEST_CASE("FunctionSized") {
	// This fails to compile (which is what we want)
	// auto capture{0};
	// auto capture2{0};
	// auto capture3{0};
	// auto func = Function<int(int, int)>{[&](int a, int b) {
	// 	capture += 10;
	// 	capture2 += 10;
	// 	capture3 += 10;
	// 	return a + b + capture;
	// }};

	auto capture{0};
	auto capture2{0};
	auto capture3{0};
	auto func = FunctionSized<sizeof(void *) * 3, int(int, int)>{[&](int a, int b) {
		capture += 10;
		capture2 += 10;
		capture3 += 10;
		return a + b + capture;
	}};
}

TEST_CASE("Callable") {
	auto capture{0};

	auto func = Callback{[&capture] {
		capture += 10;
		return capture;
	}};

	func();
	CHECK(capture == 10);
	func();
	CHECK(capture == 20);
}

TEST_CASE("CallableSized") {
	auto capture{0};
	auto capture2{0};
	auto capture3{0};

	auto func = CallbackSized<sizeof(void *) * 3>{[&] {
		capture += 10;
		capture2 += 10;
		capture3 += 10;
		return capture;
	}};

	func();
	CHECK(capture3 == 10);
	func();
	CHECK(capture3 == 20);
}

TEST_CASE("~Callable") {
	auto capture{0};

	Callback *func = new Callback{[&capture] {
		capture += 10;
		return capture;
	}};

	(*func)();
	CHECK(capture == 10);
	(*func)();
	CHECK(capture == 20);

	delete func;

	Callback *empty = new Callback();
	delete empty;
}

TEST_CASE("operator bool") {
	auto capture{0};

	Callback func;
	CHECK_FALSE((bool)func);

	func = Callback{[&capture] {
		capture += 10;
		return capture;
	}};

	CHECK((bool)func);
}
