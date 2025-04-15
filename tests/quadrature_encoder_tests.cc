#include "doctest.h"
#include "util/quadrature_encoder.hh"

TEST_CASE("Quadrature Encoder") {

	using Enc = QuadratureEncoder<>;

	Enc encoder{};

	// check clockwise
	CHECK(encoder.get_motion(true, false) == Enc::None);
	CHECK(encoder.get_motion(false, false) == Enc::None);
	CHECK(encoder.get_motion(false, true) == Enc::None);
	CHECK(encoder.get_motion(true, true) == Enc::CW);

	// check clockwise no state change
	CHECK(encoder.get_motion(true, false) == Enc::None);
	CHECK(encoder.get_motion(true, false) == Enc::None);
	CHECK(encoder.get_motion(true, false) == Enc::None);
	CHECK(encoder.get_motion(true, false) == Enc::None);

	CHECK(encoder.get_motion(false, false) == Enc::None);
	CHECK(encoder.get_motion(false, false) == Enc::None);
	CHECK(encoder.get_motion(false, false) == Enc::None);
	CHECK(encoder.get_motion(false, false) == Enc::None);

	CHECK(encoder.get_motion(false, true) == Enc::None);
	CHECK(encoder.get_motion(false, true) == Enc::None);
	CHECK(encoder.get_motion(false, true) == Enc::None);
	CHECK(encoder.get_motion(false, true) == Enc::None);

	CHECK(encoder.get_motion(true, true) == Enc::CW);
	CHECK(encoder.get_motion(true, true) == Enc::None);
	CHECK(encoder.get_motion(true, true) == Enc::None);
	CHECK(encoder.get_motion(true, true) == Enc::None);

	// check counter-clockwise
	CHECK(encoder.get_motion(false, true) == Enc::None);
	CHECK(encoder.get_motion(false, false) == Enc::None);
	CHECK(encoder.get_motion(true, false) == Enc::None);
	CHECK(encoder.get_motion(true, true) == Enc::CCW);

	// check with no state change
	CHECK(encoder.get_motion(false, true) == Enc::None);
	CHECK(encoder.get_motion(false, true) == Enc::None);

	CHECK(encoder.get_motion(false, false) == Enc::None);
	CHECK(encoder.get_motion(false, false) == Enc::None);

	CHECK(encoder.get_motion(true, false) == Enc::None);
	CHECK(encoder.get_motion(true, false) == Enc::None);

	CHECK(encoder.get_motion(true, true) == Enc::CCW);
	CHECK(encoder.get_motion(true, true) == Enc::None);
}
