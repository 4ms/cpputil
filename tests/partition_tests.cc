#include "doctest.h"
#include "util/partition.hh"
#include <array>

TEST_CASE("Basic usage") {

	Partition<2, 8> parts;

	auto nums = std::array{1u, 9u, 3u, 5u, 2u, 10u};
	parts.calculate(nums);

	CHECK(parts.parts[0].size() == 3);
	CHECK(nums[parts.parts[0][0]] == 10);
	CHECK(nums[parts.parts[0][1]] == 3);
	CHECK(nums[parts.parts[0][2]] == 2);

	CHECK(parts.parts[1].size() == 3);
	CHECK(nums[parts.parts[1][0]] == 9);
	CHECK(nums[parts.parts[1][1]] == 5);
	CHECK(nums[parts.parts[1][2]] == 1);
}
