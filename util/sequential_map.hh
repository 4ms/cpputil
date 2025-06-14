#pragma once
#include "util/fixed_vector.hh"

template<typename KeyT, typename ValT, size_t N>
class SequentialMap {
public:
	struct PairT {
		KeyT first;
		ValT second;
	};

	using DataT = FixedVector<PairT, N>;
	DataT data;

	const ValT &operator[](const KeyT key) const {
		for (auto &pair : data) {
			if (pair.first == key)
				return pair.second;
		}
		return ValT{};
	}

	ValT &operator[](const KeyT key) {
		for (auto &pair : data) {
			if (pair.first == key) {
				return pair.second;
			}
		}

		auto newsize = data.push_back_for_overwrite();
		if (newsize == data.max_size()) {
			// Overflow: silently return last element
			newsize--;
		}
		data[newsize].first = key;
		return data[newsize].second;
	}

	// TODO: unit tests for this
	std::pair<typename DataT::iterator, bool> insert(const PairT &entry) {
		for (auto it = data.begin(); it < data.end(); it++) {
			if (it->first == entry.first) {
				return {it, false};
			}
		}

		auto newsize = data.push_back_for_overwrite();
		if (newsize == data.max_size()) {
			return {data.end(), false};
		}

		data[newsize].first = entry.first;
		return {data.end() - 1, true};
	}

	size_t size() const {
		return data.size();
	}

	size_t max_size() const {
		return data.max_size();
	}

	void clear() {
		data.clear();
	}

	bool empty() const {
		return data.size() == 0;
	}

	bool is_full() const {
		return data.size() == data.max_size();
	}

	// TODO: unit tests for this
	auto find(const KeyT &key) {
		for (auto it = data.begin(); it < data.end(); it++) {
			if (it->first == key)
				return it;
		}
		return data.end();
	}

	// TODO: unit tests for this
	auto find(const KeyT &key) const {
		for (auto it = data.begin(); it < data.end(); it++) {
			if (it->first == key)
				return it;
		}
		return data.end();
	}

	// Unlike std::map, this will invalidate iterators
	// to elements before the erased element
	size_t erase(const KeyT &key) {
		if (auto it = find(key); it != data.end()) {
			data.erase(it, it + 1);
			return 1;
		}
		return 0;
	}

	bool contains(const KeyT &key) const {
		for (auto &pair : data) {
			if (pair.first == key)
				return true;
		}
		return false;
	}

	auto begin() const {
		return data.begin();
	}
	auto end() const {
		return data.end();
	}

	auto begin() {
		return data.begin();
	}
	auto end() {
		return data.end();
	}
};
