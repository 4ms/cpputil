#pragma once
#include "util/interp_array.hh"

#if 1

#include "../gcem/include/gcem.hpp"
#include <cstddef>

template<size_t Size>
consteval inline InterpArray<float, Size> make_sin_table() {
	InterpArray<float, Size> s;

	unsigned i = 0;
	for (auto &d : s.data) {
		float x = (float)i / (float)Size;
		d = gcem::sin(x * 2.0f * GCEM_PI);
		i++;
	}
	return s;
}
constexpr inline auto sinTable = make_sin_table<2048>();
static_assert(sinTable.data[0] == 0.f);
static_assert(sinTable.data[1] > 0.003067f);
static_assert(sinTable.data[1] < 0.003068f);
static_assert(sinTable.data[2] > 0.006135f);
static_assert(sinTable.data[2] < 0.006136f);
static_assert(sinTable.data[3] > 0.009203f);
static_assert(sinTable.data[3] < 0.009204f);

template<size_t Size, size_t VoltageRange>
consteval inline InterpArray<float, Size> make_exp_table() {
	InterpArray<float, Size> s;

	unsigned i = 0;
	float voltageRange = (float)VoltageRange;
	float powRange = (float)(Size - 1) / voltageRange;
	for (auto &d : s.data) {
		float index = (float)i / powRange;
		d = gcem::pow(2.0f, index);
		i++;
	}
	return s;
}
constexpr inline auto exp10Table = make_exp_table<2048, 10>();
constexpr inline auto exp5Table = make_exp_table<2048, 5>();

#else
extern const InterpArray<float, 2048> sinTable;
extern const InterpArray<float, 2048> exp5Table;
extern const InterpArray<float, 2048> exp10Table;
#endif

extern const InterpArray<float, 2048> tanTable;
extern const InterpArray<float, 2048> pow9Table;
extern const InterpArray<float, 2048> logTable;
