#include "doctest.h"
#include "util/bitfield.hh"
#include <cstdint>

enum class Flags : uint8_t {
	None = 0,
	A = 0x01,
	B = 0x02,
	C = 0x04,
	D = 0x08,
};

enum class Wide : uint32_t {
	Lo = 0x00000001,
	Hi = 0x80000000,
};

TEST_CASE("bitfield: single enum value returns underlying value") {
	CHECK(bitfield(Flags::A) == 0x01);
	CHECK(bitfield(Flags::B) == 0x02);
	CHECK(bitfield(Flags::C) == 0x04);
}

TEST_CASE("bitfield: two enum values are ORed") {
	CHECK(bitfield(Flags::A, Flags::B) == 0x03);
	CHECK(bitfield(Flags::A, Flags::C) == 0x05);
	CHECK(bitfield(Flags::B, Flags::D) == 0x0A);
}

TEST_CASE("bitfield: three enum values are ORed") {
	CHECK(bitfield(Flags::A, Flags::B, Flags::C) == 0x07);
}

TEST_CASE("bitfield: integral types pass through") {
	CHECK(bitfield(5) == 5);
	CHECK(bitfield(uint8_t{0xAB}) == 0xAB);
}

TEST_CASE("bitfield: wide enum values") {
	CHECK(bitfield(Wide::Lo, Wide::Hi) == 0x80000001u);
}

TEST_CASE("OR: single element") {
	CHECK(OR({Flags::A}) == 0x01);
}

TEST_CASE("OR: multiple elements combined") {
	CHECK(OR({Flags::A, Flags::B, Flags::D}) == 0x0B);
}

TEST_CASE("val: returns underlying value") {
	CHECK(val(Flags::A) == 0x01);
	CHECK(val(Flags::B) == 0x02);
	CHECK(val(Flags::None) == 0x00);
	CHECK(val(Wide::Hi) == 0x80000000u);
}
