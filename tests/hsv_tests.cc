#include "doctest.h"
#include "util/hsv.hh"

TEST_CASE("HSV to Color") {
	// check primary colors
	CHECK(Color{HSVColor{0, 255, 255}} == Colors::red);
	CHECK(Color{HSVColor{86, 255, 255}} == Colors::green);
	CHECK(Color{HSVColor{172, 255, 255}} == Colors::blue);

	// check no saturation
	const auto ns = Color{HSVColor{172, 0, 255}};
	CHECK(ns.red() == ns.green());
	CHECK(ns.red() == ns.blue());
	CHECK(ns.red() == 255);

	// check value
	const auto v = Color{HSVColor{0, 255, 0}};
	CHECK(v == Colors::black);
}
