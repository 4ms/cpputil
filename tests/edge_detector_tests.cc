#include "doctest.h"
#include "util/edge_detector.hh"

TEST_CASE("EdgeStateDetector: initial state is low") {
	EdgeStateDetector ed;
	CHECK(ed.is_high() == false);
	CHECK(ed.changed() == false);
	CHECK(ed.went_high() == false);
	CHECK(ed.went_low() == false);
	CHECK(ed.staying_low() == true);
	CHECK(ed.staying_high() == false);
}

TEST_CASE("EdgeStateDetector: rising edge") {
	EdgeStateDetector ed;
	ed.update(true);
	CHECK(ed.is_high() == true);
	CHECK(ed.changed() == true);
	CHECK(ed.went_high() == true);
	CHECK(ed.went_low() == false);
	CHECK(ed.staying_high() == false);
	CHECK(ed.staying_low() == false);
}

TEST_CASE("EdgeStateDetector: falling edge") {
	EdgeStateDetector ed;
	ed.update(true);
	ed.update(false);
	CHECK(ed.is_high() == false);
	CHECK(ed.changed() == true);
	CHECK(ed.went_high() == false);
	CHECK(ed.went_low() == true);
}

TEST_CASE("EdgeStateDetector: staying high") {
	EdgeStateDetector ed;
	ed.update(true);
	ed.update(true);
	CHECK(ed.is_high() == true);
	CHECK(ed.changed() == false);
	CHECK(ed.staying_high() == true);
	CHECK(ed.staying_low() == false);
}

TEST_CASE("EdgeStateDetector: staying low") {
	EdgeStateDetector ed;
	ed.update(false);
	CHECK(ed.staying_low() == true);
	CHECK(ed.changed() == false);
}

TEST_CASE("EdgeStateDetector: reset clears state") {
	EdgeStateDetector ed;
	ed.update(true);
	ed.reset();
	CHECK(ed.is_high() == false);
	CHECK(ed.changed() == false);
	CHECK(ed.staying_low() == true);
}

TEST_CASE("RisingEdgeDetector: detects rising edge") {
	RisingEdgeDetector red;
	CHECK(red.update(false) == false);
	CHECK(red.update(true) == true);
	CHECK(red.update(true) == false); // sustained high, no edge
	CHECK(red.update(false) == false);
	CHECK(red.update(true) == true); // new rising edge
}

TEST_CASE("RisingEdgeDetector: high-to-low is not a rising edge") {
	RisingEdgeDetector red;
	red.update(true); // primes last_state = true
	// Going high from low first:
	// Actually from false->true was the first call. Let's be explicit:
	RisingEdgeDetector red2;
	red2.update(false);
	red2.update(true);  // rising edge
	CHECK(red2.update(false) == false); // falling, not rising
}

TEST_CASE("FallingEdgeDetector: detects falling edge") {
	FallingEdgeDetector fed;
	CHECK(fed.update(true) == false);  // rising, not falling
	CHECK(fed.update(false) == true);  // falling edge
	CHECK(fed.update(false) == false); // staying low
	CHECK(fed.update(true) == false);  // rising, not falling
	CHECK(fed.update(false) == true);  // falling edge again
}

TEST_CASE("FallingEdgeDetector: starts low, stays low - no edge") {
	FallingEdgeDetector fed;
	CHECK(fed.update(false) == false);
	CHECK(fed.update(false) == false);
}
