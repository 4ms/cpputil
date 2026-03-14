#include "doctest.h"
#include "util/monotonic_allocator.hh"
#include <array>

TEST_CASE("MonotonicAllocator: initial state") {
	std::array<uint8_t, 64> arena{};
	MonotonicAllocator alloc{arena};
	CHECK(alloc.total_size() == 64);
	CHECK(alloc.free_space() == 64);
}

TEST_CASE("MonotonicAllocator: allocate reduces free space") {
	std::array<uint8_t, 64> arena{};
	MonotonicAllocator alloc{arena};
	alloc.allocate(16);
	CHECK(alloc.free_space() == 48);
}

TEST_CASE("MonotonicAllocator: returns valid pointer") {
	std::array<uint8_t, 64> arena{};
	MonotonicAllocator alloc{arena};
	uint8_t *p = alloc.allocate(8);
	CHECK(p != nullptr);
	p[0] = 42;
	CHECK(p[0] == 42);
}

TEST_CASE("MonotonicAllocator: sequential allocations are contiguous") {
	std::array<uint8_t, 64> arena{};
	MonotonicAllocator alloc{arena};
	uint8_t *a = alloc.allocate(8);
	uint8_t *b = alloc.allocate(8);
	CHECK(b == a + 8);
}

TEST_CASE("MonotonicAllocator: returns nullptr when out of space") {
	std::array<uint8_t, 8> arena{};
	MonotonicAllocator alloc{arena};
	uint8_t *p = alloc.allocate(8); // exactly fills (not strictly greater)
	// allocate(size) requires bytesRemaining > size (strictly), so exactly 8 fails
	CHECK(p == nullptr);
}

TEST_CASE("MonotonicAllocator: allocate all but one byte then one more fails") {
	std::array<uint8_t, 16> arena{};
	MonotonicAllocator alloc{arena};
	uint8_t *p = alloc.allocate(15);
	CHECK(p != nullptr);
	CHECK(alloc.free_space() == 1);
	// free_space(1) is not > 1, so next alloc fails
	CHECK(alloc.allocate(1) == nullptr);
}

TEST_CASE("MonotonicAllocator: reset restores full capacity") {
	std::array<uint8_t, 64> arena{};
	MonotonicAllocator alloc{arena};
	alloc.allocate(32);
	alloc.reset();
	CHECK(alloc.free_space() == 64);
}

TEST_CASE("MonotonicAllocator: total_size unchanged after allocations") {
	std::array<uint8_t, 64> arena{};
	MonotonicAllocator alloc{arena};
	alloc.allocate(16);
	CHECK(alloc.total_size() == 64);
}
