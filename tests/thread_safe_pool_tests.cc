#include "doctest.h"
#include "util/thread_safe_pool.hh"

using namespace cpputil;

TEST_CASE("ThreadSafePool: size matches template parameter") {
	ThreadSafePool<int, 4> pool;
	CHECK(pool.size() == 4);
}

TEST_CASE("ThreadSafePool: create returns valid index") {
	ThreadSafePool<int, 4> pool;
	auto idx = pool.create();
	CHECK(idx.has_value() == true);
	CHECK(idx.value() < 4);
}

TEST_CASE("ThreadSafePool: created slot can be written") {
	ThreadSafePool<int, 4> pool;
	auto idx = pool.create();
	pool[idx.value()] = 99;
	CHECK(pool[idx.value()] == 99);
}

TEST_CASE("ThreadSafePool: multiple creates return distinct indices") {
	ThreadSafePool<int, 4> pool;
	auto i0 = pool.create();
	auto i1 = pool.create();
	CHECK(i0.has_value());
	CHECK(i1.has_value());
	CHECK(i0.value() != i1.value());
}

TEST_CASE("ThreadSafePool: returns nullopt when full") {
	ThreadSafePool<int, 2> pool;
	pool.create();
	pool.create();
	auto idx = pool.create();
	CHECK(idx.has_value() == false);
}

TEST_CASE("ThreadSafePool: destroy frees slot") {
	ThreadSafePool<int, 1> pool;
	auto idx = pool.create();
	CHECK(idx.has_value());
	CHECK(pool.create().has_value() == false); // full

	bool ok = pool.destroy(idx.value());
	CHECK(ok == true);
	CHECK(pool.create().has_value() == true); // available again
}

TEST_CASE("ThreadSafePool: destroy returns false for out-of-range") {
	ThreadSafePool<int, 4> pool;
	CHECK(pool.destroy(100) == false);
}

TEST_CASE("ThreadSafePool: destroy returns false for unused slot") {
	ThreadSafePool<int, 4> pool;
	CHECK(pool.destroy(0) == false);
}

TEST_CASE("ThreadSafePool: clear resets all slots") {
	ThreadSafePool<int, 2> pool;
	pool.create();
	pool.create();
	CHECK(pool.create().has_value() == false);
	pool.clear();
	CHECK(pool.create().has_value() == true);
}
