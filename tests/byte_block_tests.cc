#include "doctest.h"
#include <cstdint>
#include "util/byte_block.hh"

TEST_CASE("ByteBlock: initial state is empty") {
	ByteBlock bb;
	CHECK(bb.data.empty());
}

TEST_CASE("ByteBlock: printU8 adds one byte") {
	ByteBlock bb;
	bb.printU8(0xAB);
	REQUIRE(bb.data.size() == 1);
	CHECK(bb.data[0] == 0xAB);
}

TEST_CASE("ByteBlock: printU16LE adds two bytes little-endian") {
	ByteBlock bb;
	bb.printU16LE(0x1234);
	REQUIRE(bb.data.size() == 2);
	CHECK(bb.data[0] == 0x34); // low byte
	CHECK(bb.data[1] == 0x12); // high byte
}

TEST_CASE("ByteBlock: printU32LE adds four bytes little-endian") {
	ByteBlock bb;
	bb.printU32LE(0x12345678);
	REQUIRE(bb.data.size() == 4);
	CHECK(bb.data[0] == 0x78);
	CHECK(bb.data[1] == 0x56);
	CHECK(bb.data[2] == 0x34);
	CHECK(bb.data[3] == 0x12);
}

TEST_CASE("ByteBlock: print(uint8_t) delegates to printU8") {
	ByteBlock bb;
	bb.print(uint8_t{0xFF});
	REQUIRE(bb.data.size() == 1);
	CHECK(bb.data[0] == 0xFF);
}

TEST_CASE("ByteBlock: print(uint16_t) delegates to printU16LE") {
	ByteBlock bb;
	bb.print(uint16_t{0xABCD});
	REQUIRE(bb.data.size() == 2);
	CHECK(bb.data[0] == 0xCD);
	CHECK(bb.data[1] == 0xAB);
}

TEST_CASE("ByteBlock: print(uint32_t) delegates to printU32LE") {
	ByteBlock bb;
	bb.print(uint32_t{0xDEADBEEF});
	REQUIRE(bb.data.size() == 4);
	CHECK(bb.data[0] == 0xEF);
	CHECK(bb.data[1] == 0xBE);
	CHECK(bb.data[2] == 0xAD);
	CHECK(bb.data[3] == 0xDE);
}

TEST_CASE("ByteBlock: printRaw appends raw bytes") {
	ByteBlock bb;
	unsigned char raw[] = {0x01, 0x02, 0x03};
	bb.printRaw(raw, 3);
	REQUIRE(bb.data.size() == 3);
	CHECK(bb.data[0] == 0x01);
	CHECK(bb.data[1] == 0x02);
	CHECK(bb.data[2] == 0x03);
}

TEST_CASE("ByteBlock: multiple prints accumulate data") {
	ByteBlock bb;
	bb.printU8(0x01);
	bb.printU16LE(0x0203);
	bb.printU8(0x04);
	REQUIRE(bb.data.size() == 4);
	CHECK(bb.data[0] == 0x01);
	CHECK(bb.data[1] == 0x03);
	CHECK(bb.data[2] == 0x02);
	CHECK(bb.data[3] == 0x04);
}
