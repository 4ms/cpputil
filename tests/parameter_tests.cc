#include "doctest.h"
#include "util/parameter.hh"

TEST_CASE("basic usage") {

	//
	LatchedParam<float, 1, 4096> x;
	LatchedParam<float, 1, 4096> y;

	x = 2.4f;
	CHECK(x.val == 2.4f);
	CHECK(x.changed == false);

	x.store_changed(10.f);
	CHECK(x.val == 10.f);
	CHECK(x.changed == true);

	CHECK(y.val == 0.f);
	CHECK(y.changed == false);
	y = x;
	CHECK(y.val == 10.f);
	CHECK(y.changed == true);

	CHECK(y.did_change());
	CHECK_FALSE(y.did_change());
}
