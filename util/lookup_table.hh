#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace details
{

template<typename T, std::size_t NumPoints, typename rangeClass, typename Func>
struct CreateArray {
	constexpr CreateArray(const Func &func)
		: data() {
		constexpr float min = rangeClass::min;
		constexpr float max = rangeClass::max;

		for (std::size_t i = 0; i < NumPoints; i++) {
			auto x = min + i * (max - min) / float(NumPoints - 1);
			data[i] = func(x);
		}
	}
	std::array<T, NumPoints> data;
};

} // namespace details

template<std::size_t NumPoints, typename T = float>
class LookupTable {
public:
	using Base_t = std::array<T, NumPoints>;

public:
	constexpr LookupTable(const T min_, const T max_, const Base_t &input)
		: min(min_)
		, max(max_) {
		static_assert(NumPoints >= 2);
		std::copy_n(input.begin(), NumPoints, points.begin());
	}

	template<typename rangeClass, typename F>
	static constexpr LookupTable generate(const F func) {
		constexpr details::CreateArray<T, NumPoints, rangeClass, F> dataArray(func);
		return LookupTable(rangeClass::min, rangeClass::max, dataArray.data);
	}

	constexpr T lookup(T val) const {
		T idx = ((val - min) / (max - min)) * (NumPoints - 1);

		if (idx <= 0.f)
			return points.front();
		else if (idx >= (NumPoints - 1))
			return points.back();
		else {
			auto lower_idx = (uint32_t)idx;
			T phase = idx - lower_idx;
			auto lower = points[lower_idx];
			auto upper = points[lower_idx + 1];
			return lower + phase * (upper - lower);
		}
	}

	constexpr T operator()(T val) const {
		return lookup(val);
	}

private:
	Base_t points;
	const T min;
	const T max;
};

namespace LookupTableTests
{

struct InputRange {
	static constexpr float min = -0.2f;
	static constexpr float max = 0.2f;
};

static constinit auto TestTable = LookupTable<3>::generate<InputRange>([](auto input) { return input * input; });
//will work when clang handles non-integral constexpr(?)
// static_assert(TestTable.lookup(-0.2f) == 0.2f*0.2f);
// static_assert(TestTable.lookup(-0.1f) == 0.02f); // linear interpolation between previous (-0.2, 0.4) and next point (0,0)
// static_assert(TestTable.lookup(0.0f) == 0.0f);
// static_assert(TestTable.lookup(0.15f) == 0.03f); // linear interpolation
// static_assert(TestTable.lookup(0.2f) == 0.2f*0.2f);

} // namespace LookupTableTests
