#include "doctest.h"
#include "util/debouncer.hh"

TEST_CASE("Toggler: initial state is low") {
	Toggler t;
	CHECK(t.is_high() == false);
	CHECK(t.is_pressed() == false);
	CHECK(bool(t) == false);
}

TEST_CASE("Toggler: register_rising_edge sets high") {
	Toggler t;
	t.register_rising_edge();
	CHECK(t.is_high() == true);
	CHECK(t.is_just_pressed() == true);
	CHECK(t.is_just_pressed() == false); // clears after read
	CHECK(t.is_just_released() == false);
}

TEST_CASE("Toggler: register_falling_edge sets low") {
	Toggler t;
	t.register_rising_edge();
	t.is_just_pressed(); // consume
	t.register_falling_edge();
	CHECK(t.is_high() == false);
	CHECK(t.is_just_released() == true);
	CHECK(t.is_just_released() == false); // clears after read
}

TEST_CASE("Toggler: just_went_high/low aliases") {
	Toggler t;
	t.register_rising_edge();
	CHECK(t.just_went_high() == true);
	t.register_falling_edge();
	CHECK(t.just_went_low() == true);
}

TEST_CASE("Toggler: register_state detects transitions") {
	Toggler t;
	t.register_state(false); // no change (was already false)
	CHECK(t.is_high() == false);
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

TEST_CASE("Toggler: clear_events clears pending events") {
	Toggler t;
	t.register_rising_edge();
	t.clear_events();
	CHECK(t.is_just_pressed() == false);
	CHECK(t.is_just_released() == false);
	CHECK(t.is_high() == true); // state stays
}

TEST_CASE("Toggler: reset clears everything") {
	Toggler t;
	t.register_rising_edge();
	t.reset();
	CHECK(t.is_high() == false);
	CHECK(t.is_just_pressed() == false);
	CHECK(t.is_just_released() == false);
}

TEST_CASE("Toggler: set_state_no_events sets state without events") {
	Toggler t;
	t.set_state_no_events(1);
	CHECK(t.is_high() == true);
	CHECK(t.is_just_pressed() == false);
	t.set_state_no_events(0);
	CHECK(t.is_high() == false);
	CHECK(t.is_just_released() == false);
}

TEST_CASE("Toggler: copy_state copies all fields") {
	Toggler src;
	src.register_rising_edge();
	Toggler dst;
	dst.copy_state(src);
	CHECK(dst.is_high() == true);
	CHECK(dst.is_just_pressed() == true);
}

TEST_CASE("Toggler: assignment operator copies state") {
	Toggler src;
	src.register_rising_edge();
	Toggler dst;
	dst = src;
	CHECK(dst.is_high() == true);
	CHECK(dst.just_went_high() == true);
}

TEST_CASE("Toggler: update_state merges events") {
	Toggler src;
	src.register_rising_edge();
	Toggler dst;
	dst.update_state(src);
	CHECK(dst.is_high() == true);
	CHECK(dst.is_just_pressed() == true);
}

TEST_CASE("Toggler: transfer_events moves events and clears source") {
	Toggler src;
	src.register_rising_edge();
	Toggler dst;
	dst.transfer_events(src);
	CHECK(dst.is_high() == true);
	CHECK(dst.just_went_high() == true);
	CHECK(src.just_went_high() == false); // source cleared
}

TEST_CASE("Debouncer: rising edge requires pattern") {
	// Default pattern: RisingEdgePattern=0x01, StateMask=0x0FFF
	// Rising edge fires when debounce_state_ == 0x001 (one '1' after all zeros)
	Debouncer<> d;
	CHECK(d.is_high() == false);

	// Feed 0s then a single 1
	for (int i = 0; i < 11; i++)
		d.register_state(0);
	d.register_state(1);
	CHECK(d.is_high() == true);
	CHECK(d.is_just_pressed() == true);
}

TEST_CASE("Debouncer: falling edge requires pattern") {
	// FallingEdgePattern=0xFFFFFFFE, StateMask=0x0FFF -> 0x0FFE
	// Falling edge fires when state == 0x0FFE
	Debouncer<> d;
	// First get it high
	for (int i = 0; i < 11; i++)
		d.register_state(0);
	d.register_state(1);
	d.is_just_pressed(); // consume

	// Now fill with 1s then drop a 0
	for (int i = 0; i < 11; i++)
		d.register_state(1);
	d.register_state(0);
	CHECK(d.is_high() == false);
	CHECK(d.is_just_released() == true);
}

TEST_CASE("DebouncerCounter: counts steady state cycles") {
	DebouncerCounter<> dc;

	// Feed all zeros
	for (int i = 0; i < 5; i++)
		dc.register_state(0);

	// Counter increments when no edge detected
	CHECK(dc.steady_state_ctr > 0);

	// Rising edge resets counter
	for (int i = 0; i < 11; i++)
		dc.register_state(0);
	dc.register_state(1); // triggers rising edge
	CHECK(dc.steady_state_ctr == 0);
}
