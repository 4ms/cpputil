#include "doctest.h"
#include "util/seq_map.hh"
#include "util/static_string.hh"
/*
TEST_CASE("Basic usage with static_string") {
	StaticString<3> key1 = {"abc"};
	StaticString<3> key2 = {"def"};

	SeqMap<StaticString<3>, uint32_t, 16> map;

	map.insert(key1, 11);
	map.insert(key2, 22);

	CHECK(map.keys[0] == key1);
	CHECK(map.keys[1] == key2);

	auto *val1 = map.get(key1);
	auto *val2 = map.get(key2);

	CHECK(*val1 == 11);
	CHECK(*val2 == 22);
}
*/
