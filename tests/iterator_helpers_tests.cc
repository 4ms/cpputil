#include "doctest.h"
#include "util/iterator_helpers.hh"
#include <tuple>

TEST_CASE("any_match: matching first element") {
	auto lhs = std::make_tuple(1, 2, 3);
	auto rhs = std::make_tuple(1, 5, 6);
	CHECK(IteratorHelper::any_match(lhs, rhs) == true);
}

TEST_CASE("any_match: matching middle element") {
	auto lhs = std::make_tuple(1, 2, 3);
	auto rhs = std::make_tuple(9, 2, 8);
	CHECK(IteratorHelper::any_match(lhs, rhs) == true);
}

TEST_CASE("any_match: matching last element") {
	auto lhs = std::make_tuple(1, 2, 3);
	auto rhs = std::make_tuple(9, 8, 3);
	CHECK(IteratorHelper::any_match(lhs, rhs) == true);
}

TEST_CASE("any_match: no matching elements") {
	auto lhs = std::make_tuple(1, 2, 3);
	auto rhs = std::make_tuple(4, 5, 6);
	CHECK(IteratorHelper::any_match(lhs, rhs) == false);
}

TEST_CASE("any_match: all matching elements") {
	auto lhs = std::make_tuple(1, 2, 3);
	auto rhs = std::make_tuple(1, 2, 3);
	CHECK(IteratorHelper::any_match(lhs, rhs) == true);
}

TEST_CASE("any_match: single element match") {
	auto lhs = std::make_tuple(42);
	auto rhs = std::make_tuple(42);
	CHECK(IteratorHelper::any_match(lhs, rhs) == true);
}

TEST_CASE("any_match: single element no match") {
	auto lhs = std::make_tuple(42);
	auto rhs = std::make_tuple(0);
	CHECK(IteratorHelper::any_match(lhs, rhs) == false);
}
