#pragma once
#include <array>
#include <cstddef>

// Store a stream of input values to be used later
// via data(), or begin()/end().
// Stops when full (does not overwrite)
template<class T, size_t max_size_>
class InputBuffer {
public:
	void put(T item) {
		if (head_ >= max_size_)
			return;

		buf_[head_] = item;
		head_++;
	}

	void reset() {
		head_ = 0;
	}

	T *data() {
		return buf_.data();
	}

	size_t size() {
		return max_size_;
	}

	T *begin() {
		return buf_.begin();
	}

	T *end() {
		return buf_.end();
	}

private:
	std::array<T, max_size_> buf_{};
	size_t head_ = 0;
};

// Fill an OutputBuffer with data(), or begin()/end()
// Then later stream the samples with get()
// get() returns default constructed T when buffer is depleted
template<class T, size_t max_size_>
class OutputBuffer {
public:
	T get(T item) {
		if (head_ >= max_size_)
			return T{};

		auto t = buf_[head_];
		head_++;
		return t;
	}

	void reset() {
		head_ = 0;
	}

	T *data() {
		return buf_.data();
	}

	size_t size() {
		return max_size_;
	}

	T *begin() {
		return buf_.begin();
	}

	T *end() {
		return buf_.end();
	}

private:
	std::array<T, max_size_> buf_{};
	size_t head_ = 0;
};
