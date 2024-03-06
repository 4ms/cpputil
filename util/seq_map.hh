#pragma once
#include <array>
#include <cstddef>

template<typename KeyT, typename ValT, size_t MaxSize>
struct SeqMap {
	std::array<KeyT, MaxSize> keys;
	std::array<ValT, MaxSize> vals;
	size_t sz = 0;
	size_t oldest = 0;

	bool insert(KeyT key, const ValT &val) {
		if (sz >= MaxSize)
			return false;
		keys[sz] = key;
		vals[sz] = val;
		sz++;
		return true;
	}

	ValT *overwrite(KeyT key, const ValT &val) {
		// overwrite existing key
		for (size_t i = 0; auto k : keys) {
			if (i >= sz)
				break;
			if (k == key) {
				vals[i] = val;
				return &vals[i];
			}
			i++;
		}

		// Overwrite oldest value if full, otherwise append
		size_t index = sz;
		if (sz >= MaxSize) {
			index = oldest;
			oldest++;
			if (oldest >= MaxSize)
				oldest = 0;
		} else
			sz++;

		keys[index] = key;
		vals[index] = val;
		return &vals[index];
	}

	ValT *get(KeyT key) {
		for (size_t i = 0; auto k : keys) {
			if (i >= sz)
				return nullptr;
			if (k == key)
				return &vals[i];
			i++;
		}
		return nullptr;
	}

	void remove_last() {
		if (sz > 0)
			sz--;
	}

	bool key_exists(KeyT key) const {
		for (size_t i = 0; auto &k : keys) {
			if (i >= sz)
				return false;
			if (k == key)
				return true;
			i++;
		}
		return false;
	}

	size_t size() const {
		return sz;
	}
};
