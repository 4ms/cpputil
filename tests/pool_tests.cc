#include "doctest.h"
#include "util/pool.hh"

using namespace cpputil;

TEST_CASE("Pool: create returns non-null pointer") {
	Pool<int, 4> pool;
	int *p = pool.create();
	CHECK(p != nullptr);
}

TEST_CASE("Pool: create default-constructs the element") {
	Pool<int, 4> pool;
	int *p = pool.create();
	CHECK(*p == 0);
}

TEST_CASE("Pool: created elements can be written and read") {
	Pool<int, 4> pool;
	int *p = pool.create();
	*p = 42;
	CHECK(*p == 42);
}

TEST_CASE("Pool: multiple creates return distinct pointers") {
	Pool<int, 4> pool;
	int *a = pool.create();
	int *b = pool.create();
	CHECK(a != nullptr);
	CHECK(b != nullptr);
	CHECK(a != b);
}

TEST_CASE("Pool: returns nullptr when full") {
	Pool<int, 2> pool;
	pool.create();
	pool.create();
	CHECK(pool.create() == nullptr);
}

TEST_CASE("Pool: destroy frees slot for reuse") {
	Pool<int, 1> pool;
	int *p = pool.create();
	CHECK(p != nullptr);
	CHECK(pool.create() == nullptr); // full
	bool destroyed = pool.destroy(p);
	CHECK(destroyed == true);
	CHECK(pool.create() != nullptr); // slot freed
}

TEST_CASE("Pool: destroy returns false for unknown pointer") {
	Pool<int, 4> pool;
	int x = 0;
	CHECK(pool.destroy(&x) == false);
}

TEST_CASE("Pool: destroy returns false for already-destroyed") {
	Pool<int, 4> pool;
	int *p = pool.create();
	pool.destroy(p);
	CHECK(pool.destroy(p) == false);
}

TEST_CASE("Pool: create_for_overwrite returns non-null") {
	Pool<int, 4> pool;
	int *p = pool.create_for_overwrite();
	CHECK(p != nullptr);
}

TEST_CASE("Pool: create_for_overwrite and destroy work together") {
	Pool<int, 1> pool;
	int *p = pool.create_for_overwrite();
	CHECK(pool.create_for_overwrite() == nullptr);
	pool.destroy(p);
	CHECK(pool.create_for_overwrite() != nullptr);
}
