#pragma once
#include <array>
#include <cstddef>

namespace cpputil
{

// Hold static memory for a maximum number of objects of a given type T
// Default constructs one when asked to create()
template<typename T, size_t MaxEntries>
struct Pool {

	struct Entry {
		T data;
		bool used = false;
	};
	std::array<Entry, MaxEntries> entries{};

	Pool() = default;

	// Returns ptr to a default-constructed T
	// Returns nullptr if none available
	T *create() {
		for (auto &entry : entries) {
			if (!entry.used) {
				entry.used = true;
				entry.data = T{};
				return &entry.data;
			}
		}

		return nullptr;
	}

	// Returns ptr to a T in an unspecificed state
	// Returns nullptr if none available
	T *create_for_overwrite() {
		for (auto &entry : entries) {
			if (!entry.used) {
				entry.used = true;
				return &entry.data;
			}
		}

		return nullptr;
	}

	// Returns whether item was found
	bool destroy(T *item) {
		for (auto &entry : entries) {
			if (entry.used && item == &entry.data) {
				entry.used = false;
				return true;
			}
		}
		return false;
	}
};
} // namespace cpputil
