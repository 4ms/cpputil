#include "doctest.h"
#include "util/input_output_buffer.hh"

TEST_CASE("InputBuffer: put and data access") {
	InputBuffer<int, 4> buf;
	buf.put(10);
	buf.put(20);
	CHECK(buf.data()[0] == 10);
	CHECK(buf.data()[1] == 20);
}

TEST_CASE("InputBuffer: size is always max_size") {
	InputBuffer<int, 8> buf;
	CHECK(buf.size() == 8);
	buf.put(1);
	CHECK(buf.size() == 8);
}

TEST_CASE("InputBuffer: full() when max reached") {
	InputBuffer<int, 2> buf;
	CHECK(buf.full() == false);
	buf.put(1);
	CHECK(buf.full() == false);
	buf.put(2);
	CHECK(buf.full() == true);
}

TEST_CASE("InputBuffer: put ignores extras when full") {
	InputBuffer<int, 2> buf;
	buf.put(1);
	buf.put(2);
	buf.put(99); // ignored
	CHECK(buf.data()[0] == 1);
	CHECK(buf.data()[1] == 2);
}

TEST_CASE("InputBuffer: reset allows reuse") {
	InputBuffer<int, 2> buf;
	buf.put(1);
	buf.put(2);
	CHECK(buf.full());
	buf.reset();
	CHECK(buf.full() == false);
	buf.put(3);
	CHECK(buf.data()[0] == 3);
}

TEST_CASE("InputBuffer: begin/end iterators") {
	InputBuffer<int, 4> buf;
	buf.put(5);
	buf.put(6);
	int sum = 0;
	for (auto *it = buf.begin(); it != buf.end(); ++it)
		sum += *it;
	// end() is at max_size_, not at the number of elements pushed
	// buf.begin()[0] == 5, buf.begin()[1] == 6, rest are 0
	CHECK(sum == 5 + 6); // rest are zero-initialized
}

TEST_CASE("OutputBuffer: get retrieves elements in order") {
	OutputBuffer<int, 4> buf;
	buf.data()[0] = 10;
	buf.data()[1] = 20;
	buf.data()[2] = 30;
	CHECK(buf.get() == 10);
	CHECK(buf.get() == 20);
	CHECK(buf.get() == 30);
}

TEST_CASE("OutputBuffer: get returns default when depleted") {
	OutputBuffer<int, 2> buf;
	buf.data()[0] = 5;
	buf.data()[1] = 6;
	buf.get();
	buf.get();
	CHECK(buf.full() == true);
	CHECK(buf.get() == 0); // default int
}

TEST_CASE("OutputBuffer: size is max_size") {
	OutputBuffer<int, 8> buf;
	CHECK(buf.size() == 8);
}

TEST_CASE("OutputBuffer: reset allows re-reading") {
	OutputBuffer<int, 2> buf;
	buf.data()[0] = 42;
	buf.get();
	buf.get();
	CHECK(buf.full());
	buf.reset();
	CHECK(buf.full() == false);
	CHECK(buf.get() == 42);
}

TEST_CASE("OutputBuffer: begin/end iterators") {
	OutputBuffer<int, 3> buf;
	buf.data()[0] = 1;
	buf.data()[1] = 2;
	buf.data()[2] = 3;
	int sum = 0;
	for (auto *it = buf.begin(); it != buf.end(); ++it)
		sum += *it;
	CHECK(sum == 6);
}
