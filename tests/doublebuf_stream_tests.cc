#include <cstdint>
#include "doctest.h"
#include "util/doublebuf_stream.hh"

TEST_CASE("Transmit without errors, always complete transmission before another is started") {
	std::vector<uint8_t> log;

	DoubleBufferStream<uint8_t, 8> double_buf{[&log](std::span<uint8_t> buf) {
		log.insert(log.end(), buf.begin(), buf.end());
		return true;
	}};

	std::array<uint8_t, 3> a1 = {'A', 'b', 'c'};
	CHECK(double_buf.transmit(a1));
	double_buf.tx_done_callback();

	CHECK(log.size() == 3);
	CHECK(log[0] == 'A');
	CHECK(log[1] == 'b');
	CHECK(log[2] == 'c');

	// rely on unsigned overflow:
	uint32_t word = 0xb0227f;
	std::array<uint8_t, 3> bytes{uint8_t(word >> 16), uint8_t(word >> 8), uint8_t(word)};
	CHECK(double_buf.transmit(bytes));
	double_buf.tx_done_callback();

	CHECK(log.size() == 6);
	CHECK(log[3] == 0xb0);
	CHECK(log[4] == 0x22);
	CHECK(log[5] == 0x7f);

	// Can send up to max size of buffer
	std::array<uint8_t, 8> a2 = {1, 2, 3, 4, 5, 6, 7, 8};
	CHECK(double_buf.transmit(a2));
	double_buf.tx_done_callback();

	CHECK(log.size() == 14);
	CHECK(log[6] == 1);
	CHECK(log[7] == 2);
	CHECK(log[8] == 3);
	CHECK(log[9] == 4);
	CHECK(log[10] == 5);
	CHECK(log[11] == 6);
	CHECK(log[12] == 7);
	CHECK(log[13] == 8);

	// Cannot send more then the buffer size
	std::array<uint8_t, 9> a3 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	CHECK_FALSE(double_buf.transmit(a3));
	CHECK(log.size() == 14);
}

TEST_CASE("Transmit without errors, interleaving transmissions") {
	std::vector<uint8_t> log;

	DoubleBufferStream<uint8_t, 8> double_buf{[&log](std::span<uint8_t> buf) {
		log.insert(log.end(), buf.begin(), buf.end());
		return true;
	}};

	std::array<uint8_t, 3> a1 = {1, 2, 3};
	CHECK(double_buf.transmit(a1));

	std::array<uint8_t, 4> a11 = {4, 5, 6, 7};
	CHECK(double_buf.transmit(a11));

	std::array<uint8_t, 1> a2 = {8};
	CHECK(double_buf.transmit(a2));

	std::array<uint8_t, 2> a22 = {9, 10};
	CHECK(double_buf.transmit(a22));

	// a1 has been sent in the first internal buffer.
	// but the inactive buffer will not be transmitted until
	// the tx_done_callback() is called
	CHECK(log.size() == 3);

	double_buf.tx_done_callback();
	// Now the inactive buffer, which contains the remaining data,
	// will be sent
	CHECK(log.size() == 10);

	CHECK(log[0] == 1);
	CHECK(log[1] == 2);
	CHECK(log[2] == 3);
	CHECK(log[3] == 4);
	CHECK(log[4] == 5);
	CHECK(log[5] == 6);
	CHECK(log[6] == 7);
	CHECK(log[7] == 8);
	CHECK(log[8] == 9);
	CHECK(log[9] == 10);
}
