#include "doctest.h"
#include "util/debouncer.hh"

TEST_CASE("TogglerCounter starts low with zero count and no events") {
	TogglerCounter t;

	CHECK(t.is_high() == false);
	CHECK(t.is_pressed() == false);
	CHECK(bool(t) == false);
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_just_pressed() == false);
	CHECK(t.is_just_released() == false);
}

TEST_CASE("TogglerCounter rising edge sets state and event, resets count") {
	TogglerCounter t;

	t.process(1);
	CHECK(t.is_high() == true);
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_just_pressed() == true);
	CHECK(t.is_just_pressed() == false); // event clears after read
	CHECK(t.is_just_released() == false);
}

TEST_CASE("TogglerCounter steady state increments count without events") {
	TogglerCounter t;

	// staying low: count increments each call
	t.process(0);
	CHECK(t.steady_state_count() == 1);
	CHECK(t.is_just_pressed() == false);
	CHECK(t.is_just_released() == false);

	t.process(0);
	CHECK(t.steady_state_count() == 2);

	t.process(0);
	CHECK(t.steady_state_count() == 3);
	CHECK(t.is_high() == false);
}

TEST_CASE("TogglerCounter falling edge resets count and fires event") {
	TogglerCounter t;

	t.process(1);
	CHECK(t.steady_state_count() == 0);

	t.process(1);
	t.process(1);
	CHECK(t.steady_state_count() == 2);

	t.process(0);
	// Count reset:
	CHECK(t.steady_state_count() == 0);
	// Event fired:
	CHECK(t.is_just_released() == true);
	CHECK(t.is_just_released() == false);
}

TEST_CASE("TogglerCounter count accumulates while steady high") {
	TogglerCounter t;

	// Start with button pressed
	t.process(1);

	for (unsigned i = 1; i <= 10; i++) {
		t.process(1);
		CHECK(t.steady_state_count() == i);
	}
}

TEST_CASE("TogglerCounter count accumulates while steady low") {
	TogglerCounter t;

	for (unsigned i = 1; i <= 10; i++) {
		t.process(0);
		CHECK(t.steady_state_count() == i);
	}
}

TEST_CASE("TogglerCounter register_state mirrors process") {
	TogglerCounter t;

	t.register_state(1);
	CHECK(t.is_high() == true);
	CHECK(t.steady_state_count() == 0);

	t.register_state(1);
	CHECK(t.steady_state_count() == 1);

	t.register_state(0);
	CHECK(t.is_high() == false);
	CHECK(t.steady_state_count() == 0);
}

TEST_CASE("TogglerCounter reset_count clears count without affecting state") {
	TogglerCounter t;

	t.process(1);
	t.process(1);
	t.process(1);
	t.process(1);
	CHECK(t.steady_state_count() == 3);
	CHECK(t.is_high() == true);

	t.reset_count();
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_high() == true);

	t.process(1);
	CHECK(t.steady_state_count() == 1);
}

TEST_CASE("TogglerCounter reset clears state and events but not count") {
	TogglerCounter t;

	t.process(1);
	t.process(1);
	CHECK(t.steady_state_count() == 1);

	t.reset();
	CHECK(t.is_high() == false);
	CHECK(t.is_just_pressed() == false);
	CHECK(t.is_just_released() == false);

	// after reset (state low) processing 0 increments steady count
	t.process(0);
	CHECK(t.steady_state_count() == 2);
}

TEST_CASE("TogglerCounter clear_events does not change state or count") {
	TogglerCounter t;

	t.process(1);
	CHECK(t.is_high() == true);

	t.clear_events();
	CHECK(t.is_just_pressed() == false);
	CHECK(t.is_just_released() == false);
	CHECK(t.is_high() == true);

	t.process(1);
	CHECK(t.steady_state_count() == 1);
}

TEST_CASE("TogglerCounter alternating transitions reset count each edge") {
	TogglerCounter t;

	t.process(1);
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_just_pressed() == true);

	t.process(0);
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_just_released() == true);

	t.process(1);
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_just_pressed() == true);

	t.process(0);
	CHECK(t.steady_state_count() == 0);
	CHECK(t.is_just_released() == true);
}

TEST_CASE("TogglerCounter is_just_pressed cleared after read across steady samples") {
	TogglerCounter t;

	t.process(1);
	t.process(1);
	t.process(1);
	CHECK(t.is_just_pressed() == true);
	CHECK(t.is_just_pressed() == false);
	CHECK(t.steady_state_count() == 2);
	CHECK(t.is_high() == true);
}
