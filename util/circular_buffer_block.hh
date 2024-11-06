#pragma once
#include "util/math.hh"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>

template<typename T, size_t SIZE>
struct CircularBufferBlock {
	static_assert(MathTools::is_power_of_2(SIZE));
	static constexpr unsigned SIZEMASK = (SIZE - 1);

	std::array<T, SIZE> data;

	void write(std::span<const T> block, unsigned start_pos) {
		unsigned offset = start_pos & SIZEMASK;
		auto begin = data.begin() + offset;

		if (offset + block.size() <= size()) {
			std::memcpy(&*begin, block.data(), block.size());
		} else {
			int space_avail = size() - offset;
			std::memcpy(&*begin, block.data(), space_avail);
			std::memcpy(data.data(), block.data() + space_avail, block.size() - space_avail);
		}
	}

	constexpr size_t size() {
		return data.size();
	}
};
