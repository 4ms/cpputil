#include "doctest.h"
#include "util/change_detector.hh"

TEST_CASE("ChangeDetector: no change initially") {
	ChangeDetector cd;
	CHECK(cd.changed() == false);
}

TEST_CASE("ChangeDetector: detects change from false to true") {
	ChangeDetector cd;
	cd.update(true);
	CHECK(cd.changed() == true);
}

TEST_CASE("ChangeDetector: changed() clears after read") {
	ChangeDetector cd;
	cd.update(true);
	CHECK(cd.changed() == true);
	CHECK(cd.changed() == false);
}

TEST_CASE("ChangeDetector: same state does not trigger change") {
	ChangeDetector cd;
	cd.update(false);
	CHECK(cd.changed() == false);
	cd.update(false);
	CHECK(cd.changed() == false);
}

TEST_CASE("ChangeDetector: detects change from true to false") {
	ChangeDetector cd;
	cd.update(true);
	cd.changed(); // consume
	cd.update(false);
	CHECK(cd.changed() == true);
}

TEST_CASE("ChangeDetector: multiple updates before read latches change") {
	ChangeDetector cd;
	cd.update(true);
	cd.update(false);
	cd.update(true);
	CHECK(cd.changed() == true);
	CHECK(cd.changed() == false);
}

TEST_CASE("ChangeDetector: oscillation between reads reports change once") {
	ChangeDetector cd;
	// Start at false, update to true then back to false
	cd.update(true);
	cd.update(false);
	// changed_ is true (was set when going high, stays true)
	CHECK(cd.changed() == true);
	CHECK(cd.changed() == false);
}
