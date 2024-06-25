#include "doctest.h"
#include "util/filter.hh"

TEST_CASE("ResizingOversampler") {

	ResizingOversampler os{6};

	os.add_val(1);
	CHECK(os.val() == 0);
	os.add_val(2);
	CHECK(os.val() == 0);
	os.add_val(3);
	CHECK(os.val() == 0);
	os.add_val(4);
	CHECK(os.val() == 0);
	os.add_val(5);
	CHECK(os.val() == 0);
	os.add_val(6);
	CHECK(os.val() == (1 + 2 + 3 + 4 + 5 + 6) / 6.f);

	os.add_val(7);
	CHECK(os.val() == (1 + 2 + 3 + 4 + 5 + 6) / 6.f);

	os.add_val(8);
	CHECK(os.val() == (1 + 2 + 3 + 4 + 5 + 6) / 6.f);

	os.add_val(9);
	CHECK(os.val() == (1 + 2 + 3 + 4 + 5 + 6) / 6.f);

	os.add_val(10);
	CHECK(os.val() == (1 + 2 + 3 + 4 + 5 + 6) / 6.f);

	//now we have 4 values in the buffer. Resizing should average those values
	os.set_size(4);
	CHECK(os.val() == (7 + 8 + 9 + 10) / 4.f);

	os.add_val(20);
	CHECK(os.val() == (7 + 8 + 9 + 10) / 4.f);

	os.add_val(21);
	CHECK(os.val() == (7 + 8 + 9 + 10) / 4.f);

	os.add_val(22);
	CHECK(os.val() == (7 + 8 + 9 + 10) / 4.f);

	os.add_val(23);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);

	// Resize larger does not clear values
	os.add_val(30);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);
	os.add_val(31);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);
	os.set_size(6);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);
	os.add_val(32);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);
	os.add_val(33);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);
	os.add_val(34);
	CHECK(os.val() == (20 + 21 + 22 + 23) / 4.f);
	os.add_val(35);
	CHECK(os.val() == (30 + 31 + 32 + 33 + 34 + 35) / 6.f);
}
