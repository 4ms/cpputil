#pragma once
#include <cstdint>

struct Toggler {

protected:
	bool is_high_;
	bool got_rising_edge_;
	bool got_falling_edge_;

public:
	Toggler()
		: is_high_{false}
		, got_rising_edge_{false}
		, got_falling_edge_{false} {
	}

	void clear_events() {
		got_falling_edge_ = false;
		got_rising_edge_ = false;
	}

	void reset() {
		is_high_ = false;
		clear_events();
	}

	operator bool() const {
		return is_high();
	}

	bool is_high() const {
		return is_high_;
	}

	bool is_pressed() const volatile {
		return is_high_;
	}

	bool is_just_pressed() {
		if (got_rising_edge_) {
			got_rising_edge_ = false;
			return true;
		} else
			return false;
	}

	bool just_went_high() {
		return is_just_pressed();
	}

	bool is_just_released() {
		if (got_falling_edge_) {
			got_falling_edge_ = false;
			return true;
		} else
			return false;
	}
	bool just_went_low() {
		return is_just_released();
	}

	void register_rising_edge() {
		is_high_ = true;
		got_rising_edge_ = true;
		got_falling_edge_ = false;
	}

	void register_falling_edge() {
		is_high_ = false;
		got_falling_edge_ = true;
		got_rising_edge_ = false;
	}

	void process(bool newstate) {
		register_state(newstate);
	}

	void register_state(bool newstate) {
		if (newstate == is_high_)
			return;

		is_high_ = newstate;

		if (newstate)
			got_rising_edge_ = true;
		else
			got_falling_edge_ = true;
	}

	[[deprecated("Misleading name: use set_state_no_events")]] void set_state(unsigned x) {
		is_high_ = x ? true : false;
	}

	void set_state_no_events(unsigned x) {
		is_high_ = x ? true : false;
	}

	void copy_state(const Toggler &other) {
		is_high_ = other.is_high_;
		got_rising_edge_ = other.got_rising_edge_;
		got_falling_edge_ = other.got_falling_edge_;
	}

	// Update the toggler with more recent events from another toggler
	void update_state(const Toggler &other) {
		if (other.got_falling_edge_)
			got_falling_edge_ = true;

		else if (other.got_rising_edge_)
			got_rising_edge_ = true;

		is_high_ = other.is_high_;
	}

	// Update the toggler with more recent events AND clear the other toggler
	void transfer_events(Toggler &other) {
		update_state(other);
		other.got_falling_edge_ = false;
		other.got_rising_edge_ = false;
	}

	Toggler &operator=(const Toggler &other) {
		copy_state(other);
		return *this;
	}
};

template<unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct Debouncer : Toggler {
	Debouncer()
		: debounce_state_{0} {
	}

	void register_state(unsigned new_state) {
		debounce_state_ = ((debounce_state_ << 1) | new_state) & StateMask;
		if (debounce_state_ == (RisingEdgePattern & StateMask)) {
			register_rising_edge();
		} else if (debounce_state_ == (FallingEdgePattern & StateMask)) {
			register_falling_edge();
		} else {
			set_state_no_events(new_state);
		}
	}

private:
	unsigned debounce_state_;
};

template<unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct DebouncerCounter : Toggler {
	void register_state(unsigned new_state) {
		debounce_state_ = ((debounce_state_ << 1) | new_state) & StateMask;
		if (debounce_state_ == (RisingEdgePattern & StateMask)) {
			register_rising_edge();
			steady_state_ctr = 0;
		} else if (debounce_state_ == (FallingEdgePattern & StateMask)) {
			register_falling_edge();
			steady_state_ctr = 0;
		} else {
			set_state_no_events(new_state);
			steady_state_ctr++;
		}
	}

	unsigned steady_state_ctr = 0;

private:
	unsigned debounce_state_ = 0;
};
