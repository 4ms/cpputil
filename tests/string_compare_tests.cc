#include "doctest.h"
#include "util/string_compare.hh"

TEST_CASE("equal_ci: identical strings") {
	CHECK(equal_ci("hello", "hello") == true);
}

TEST_CASE("equal_ci: case insensitive match") {
	CHECK(equal_ci("Hello", "hello") == true);
	CHECK(equal_ci("HELLO", "hello") == true);
	CHECK(equal_ci("hElLo", "HeLlO") == true);
}

TEST_CASE("equal_ci: different strings") {
	CHECK(equal_ci("hello", "world") == false);
	CHECK(equal_ci("abc", "abd") == false);
}

TEST_CASE("equal_ci: different lengths") {
	CHECK(equal_ci("hello", "hell") == false);
	CHECK(equal_ci("hi", "high") == false);
}

TEST_CASE("equal_ci: empty strings") {
	CHECK(equal_ci("", "") == true);
	CHECK(equal_ci("", "a") == false);
	CHECK(equal_ci("a", "") == false);
}

TEST_CASE("less_ci: alphabetical ordering") {
	CHECK(less_ci("apple", "banana") == true);
	CHECK(less_ci("banana", "apple") == false);
	CHECK(less_ci("apple", "apple") == false);
}

TEST_CASE("less_ci: case insensitive ordering") {
	CHECK(less_ci("Apple", "banana") == true);
	CHECK(less_ci("APPLE", "BANANA") == true);
	CHECK(less_ci("banana", "APPLE") == false);
}

TEST_CASE("less_ci: prefix ordering") {
	CHECK(less_ci("abc", "abcd") == true);
	CHECK(less_ci("abcd", "abc") == false);
}

TEST_CASE("less_ci: empty string is less than non-empty") {
	CHECK(less_ci("", "a") == true);
	CHECK(less_ci("a", "") == false);
	CHECK(less_ci("", "") == false);
}
