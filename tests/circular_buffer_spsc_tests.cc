#include "doctest.h"
#include "util/circular_buffer_spsc.hh"

TEST_CASE("Basic usage: FIFO") {
	LockFreeQueueSpSc<int, 4> a;
	CHECK(a.SIZE_MASK == 0b11);
	CHECK(a.max_size() == 4);

	a.put(1);
	a.put(2);
	a.put(3);
	a.put(4);
	CHECK(a.get().value() == 1);
	CHECK(a.get().value() == 2);
	CHECK(a.get().value() == 3);
	CHECK(a.get().value() == 4);
}

TEST_CASE("Basic usage: Does not overwrites if put() past end") {
	LockFreeQueueSpSc<int, 4> a;

	a.put(1);
	a.put(2);
	a.put(3);
	a.put(4);
	CHECK(a.put(5) == false);

	//1 is NOT dropped
	CHECK(a.get().value() == 1);
	CHECK(a.get().value() == 2);
	CHECK(a.get().value() == 3);
	CHECK(a.get().value() == 4);
}

TEST_CASE("Basic usage: Interleaving put/get is ok") {
	LockFreeQueueSpSc<int, 4> a;

	a.put(1);
	a.put(2);
	CHECK(a.get().value() == 1);
	CHECK(a.get().value() == 2);
	a.put(3);
	CHECK(a.get().value() == 3);
	a.put(4);
	CHECK(a.get().value() == 4);
}

TEST_CASE("Basic usage: get() when empty returns default value (0)") {
	LockFreeQueueSpSc<int, 4> a;
	CHECK(a.get().has_value() == false);
	CHECK(a.get().has_value() == false);
	CHECK(a.get().has_value() == false);
	CHECK(a.get().has_value() == false);

	a.put(1);
	CHECK(a.get().value() == 1);
	CHECK(a.get().has_value() == false);
	CHECK(a.get().has_value() == false);
	CHECK(a.get().has_value() == false);
	CHECK(a.get().has_value() == false);
}

TEST_CASE("Basic usage: size()") {
	LockFreeQueueSpSc<int, 4> a;
	CHECK(a.num_filled() == 0);
	a.put(1);
	CHECK(a.num_filled() == 1);
	a.put(2);
	CHECK(a.num_filled() == 2);
	a.put(3);
	CHECK(a.num_filled() == 3);
	a.put(4);
	CHECK(a.num_filled() == 4);
}

TEST_CASE("Basic usage: full/empty()") {
	LockFreeQueueSpSc<int, 4> a;
	CHECK(a.empty());
	CHECK(!a.full());

	a.put(1);
	CHECK(!a.empty());
	CHECK(!a.full());

	a.put(2);
	a.put(3);
	a.put(4);
	CHECK(!a.empty());
	CHECK(a.full());

	a.put(5);
	CHECK(!a.empty());
	CHECK(a.full());

	a.get();
	CHECK(!a.empty());
	CHECK(!a.full());

	a.get();
	a.get();
	a.get();
	CHECK(a.empty());
	CHECK(!a.full());
}

TEST_CASE("Basic usage: reset") {
	LockFreeQueueSpSc<int, 4> a;

	a.put(1);
	a.put(2);
	a.put(3);
	a.put(4);
	a.reset();
	CHECK(a.empty());
	CHECK(!a.full());
	CHECK(a.num_filled() == 0);
}

TEST_CASE("Construct with offset") {
	LockFreeQueueSpSc<int, 4> a{2};

	a.put(1);
	CHECK(a.get().value() == 0);
	a.put(2);
	CHECK(a.get().value() == 0);
	a.put(3);
	CHECK(a.get().value() == 1);
	a.put(4);
	CHECK(a.get().value() == 2);
	a.put(5);
	CHECK(a.get().value() == 3);
	a.put(6);
	CHECK(a.get().value() == 4);
}
