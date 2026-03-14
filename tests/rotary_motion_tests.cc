#include "doctest.h"
#include "util/rotary_motion.hh"

TEST_CASE("RotaryMotion: initial state is zero") {
	RotaryMotion rm;
	CHECK(rm.abs_pos == 0);
	CHECK(rm.motion == 0);
}

TEST_CASE("RotaryMotion: use_motion returns motion and clears it") {
	RotaryMotion rm;
	rm.motion = 5;
	CHECK(rm.use_motion() == 5);
	CHECK(rm.motion == 0);
}

TEST_CASE("RotaryMotion: use_motion on zero returns zero") {
	RotaryMotion rm;
	CHECK(rm.use_motion() == 0);
}

TEST_CASE("RotaryMotion: add_motion accumulates motion and abs_pos") {
	RotaryMotion rm;
	RotaryMotion src;
	src.motion = 3;

	rm.add_motion(src);
	CHECK(rm.motion == 3);
	CHECK(rm.abs_pos == 3);

	rm.add_motion(src); // add again (src still has motion=3)
	CHECK(rm.motion == 6);
	CHECK(rm.abs_pos == 6);
}

TEST_CASE("RotaryMotion: add_motion with negative motion") {
	RotaryMotion rm;
	RotaryMotion src;
	src.motion = -2;
	rm.add_motion(src);
	CHECK(rm.motion == -2);
	CHECK(rm.abs_pos == -2);
}

TEST_CASE("RotaryMotion: transfer_motion moves from source and clears it") {
	RotaryMotion src;
	src.motion = 7;
	RotaryMotion dst;
	dst.transfer_motion(src);
	CHECK(dst.motion == 7);
	CHECK(dst.abs_pos == 7);
	CHECK(src.motion == 0); // consumed
}

TEST_CASE("RotaryMotion: transfer_motion accumulates over multiple transfers") {
	RotaryMotion src;
	RotaryMotion dst;

	src.motion = 3;
	dst.transfer_motion(src);

	src.motion = 5;
	dst.transfer_motion(src);

	CHECK(dst.motion == 8);
	CHECK(dst.abs_pos == 8);
}

TEST_CASE("RotaryMotion: abs_pos tracks cumulative position") {
	RotaryMotion rm;
	RotaryMotion inc;
	inc.motion = 1;

	for (int i = 0; i < 10; i++)
		rm.add_motion(inc);

	CHECK(rm.abs_pos == 10);

	// use_motion clears motion but not abs_pos
	rm.use_motion();
	CHECK(rm.abs_pos == 10);
	CHECK(rm.motion == 0);
}
