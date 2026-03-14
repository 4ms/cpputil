#include "doctest.h"
#include "util/debouncer_compact.hh"

TEST_CASE("TogglerCompact: size is 1 byte") {
	CHECK(sizeof(TogglerCompact) == 1);
}

TEST_CASE("TogglerCompact: initial state is low") {
	TogglerCompact t;
	CHECK(t.is_high() == false);
	CHECK(t.is_pressed() == false);
	CHECK(bool(t) == false);
}

TEST_CASE("TogglerCompact: register_rising_edge") {
	TogglerCompact t;
	t.register_rising_edge();
	CHECK(t.is_high() == true);
	CHECK(t.is_just_pressed() == true);
	CHECK(t.is_just_pressed() == false); // clears after read
	CHECK(t.is_just_released() == false);
}

TEST_CASE("TogglerCompact: register_falling_edge") {
	TogglerCompact t;
	t.register_rising_edge();
	t.is_just_pressed();
	t.register_falling_edge();
	CHECK(t.is_high() == false);
	CHECK(t.is_just_released() == true);
	CHECK(t.is_just_released() == false);
}

TEST_CASE("TogglerCompact: just_went_high/low aliases") {
	TogglerCompact t;
	t.register_rising_edge();
	CHECK(t.just_went_high() == true);
	t.register_falling_edge();
	CHECK(t.just_went_low() == true);
}

TEST_CASE("TogglerCompact: register_state transitions") {
	TogglerCompact t;
	t.register_state(false);
	CHECK(t.is_just_pressed() == false);

	t.register_state(true);
	CHECK(t.is_high() == true);
	CHECK(t.is_just_pressed() == true);

	t.register_state(true); // no change
	CHECK(t.is_just_pressed() == false);

	t.register_state(false);
	CHECK(t.is_high() == false);
	CHECK(t.is_just_released() == true);
}

TEST_CASE("TogglerCompact: reset") {
	TogglerCompact t;
	t.register_rising_edge();
	t.reset();
	CHECK(t.is_high() == false);
	CHECK(t.is_just_pressed() == false);
}

TEST_CASE("TogglerCompact: clear_events preserves state") {
	TogglerCompact t;
	t.register_rising_edge();
	t.clear_events();
	CHECK(t.is_high() == true);
	CHECK(t.is_just_pressed() == false);
}

TEST_CASE("TogglerCompact: set_state_no_events") {
	TogglerCompact t;
	t.set_state_no_events(1);
	CHECK(t.is_high() == true);
	CHECK(t.is_just_pressed() == false);
}

TEST_CASE("TogglerCompact: copy_state and assignment") {
	TogglerCompact src;
	src.register_rising_edge();

	TogglerCompact dst;
	dst.copy_state(src);
	CHECK(dst.is_high() == true);
	CHECK(dst.just_went_high() == true);

	TogglerCompact dst2;
	dst2 = src;
	CHECK(dst2.is_high() == true);
}

TEST_CASE("TogglerCompact: update_state merges") {
	TogglerCompact src;
	src.register_rising_edge();
	TogglerCompact dst;
	dst.update_state(src);
	CHECK(dst.is_high() == true);
	CHECK(dst.just_went_high() == true);
}

TEST_CASE("TogglerCompact: transfer_events clears source") {
	TogglerCompact src;
	src.register_rising_edge();
	TogglerCompact dst;
	dst.transfer_events(src);
	CHECK(dst.just_went_high() == true);
	CHECK(src.just_went_high() == false);
}
