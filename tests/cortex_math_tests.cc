#include "doctest.h"
#include "util/cortex_math.hh"

// Tests run the portable C++ fallback path (non-ARM targets)

TEST_CASE("signed_saturate: value within range passes through") {
	CHECK(MathTools::signed_saturate(10, 8) == 10);
	CHECK(MathTools::signed_saturate(0, 8) == 0);
	CHECK(MathTools::signed_saturate(-10, 8) == -10);
}

TEST_CASE("signed_saturate: clamps positive overflow") {
	// sat=8 means range [-128, 127]
	CHECK(MathTools::signed_saturate(200, 8) == 127);
	CHECK(MathTools::signed_saturate(1000, 8) == 127);
}

TEST_CASE("signed_saturate: clamps negative overflow") {
	// sat=8 means range [-128, 127]
	CHECK(MathTools::signed_saturate(-200, 8) == -128);
}

TEST_CASE("signed_saturate: sat=1 gives range [-1, 0]") {
	CHECK(MathTools::signed_saturate(5, 1) == 0);
	CHECK(MathTools::signed_saturate(-5, 1) == -1);
	CHECK(MathTools::signed_saturate(0, 1) == 0);
}

TEST_CASE("signed_saturate: sat=16 gives range [-32768, 32767]") {
	CHECK(MathTools::signed_saturate(32767, 16) == 32767);
	CHECK(MathTools::signed_saturate(32768, 16) == 32767);
	CHECK(MathTools::signed_saturate(-32768, 16) == -32768);
	CHECK(MathTools::signed_saturate(-32769, 16) == -32768);
}

TEST_CASE("unsigned_saturate: value within range passes through") {
	// sat=8 means range [0, 255]
	CHECK(MathTools::unsigned_saturate(100, 8) == 100);
	CHECK(MathTools::unsigned_saturate(255, 8) == 255);
	CHECK(MathTools::unsigned_saturate(0, 8) == 0);
}

TEST_CASE("unsigned_saturate: clamps positive overflow") {
	// sat=8 means max = 255
	CHECK(MathTools::unsigned_saturate(300, 8) == 255);
}

TEST_CASE("unsigned_saturate: clamps negative to zero") {
	CHECK(MathTools::unsigned_saturate(-1, 8) == 0);
	CHECK(MathTools::unsigned_saturate(-100, 8) == 0);
}

TEST_CASE("unsigned_saturate: sat=4 gives range [0, 15]") {
	CHECK(MathTools::unsigned_saturate(15, 4) == 15);
	CHECK(MathTools::unsigned_saturate(16, 4) == 15);
	CHECK(MathTools::unsigned_saturate(-1, 4) == 0);
}
