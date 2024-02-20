#pragma once
#include <array>
#include <cstddef>

template<typename T, size_t MaxElements>
class FixedVector {
	std::array<T, MaxElements> data;
	size_t back_idx = 0;

public:
	template<typename... Ts>
	FixedVector(const Ts... t)
		: data{t...} {
	}

	// Unchecked!
	T &operator[](size_t idx) {
		return data[idx];
	}

	const T &operator[](size_t idx) const {
		return data[idx];
	}

	bool push_back(T el) {
		if (back_idx >= MaxElements)
			return false;

		data[back_idx] = el;
		back_idx++;
		return true;
	}

	T pop_back() {
		if (back_idx == 0)
			return T{};

		back_idx--;
		return data[back_idx];
	}

	size_t size() const {
		return back_idx;
	}

	constexpr size_t max_size() {
		return MaxElements;
	}

	void clear() {
		back_idx = 0;
	}
};
