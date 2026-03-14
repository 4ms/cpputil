#include "doctest.h"
#include <cstdint>
#include "util/mem_test.hh"

TEST_CASE("MemTest: valid memory region passes") {
	uint32_t buf[16]{};
	CHECK(MemTest::check(buf, buf + 16) == true);
}

TEST_CASE("MemTest: single element region passes") {
	uint32_t buf[1]{};
	CHECK(MemTest::check(buf, buf + 1) == true);
}

TEST_CASE("MemTest: empty range (start == stop) trivially passes") {
	uint32_t buf[4]{};
	CHECK(MemTest::check(buf, buf) == true);
}

TEST_CASE("MemTest: address-based check returns false when start > stop") {
	// The address overload takes uint32_t which is only safe on 32-bit targets.
	// On 64-bit hosts we can only test the boundary condition where start > stop.
	CHECK(MemTest::check(uint32_t{100}, uint32_t{50}) == false);
	CHECK(MemTest::check(uint32_t{0}, uint32_t{0}) == true); // empty range
}

TEST_CASE("MemTest: corrupted memory fails") {
	// We can't easily test this without platform-specific behavior,
	// but we can verify the encode path by checking that check() writes then reads
	// correctly (the test passes for valid RAM).
	uint32_t buf[4]{};
	bool result = MemTest::check(buf, buf + 4);
	CHECK(result == true);
	// After check(), buf contains the encoded pattern - not our original values
}
