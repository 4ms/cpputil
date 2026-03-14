#include "doctest.h"
#include "util/circular_buffer_opt.hh"

TEST_CASE("CircularBufferOpt: basic put and get") {
	CircularBufferOpt<int, 4> buf;
	buf.put(42);
	auto val = buf.get();
	CHECK(val.has_value());
	CHECK(val.value() == 42);
}

TEST_CASE("CircularBufferOpt: get on empty returns nullopt") {
	CircularBufferOpt<int, 4> buf;
	CHECK(buf.get().has_value() == false);
}

TEST_CASE("CircularBufferOpt: FIFO ordering") {
	CircularBufferOpt<int, 4> buf;
	buf.put(1);
	buf.put(2);
	buf.put(3);
	CHECK(buf.get().value() == 1);
	CHECK(buf.get().value() == 2);
	CHECK(buf.get().value() == 3);
}

TEST_CASE("CircularBufferOpt: empty/full state") {
	CircularBufferOpt<int, 4> buf;
	CHECK(buf.empty() == true);
	CHECK(buf.full() == false);

	buf.put(1);
	buf.put(2);
	buf.put(3);
	buf.put(4);
	CHECK(buf.empty() == false);
	CHECK(buf.full() == true);
}

TEST_CASE("CircularBufferOpt: max_size and max_capacity") {
	CircularBufferOpt<int, 8> buf;
	CHECK(buf.max_size() == 8);
	CHECK(buf.max_capacity() == 8);
}

TEST_CASE("CircularBufferOpt: count tracks filled elements") {
	CircularBufferOpt<int, 4> buf;
	CHECK(buf.count() == 0);
	buf.put(1);
	CHECK(buf.count() == 1);
	buf.put(2);
	CHECK(buf.count() == 2);
	buf.remove_first();
	CHECK(buf.count() == 1);
}

TEST_CASE("CircularBufferOpt: num_filled alias for count") {
	CircularBufferOpt<int, 4> buf;
	buf.put(1);
	buf.put(2);
	CHECK(buf.num_filled() == 2);
}

TEST_CASE("CircularBufferOpt: overwrites oldest when full") {
	CircularBufferOpt<int, 4> buf;
	buf.put(1);
	buf.put(2);
	buf.put(3);
	buf.put(4);
	buf.put(5); // overwrites 1
	CHECK(buf.get().value() == 2);
}

TEST_CASE("CircularBufferOpt: reset clears buffer") {
	CircularBufferOpt<int, 4> buf;
	buf.put(1);
	buf.put(2);
	buf.reset();
	CHECK(buf.empty() == true);
	CHECK(buf.get().has_value() == false);
}

TEST_CASE("CircularBufferOpt: remove_first discards without returning") {
	CircularBufferOpt<int, 4> buf;
	buf.put(1);
	buf.put(2);
	buf.remove_first();
	CHECK(buf.get().value() == 2);
}

TEST_CASE("CircularBufferOpt: remove_first on empty is no-op") {
	CircularBufferOpt<int, 4> buf;
	buf.remove_first(); // should not crash
	CHECK(buf.empty() == true);
}

TEST_CASE("CircularBufferOpt: get_head_idx and get_tail_idx") {
	CircularBufferOpt<int, 4> buf;
	CHECK(buf.get_head_idx() == buf.get_tail_idx());
	buf.put(1);
	CHECK(buf.get_head_idx() != buf.get_tail_idx());
}
