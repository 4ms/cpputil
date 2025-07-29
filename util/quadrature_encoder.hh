#pragma once

#include <cstdint>

template<bool invert = false>
class QuadratureEncoder {
	static constexpr uint8_t valid_ccw = 0b10000111 ^ (invert ? 0xff : 0x00);
	static constexpr uint8_t valid_cw = 0b01001011 ^ (invert ? 0xff : 0x00);

	uint8_t prev_state{};

public:
	enum Direction { None = 0, CW = 1, CCW = -1 };

	Direction get_motion(bool state_a, bool state_b) {
		const auto cur_state = state_a | (state_b << 1u);

		if ((prev_state & 0b11) == cur_state)
			return None;

		prev_state <<= 2;
		prev_state |= cur_state;

		switch (prev_state) {
			case valid_cw:
				return CW;
			case valid_ccw:
				return CCW;
			default:
				return None;
		}
	}
};
