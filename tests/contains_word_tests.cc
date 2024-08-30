#include "doctest.h"

#include "util/contains_word.hh"

TEST_CASE("basic usage") {
	CHECK(contains_word("In", "in"));
	CHECK(contains_word("Play IN", "in"));
	CHECK_FALSE(contains_word("PlaIN", "in"));
	CHECK(contains_word("iN 1", "in"));
	CHECK_FALSE(contains_word("inner gain", "in"));
	CHECK(contains_word("Ch2 In L", "in"));
	CHECK_FALSE(contains_word("ChaIn L", "in"));
}
