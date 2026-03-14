#include "doctest.h"
#include "util/padded_aligned.hh"
#include <cstdint>

TEST_CASE("PaddedAligned: total size is aligned") {
	// int (4 bytes) with 16-byte alignment -> padded to 16
	CHECK(sizeof(PaddedAligned<int, 16>) == 16);
}

TEST_CASE("PaddedAligned: no padding needed when already aligned") {
	// 16-byte type with 16-byte alignment -> no padding
	struct Aligned16 { char data[16]; };
	CHECK(sizeof(PaddedAligned<Aligned16, 16>) == 16);
}

TEST_CASE("PaddedAligned: larger type rounded up") {
	// 20-byte type with 16-byte alignment -> padded to 32
	struct T20 { char data[20]; };
	CHECK(sizeof(PaddedAligned<T20, 16>) == 32);
}

TEST_CASE("PaddedAligned: data is accessible via operator()") {
	PaddedAligned<int, 16> pa;
	pa() = 42;
	CHECK(pa() == 42);
}

TEST_CASE("PaddedAligned: implicit conversion to T&") {
	PaddedAligned<int, 16> pa;
	pa.data = 7;
	int &ref = pa;
	CHECK(ref == 7);
	ref = 99;
	CHECK(pa.data == 99);
}

TEST_CASE("PaddedAligned: alignment of data member") {
	PaddedAligned<int, 64> pa;
	auto addr = reinterpret_cast<uintptr_t>(&pa.data);
	CHECK((addr % 64) == 0);
}

TEST_CASE("PaddedAligned: constexpr size calculations") {
	CHECK(PaddedAligned<int, 8>::data_sz == sizeof(int));
	CHECK(PaddedAligned<int, 8>::aligned_sz == 8);
	CHECK(PaddedAligned<int, 8>::pad_sz == 8 - sizeof(int));
}
