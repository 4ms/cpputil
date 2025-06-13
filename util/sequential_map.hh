#pragma once
#include "util/fixed_vector.hh"
#include <type_traits>

template<typename KeyT, typename ValT, size_t N>
class SequentialMap {
public:
	struct PairT {
		KeyT key;
		ValT second;
	};

	using DataT = FixedVector<PairT, N>;
	DataT data;

	const ValT &operator[](const KeyT key) const {
		for (auto &pair : data) {
			if (pair.key == key)
				return pair.second;
		}
		return ValT{};
	}

	ValT &operator[](const KeyT key) {
		for (auto &pair : data) {
			if (pair.key == key)
				return pair.second;
		}

		auto newsize = data.push_back_for_overwrite();
		if (newsize == data.max_size()) {
			// Overflow: silently return last element
			newsize--;
		}
		return data[newsize].second;
	}

	void clear() {
		data.clear();
	}

	bool is_full() const {
		return data.size() == data.max_size();
	}

	auto find(KeyT key) {
		for (auto it = data.begin(); it < data.end(); it++) {
			if (it->key == key)
				return it;
		}
		return data.end();
	}

	bool exists(const KeyT key) const {
		for (auto &pair : data) {
			if (pair.key == key)
				return true;
		}
		return false;
	}

	const auto begin() const {
		return data.begin();
	}
	const auto end() const {
		return data.end();
	}

	auto begin() {
		return data.begin();
	}
	auto end() {
		return data.end();
	}
};
