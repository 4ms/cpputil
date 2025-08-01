#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <optional>

namespace cpputil
{

template<typename T, size_t MaxEntries>
class ThreadSafePool {
private:
	std::array<T, MaxEntries> data;
	std::array<std::atomic<bool>, MaxEntries> used_flags;

public:
	constexpr size_t size() const {
		return MaxEntries;
	}

	ThreadSafePool() = default;

	// Returns index of first available entry
	// Returns nullopt if none available
	std::optional<size_t> create() {
		for (size_t i = 0; auto &used : used_flags) {
			bool expected = false;
			if (used.compare_exchange_strong(expected, true)) {
				return i;
			}
			i++;
		}
		return std::nullopt;
	}

	// Returns whether item was actually destroyed
	bool destroy(size_t idx) {
		if (idx < used_flags.size()) {
			bool expected = true;
			if (used_flags[idx].compare_exchange_strong(expected, false)) {
				return true;
			}
		}
		return false;
	}

	void clear() {
		for (size_t i = 0; i < MaxEntries; i++) {
			data[i] = T{};
			used_flags[i].store(false, std::memory_order_acquire);
		}
	}

	T &operator[](size_t idx) {
		return data[idx];
	}

	T &operator[](size_t idx) const {
		return data[idx];
	}
};

} // namespace cpputil
