#include "doctest.h"
#include "util/aligned_allocator.hh"
#include <cstdint>
#include <vector>

TEST_CASE("AlignedAllocator: allocate and deallocate single element") {
	AlignedAllocator<int> alloc;
	int *p = alloc.allocate(1);
	CHECK(p != nullptr);
	*p = 42;
	CHECK(*p == 42);
	alloc.deallocate(p, 1);
}

TEST_CASE("AlignedAllocator: allocate returns aligned pointer") {
	AlignedAllocator<int, 64> alloc;
	int *p = alloc.allocate(4);
	CHECK(p != nullptr);
	auto addr = reinterpret_cast<uintptr_t>(p);
	CHECK((addr % 64) == 0);
	alloc.deallocate(p, 4);
}

TEST_CASE("AlignedAllocator: allocate zero returns nullptr") {
	AlignedAllocator<int> alloc;
	int *p = alloc.allocate(0);
	CHECK(p == nullptr);
}

TEST_CASE("AlignedAllocator: equality") {
	AlignedAllocator<int> a, b;
	CHECK(a == b);
	CHECK(!(a != b));
}

TEST_CASE("AlignedAllocator: rebind type") {
	AlignedAllocator<int, 32> alloc;
	AlignedAllocator<int, 32>::rebind<double>::other dbl_alloc(alloc);
	double *p = dbl_alloc.allocate(2);
	CHECK(p != nullptr);
	auto addr = reinterpret_cast<uintptr_t>(p);
	CHECK((addr % 32) == 0);
	dbl_alloc.deallocate(p, 2);
}

TEST_CASE("AlignedAllocator: works with std::vector") {
	std::vector<float, AlignedAllocator<float, 16>> v;
	v.push_back(1.0f);
	v.push_back(2.0f);
	v.push_back(3.0f);
	CHECK(v.size() == 3);
	CHECK(v[0] == 1.0f);
	CHECK(v[2] == 3.0f);
}
