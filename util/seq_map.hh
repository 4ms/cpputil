#pragma once
#include <array>
#include <cstddef>

template<typename KeyT, typename ValT, size_t MaxSize>
struct SeqMap {
	std::array<KeyT, MaxSize> keys;
	std::array<ValT, MaxSize> vals;
	size_t sz = 0;

	bool insert(KeyT key, const ValT &val) {
		if (sz >= MaxSize)
			return false;
		keys[sz] = key;
		vals[sz] = val;
		sz++;
		return true;
	}

	bool insert(KeyT key, ValT &&val) {
		if (sz >= MaxSize)
			return false;
		keys[sz] = key;
		vals[sz] = std::move(val);
		sz++;
		return true;
	}

	//TODO: allow move for key on insert?

	ValT *get(KeyT key) {
		for (size_t i = 0; auto k : keys) {
			if (k == key)
				return &vals[i];
			i++;
		}
		return nullptr;
	}

	bool key_exists(KeyT key) {
		for (auto &k : keys) {
			if (k == key)
				return true;
		}
		return false;
	}

	size_t size() {
		return sz;
	}
};
