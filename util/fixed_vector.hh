#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <span>

template<typename T, size_t MaxElements>
class FixedVector {
	std::array<T, MaxElements> data;
	size_t back_idx = 0;

public:
	template<typename... Ts>
	constexpr FixedVector(const Ts... t)
		: data{t...}
		, back_idx{sizeof...(t)} {
	}

	// Unchecked!
	constexpr T &operator[](size_t idx) {
		return data[idx];
	}

	constexpr const T &operator[](size_t idx) const {
		return data[idx];
	}

	constexpr T back() const {
		if (back_idx > 0)
			return data[back_idx - 1];
		else
			return T{};
	}

	constexpr bool push_back(T el) {
		if (back_idx >= MaxElements)
			return false;

		data[back_idx] = el;
		back_idx++;
		return true;
	}

	// Resizes
	constexpr bool resize(size_t num, T el = T{}) {
		const auto new_back_idx = std::min(num, MaxElements);

		if (new_back_idx > back_idx) {
			for (auto i = back_idx; i < new_back_idx; i++) {
				data[i] = el;
			}
		}

		back_idx = new_back_idx;
		return (num == new_back_idx);
	}

	constexpr bool resize_for_overwrite(size_t num) {
		back_idx = std::min(num, MaxElements);
		return (num == back_idx);
	}

	// returns MaxElements for failure
	constexpr size_t push_back_for_overwrite() {
		if (back_idx >= MaxElements)
			return MaxElements;

		auto idx = back_idx;

		back_idx++;
		return idx;
	}

	constexpr T pop_back() {
		if (back_idx == 0)
			return T{};

		back_idx--;
		return data[back_idx];
	}

	constexpr size_t size() const {
		return back_idx;
	}

	constexpr size_t max_size() const {
		return MaxElements;
	}

	constexpr size_t available() {
		return max_size() - size();
	}

	constexpr void clear() {
		back_idx = 0;
	}

	constexpr void insert(unsigned index, T d) {
		if (back_idx >= max_size() || index > back_idx)
			return;

		if (index == back_idx) {
			push_back(d);
			return;
		}

		std::move_backward(&data[index], &data[back_idx], &data[back_idx + 1]);

		data[index] = d;
		back_idx++;
	}

	constexpr void erase(const unsigned index) {
		if (back_idx == 0 || index >= back_idx)
			return;

		if (index == back_idx - 1) {
			back_idx--;
			return;
		}

		std::move(&data[index + 1], &data[back_idx], &data[index]);
		back_idx -= 1;
	}

	constexpr void erase(T *first, T *last) {
		const auto count = std::distance(first, last);

		if (back_idx == 0 || count <= 0) {
			return;
		}

		std::move(last, end(), first);

		back_idx -= count;
	}

	using iterator = typename decltype(data)::iterator;

	constexpr auto begin() const {
		return data.begin();
	}

	constexpr auto end() const {
		return begin() + back_idx;
	}

	constexpr auto begin() {
		return data.begin();
	}

	constexpr auto end() {
		return begin() + back_idx;
	}

	constexpr auto span() {
		return std::span{begin(), back_idx};
	}

	constexpr auto span() const {
		return std::span{begin(), back_idx};
	}
};
