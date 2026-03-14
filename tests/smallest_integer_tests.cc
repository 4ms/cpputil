#include "doctest.h"
#include "util/smallest_integer.hh"
#include <type_traits>

// The header already contains static_asserts that run at compile time.
// These tests verify the types at runtime for documentation/clarity.

TEST_CASE("SmallestUnsignedInt: fits in uint8_t") {
	CHECK((std::is_same_v<SmallestUnsignedInt<0>::type, uint8_t>));
	CHECK((std::is_same_v<SmallestUnsignedInt<127>::type, uint8_t>));
	CHECK((std::is_same_v<SmallestUnsignedInt<255>::type, uint8_t>));
}

TEST_CASE("SmallestUnsignedInt: requires uint16_t") {
	CHECK((std::is_same_v<SmallestUnsignedInt<256>::type, uint16_t>));
	CHECK((std::is_same_v<SmallestUnsignedInt<1000>::type, uint16_t>));
	CHECK((std::is_same_v<SmallestUnsignedInt<65535>::type, uint16_t>));
}

TEST_CASE("SmallestUnsignedInt: requires uint32_t") {
	CHECK((std::is_same_v<SmallestUnsignedInt<65536>::type, uint32_t>));
	CHECK((std::is_same_v<SmallestUnsignedInt<(1ull << 32) - 1>::type, uint32_t>));
}

TEST_CASE("SmallestUnsignedInt: requires uint64_t") {
	CHECK((std::is_same_v<SmallestUnsignedInt<(1ull << 32)>::type, uint64_t>));
}

TEST_CASE("SmallestSignedInt: fits in int8_t") {
	CHECK((std::is_same_v<SmallestSignedInt<0, 127>::type, int8_t>));
	CHECK((std::is_same_v<SmallestSignedInt<-128, 127>::type, int8_t>));
}

TEST_CASE("SmallestSignedInt: requires int16_t") {
	CHECK((std::is_same_v<SmallestSignedInt<0, 128>::type, int16_t>));
	CHECK((std::is_same_v<SmallestSignedInt<-129, 0>::type, int16_t>));
	CHECK((std::is_same_v<SmallestSignedInt<-32768, 32767>::type, int16_t>));
}

TEST_CASE("SmallestSignedInt: requires int32_t") {
	CHECK((std::is_same_v<SmallestSignedInt<-32768, 32768>::type, int32_t>));
	CHECK((std::is_same_v<SmallestSignedInt<-3293768, 3932768>::type, int32_t>));
}
