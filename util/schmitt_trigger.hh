#pragma once
#include "util/debouncer.hh"

namespace cpputil
{

class SchmittTrigger : public Toggler {
	const float low;
	const float high;

public:
	SchmittTrigger(float low = 0.4f, float high = 0.6f)
		: low{low}
		, high{high} {
	}

	void process(float val) {
		if (!is_high_ && val > high) {
			register_rising_edge();

		} else if (is_high_ && val < low) {
			register_falling_edge();
		}
	}
};

class RisingSchmittTrigger {
	bool last_state_ = false;
	const float low;
	const float high;

public:
	RisingSchmittTrigger(float low = 0.4f, float high = 0.6f)
		: low{low}
		, high{high} {
	}

	bool update(float val) {
		if (!last_state_ && val > high) {
			last_state_ = true;
			return true;

		} else if (last_state_ && val < low) {
			last_state_ = false;
			return false;

		} else
			return false;
	}
};

} // namespace cpputil
