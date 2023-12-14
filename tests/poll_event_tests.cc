#include "../util/poll_event.hh"
#include "doctest.h"

TEST_CASE("Unsigned overflow of system time") {

	constexpr size_t period = 4;
	bool flag = false;
	auto action = [&flag] { flag = true; };
	// The action is called every 4 polls

	// Test at various starting times
	// so overflow happens at different times w.r.t. overflow
	{
		PollEvent<uint8_t> p{period};
		uint8_t now = 252;
		for (auto i = 0; i < 13; i++) {
			p.poll(now, action);
			CHECK(flag == ((i % 4) == 0));
			flag = false;
			now++;
		}
	}
	{
		PollEvent<uint8_t> p{period};
		uint8_t now = 251;
		for (auto i = 0; i < 13; i++) {
			p.poll(now, action);
			CHECK(flag == ((i % 4) == 0));
			flag = false;
			now++;
		}
	}
	{
		PollEvent<uint8_t> p{period};
		uint8_t now = 250;
		for (auto i = 0; i < 13; i++) {
			p.poll(now, action);
			CHECK(flag == ((i % 4) == 0));
			flag = false;
			now++;
		}
	}
	{
		PollEvent<uint8_t> p{period};
		uint8_t now = 249;
		for (auto i = 0; i < 13; i++) {
			p.poll(now, action);
			CHECK(flag == ((i % 4) == 0));
			flag = false;
			now++;
		}
	}
}
