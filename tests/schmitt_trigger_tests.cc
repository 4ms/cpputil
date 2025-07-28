#include "doctest.h"
#include "util/schmitt_trigger.hh"

TEST_CASE("basic usage") {
	cpputil::SchmittTrigger t;

	t.process(0.3f);
	CHECK(t.is_high() == false);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.5f);
	CHECK(t.is_high() == false);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.6f);
	CHECK(t.is_high() == false);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.61f);
	CHECK(t.is_high() == true);
	CHECK(t.just_went_high() == true);
	CHECK(t.just_went_low() == false);

	// state clears after reading it
	CHECK(t.just_went_high() == false);

	t.process(0.7f);
	CHECK(t.is_high() == true);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.6f);
	CHECK(t.is_high() == true);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.5f);
	CHECK(t.is_high() == true);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.4f);
	CHECK(t.is_high() == true);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == false);

	t.process(0.39f);
	CHECK(t.is_high() == false);
	CHECK(t.just_went_high() == false);
	CHECK(t.just_went_low() == true);

	// state clears after reading it
	CHECK(t.just_went_low() == false);
}
