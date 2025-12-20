#include "doctest.h"
#include "util/colors_rgb565.hh"

TEST_CASE("Basic usage") {

	RGB565 c1;
	RGB565 c2{};
	RGB565 c3{(uint8_t)0x80, 0x40, 0x20};
	RGB565 c4{0.5f, 0.25f, 0.125f};
	RGB565 c5{0x804020};

	CHECK(c1.raw() == 0);
	CHECK(c2.raw() == 0);
	CHECK(c3.raw() == 0x8204);
	CHECK(c4.raw() == 0x8204);
	CHECK(c5.raw() == 0x8204);

	CHECK(c5.red() == 0x80);
	CHECK(c5.green() == 0x40);
	CHECK(c5.blue() == 0x20);

	c1 = 0x2334;
	CHECK(c1.raw() == 0x2334);
	CHECK((uint16_t)c1 == 0x2334);
}
