#pragma once

#include "util/colors.hh"
#include <cstdint>

class HSVColor {
public:
	uint8_t h;
	uint8_t s;
	uint8_t v;

	constexpr operator Color() const {
		if (!s) {
			return Color{v, v, v};
		}

		const auto region = h / 43u;
		const auto remainder = (h - (region * 43u)) * 6u;

		const uint8_t p = (v * (255u - s)) >> 8u;
		const uint8_t q = (v * (255u - ((s * remainder) >> 8u))) >> 8u;
		const uint8_t t = (v * (255u - ((s * (255u - remainder)) >> 8u))) >> 8u;

		switch (region) {
			case 0:
				return Color{v, t, p};
			case 1:
				return Color{q, v, p};
			case 2:
				return Color{p, v, t};
			case 3:
				return Color{p, q, v};
			case 4:
				return Color{t, p, v};
			default:
				return Color{v, p, q};
		}
	}
};
