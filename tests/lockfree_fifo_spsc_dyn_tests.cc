#include "doctest.h"
#include "util/lockfree_fifo_spsc_dyn.hh"

TEST_CASE("LockFreeFifoSpscDyn: basic put and get") {
	LockFreeFifoSpscDyn<int> q{4};
	CHECK(q.put(1) == true);
	auto val = q.get();
	CHECK(val.has_value());
	CHECK(val.value() == 1);
}

TEST_CASE("LockFreeFifoSpscDyn: get on empty returns nullopt") {
	LockFreeFifoSpscDyn<int> q{4};
	CHECK(q.get().has_value() == false);
}

TEST_CASE("LockFreeFifoSpscDyn: max_size matches constructor arg") {
	LockFreeFifoSpscDyn<int> q{8};
	CHECK(q.max_size() == 8);
}

TEST_CASE("LockFreeFifoSpscDyn: FIFO ordering") {
	LockFreeFifoSpscDyn<int> q{4};
	q.put(10);
	q.put(20);
	q.put(30);
	CHECK(q.get().value() == 10);
	CHECK(q.get().value() == 20);
	CHECK(q.get().value() == 30);
}

TEST_CASE("LockFreeFifoSpscDyn: put fails when full") {
	LockFreeFifoSpscDyn<int> q{2};
	CHECK(q.put(1) == true);
	CHECK(q.put(2) == true);
	CHECK(q.put(3) == false); // full
}

TEST_CASE("LockFreeFifoSpscDyn: empty/full state") {
	LockFreeFifoSpscDyn<int> q{2};
	CHECK(q.empty() == true);
	CHECK(q.full() == false);

	q.put(1);
	q.put(2);
	CHECK(q.empty() == false);
	CHECK(q.full() == true);
}

TEST_CASE("LockFreeFifoSpscDyn: num_filled and num_free") {
	LockFreeFifoSpscDyn<int> q{4};
	CHECK(q.num_filled() == 0);
	CHECK(q.num_free() == 4);

	q.put(1);
	q.put(2);
	CHECK(q.num_filled() == 2);
	CHECK(q.num_free() == 2);
}

TEST_CASE("LockFreeFifoSpscDyn: reset clears buffer") {
	LockFreeFifoSpscDyn<int> q{4};
	q.put(1);
	q.put(2);
	q.reset();
	CHECK(q.empty() == true);
	CHECK(q.get().has_value() == false);
}

TEST_CASE("LockFreeFifoSpscDyn: get_or_default returns default when empty") {
	LockFreeFifoSpscDyn<int> q{4};
	CHECK(q.get_or_default() == 0);
}

TEST_CASE("LockFreeFifoSpscDyn: get_move moves value out") {
	LockFreeFifoSpscDyn<int> q{4};
	q.put(55);
	int val = 0;
	bool ok = q.get_move(val);
	CHECK(ok == true);
	CHECK(val == 55);
	CHECK(q.empty() == true);
}

TEST_CASE("LockFreeFifoSpscDyn: remove_first discards element") {
	LockFreeFifoSpscDyn<int> q{4};
	q.put(1);
	q.put(2);
	q.remove_first();
	CHECK(q.get().value() == 2);
}

TEST_CASE("LockFreeFifoSpscDyn: resize changes capacity") {
	LockFreeFifoSpscDyn<int> q{4};
	q.resize(8);
	CHECK(q.max_size() == 8);
	CHECK(q.empty() == true);
}

TEST_CASE("LockFreeFifoSpscDyn: resize to same size is no-op") {
	LockFreeFifoSpscDyn<int> q{4};
	q.put(1);
	q.resize(4); // same size - no-op
	CHECK(q.num_filled() == 1); // content preserved
}

TEST_CASE("LockFreeFifoSpscDyn: head offset constructor") {
	LockFreeFifoSpscDyn<int> q{4, 2}; // head starts at 2
	CHECK(q.max_size() == 4);
}
