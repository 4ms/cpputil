#pragma once
#include "zip.hh"
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
		for (auto [k, v] : zip(keys, vals)) {
			if (k == key) {
				v = val;
				return &v;
			}
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

	bool key_exists(KeyT key) {
		for (auto i = 0u; auto &k : keys) {
			if (k == key)
				return true;
			if (i++ >= sz)
				return false;
		}
		return false;
	}

	size_t size() {
		return sz;
	}
};
