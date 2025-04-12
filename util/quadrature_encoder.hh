#pragma once

#include <cstdint>

template<bool invert = false>
class QuadratureEncoder {
	static constexpr uint8_t valid_ccw = 0b10000111 ^ (invert ? 0xff : 0x00);
	static constexpr uint8_t valid_cw = 0b01001011 ^ (invert ? 0xff : 0x00);

	uint8_t prev_state{};

public:
	enum class State { Undefined, Right, Left };

	State update(bool state_a, bool state_b) {
		const auto cur_state = state_a | (state_b << 1u);
		prev_state <<= 2;
		prev_state |= cur_state;
		switch (prev_state) {
			case valid_cw:
				return State::Right;
			case valid_ccw:
				return State::Left;
			default:
				return State::Undefined;
		}
	}
};
