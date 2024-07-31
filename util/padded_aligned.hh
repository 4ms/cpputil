#pragma once
#include <cstddef>

// Start and end of PaddedAligned<T, Align> are aligned to Align bytes.
//
// Useful for guarenteeing data does not share a cache line with other data,
// which is a prerequisite for safely invalidating the cache line.
//
template<typename T, size_t Align>
struct PaddedAligned {
	static constexpr size_t data_sz = sizeof(T);
	static constexpr size_t aligned_sz = (data_sz + (Align - 1)) & (~(Align - 1));
	static constexpr size_t pad_sz = aligned_sz - data_sz;

	alignas(Align) T data{};
	char _padding[pad_sz]{};

	operator T &() {
		return data;
	}

	operator T const &() {
		return data;
	}

	T &operator()() {
		return data;
	}
};
