#pragma once

struct PollEvent {
	PollEvent(int period)
		: period{period} {
	}

	void poll(int now, auto action) {
		if (now - last_poll > period) {
			last_poll = now;
			action();
		}
	}

private:
	int period;
	int last_poll;
};
