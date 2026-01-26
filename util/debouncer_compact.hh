#pragma once
#include <cstdint>

// 1-byte version of toggler
struct TogglerCompact {

protected:
	uint8_t is_high_ : 1;
	uint8_t got_rising_edge_ : 1;
	uint8_t got_falling_edge_ : 1;

public:
	TogglerCompact()
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

	void set_state_no_events(unsigned x) {
		is_high_ = x ? true : false;
	}

	void copy_state(const TogglerCompact &other) {
		is_high_ = other.is_high_;
		got_rising_edge_ = other.got_rising_edge_;
		got_falling_edge_ = other.got_falling_edge_;
	}

	// Update the toggler with more recent events from another toggler
	void update_state(const TogglerCompact &other) {
		if (other.got_falling_edge_)
			got_falling_edge_ = true;

		else if (other.got_rising_edge_)
			got_rising_edge_ = true;

		is_high_ = other.is_high_;
	}

	// Update the toggler with more recent events AND clear the other toggler
	void transfer_events(TogglerCompact &other) {
		update_state(other);
		other.got_falling_edge_ = false;
		other.got_rising_edge_ = false;
	}

	TogglerCompact &operator=(const TogglerCompact &other) {
		copy_state(other);
		return *this;
	}
};

static_assert(sizeof(TogglerCompact) == 1);
