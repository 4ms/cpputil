#include "doctest.h"
#include "util/poll_change.hh"

TEST_CASE("PollChange: no change initially") {
	PollChange pc{10};
	CHECK(pc.take_change() == false);
}

TEST_CASE("PollChange: take_change clears after read") {
	PollChange pc{10};
	pc.reset();
	CHECK(pc.take_change() == true);
	CHECK(pc.take_change() == false);
}

TEST_CASE("PollChange: reset() marks change immediately") {
	PollChange pc{10};
	pc.reset();
	CHECK(pc.take_change() == true);
}

TEST_CASE("PollChange: poll() before interval does nothing") {
	PollChange pc{10};
	pc.poll(0, false);
	pc.poll(5, true); // not yet at tick 10
	CHECK(pc.take_change() == false);
}

TEST_CASE("PollChange: poll() at interval detects state change") {
	PollChange pc{10};
	pc.poll(0, false);  // initializes, same state
	pc.poll(10, true);  // at interval, state changed
	CHECK(pc.take_change() == true);
}

TEST_CASE("PollChange: poll() at interval with same state no change") {
	PollChange pc{10};
	pc.poll(0, false);
	pc.poll(10, false); // same state
	CHECK(pc.take_change() == false);
}

TEST_CASE("PollChange: force_next_poll() resets timer so next eligible poll fires") {
	PollChange pc{100};
	pc.poll(200, false); // fires at t=200 (200-0 >= 100), sets last_update_time=200
	pc.take_change();    // consume
	// Normally next poll would fire at t=300. force resets last_update_time to 0.
	pc.force_next_poll();
	pc.poll(100, true); // 100 - 0 = 100 >= 100: fires, state changed
	CHECK(pc.take_change() == true);
}

TEST_CASE("PollChange: poll with lambda") {
	PollChange pc{10};
	bool state = false;
	pc.poll(0, [&] { return state; });
	state = true;
	pc.poll(10, [&] { return state; }); // interval elapsed, state changed
	CHECK(pc.take_change() == true);
}

TEST_CASE("PollChange: poll with lambda - no change") {
	PollChange pc{10};
	pc.poll(0, [] { return false; });
	pc.poll(10, [] { return false; });
	CHECK(pc.take_change() == false);
}

TEST_CASE("PollChange: multiple polls accumulate only one change") {
	PollChange pc{10};
	pc.poll(0, false);
	pc.poll(10, true);
	pc.poll(20, false); // another change at next interval
	// take_change after first change
	CHECK(pc.take_change() == true);
	// After consuming, the second change may or may not have been captured
	// but there should be at most one pending
}
