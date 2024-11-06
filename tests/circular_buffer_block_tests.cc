#include "doctest.h"

#include "util/circular_buffer_block.hh"

TEST_CASE("Basic usage") {

	CircularBufferBlock<char, 8> buf;

	std::array<char, 6> a{1, 2, 3, 4, 5, 6};

	buf.write(a, 0);
	CHECK(buf.data[0] == a[0]);
	CHECK(buf.data[1] == a[1]);
	CHECK(buf.data[2] == a[2]);
	CHECK(buf.data[3] == a[3]);
	CHECK(buf.data[4] == a[4]);
	CHECK(buf.data[5] == a[5]);

	std::array<char, 6> b{7, 8, 9, 10, 11, 12};
	buf.write(b, 6);
	CHECK(buf.data[6] == b[0]);
	CHECK(buf.data[7] == b[1]);
	CHECK(buf.data[0] == b[2]);
	CHECK(buf.data[1] == b[3]);
	CHECK(buf.data[2] == b[4]);
	CHECK(buf.data[3] == b[5]);

	std::array<char, 1> c{99};
	buf.write(c, 8);
	CHECK(buf.data[0] == 99);

	std::array<char, 1> d{88};
	buf.write(d, 7);
	CHECK(buf.data[7] == 88);
}
