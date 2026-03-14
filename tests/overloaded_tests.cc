#include "doctest.h"
#include "util/overloaded.hh"
#include <variant>
#include <string>

TEST_CASE("overloaded: dispatches on int") {
	auto visitor = overloaded{
		[](int x) { return x * 2; },
		[](double x) { return (int)(x + 0.5); },
	};
	CHECK(visitor(3) == 6);
}

TEST_CASE("overloaded: dispatches on double") {
	auto visitor = overloaded{
		[](int x) { return x * 2; },
		[](double x) { return (int)(x + 0.5); },
	};
	CHECK(visitor(2.7) == 3);
}

TEST_CASE("overloaded: used with std::variant") {
	std::variant<int, std::string> v = 42;

	int result = std::visit(overloaded{
								[](int i) { return i; },
								[](const std::string &) { return -1; },
							},
							v);
	CHECK(result == 42);

	v = std::string{"hello"};
	result = std::visit(overloaded{
							[](int) { return -1; },
							[](const std::string &s) { return (int)s.size(); },
						},
						v);
	CHECK(result == 5);
}

TEST_CASE("overloaded: three-way overload") {
	auto visitor = overloaded{
		[](int) { return 1; },
		[](float) { return 2; },
		[](double) { return 3; },
	};
	CHECK(visitor(0) == 1);
	CHECK(visitor(0.0f) == 2);
	CHECK(visitor(0.0) == 3);
}
