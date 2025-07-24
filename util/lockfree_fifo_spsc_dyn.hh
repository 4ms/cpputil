#pragma once
#include <atomic>
#include <optional>
#include <vector>

// Thread-safe Lock-free Single-Producer Single-Consumer
// Allocates a vector on construction. Re-allocates (resizes) on resize()
// For cheaply copied types
// Will not overwrite -- put() returns false if buffer is full, and get() returns nullopt if buffer is empty.
// Provide the buffer size in the constructor.
//
// Edge-case: max_size cannot be more than half the largest integer representable by size_t
template<class T>
class LockFreeFifoSpscDyn {
public:
	LockFreeFifoSpscDyn(size_t max_size)
		: max_size_{max_size}
		, head_{0}
		, tail_{0}
		, buf_(max_size_) {
	}

	// Initialize with an offset between get and put
	// Useful, for example, if get() and put() happen at the same rates
	// and you want to provide a fixed delay
	LockFreeFifoSpscDyn(size_t max_size, size_t head)
		: max_size_{max_size}
		, head_{head}
		, tail_{0}
		, buf_(max_size_) {
	}

	// Resets and resizes the vector. All contents are lost.
	// If max_size does not change, do nothing.
	void resize(size_t new_max_size) {
		if (new_max_size != max_size_) {
			reset();

			if (new_max_size < max_size_) {
				// Deallocate the buffer to force shrink to fit
				std::vector<T>().swap(buf_);
			}

			buf_.resize(new_max_size);

			max_size_ = new_max_size;
		}
	}

	//
	// Producer
	//
	bool put(T item) {
		auto tmp_head = head_.load(std::memory_order_relaxed);

		if ((tmp_head - tail_.load(std::memory_order_acquire)) == max_size_)
			return false;

		buf_[tmp_head % max_size_] = item;
		tmp_head++;
		std::atomic_signal_fence(std::memory_order_release);
		head_.store(tmp_head, std::memory_order_release);
		return true;
	}

	// Number of elements available to write
	size_t num_free() const {
		return max_size_ - (head_.load(std::memory_order_relaxed) - tail_.load(std::memory_order_acquire));
	}

	// Not safe, but useful for debugging:
	// size_t head() const {
	// 	return head_ & SIZE_MASK;
	// }
	// size_t tail() const {
	// 	return tail_ & SIZE_MASK;
	// }

	bool full() const {
		return num_free() == 0;
	}

	//
	// Consumer
	//
	std::optional<T> get() {
		auto tmp_tail = tail_.load(std::memory_order_relaxed);

		if (tmp_tail == head_.load(std::memory_order_relaxed)) {
			return std::nullopt;
		}

		auto item = buf_[tmp_tail % max_size_];
		tmp_tail++;
		std::atomic_signal_fence(std::memory_order_release);
		tail_.store(tmp_tail, std::memory_order_release);
		return item;
	}

	bool get_move(T &t) {
		auto tmp_tail = tail_.load(std::memory_order_relaxed);

		if (tmp_tail == head_.load(std::memory_order_relaxed)) {
			return false;
		}

		t = std::move(buf_[tmp_tail % max_size_]);
		tmp_tail++;
		std::atomic_signal_fence(std::memory_order_release);
		tail_.store(tmp_tail, std::memory_order_release);
		return true;
	}

	T get_or_default() {
		return get().value_or(T{});
	}

	void remove_first() {
		auto tmp_tail = tail_.load(std::memory_order_relaxed);

		if (tmp_tail != head_.load(std::memory_order_relaxed))
			tail_.store(++tmp_tail, std::memory_order_release);
	}

	// Number of elements available to read
	size_t num_filled() const {
		return head_.load(std::memory_order_acquire) - tail_.load(std::memory_order_relaxed);
	}

	bool empty() const {
		return num_filled() == 0;
	}

	void set_read_pos(size_t pos) {
		tail_.store(pos, std::memory_order_release);
	}

	// Sets read head `offset` samples before write head, if possible
	void set_read_offset(size_t offset) {
		auto head = head_.load(std::memory_order_acquire);
		if (offset <= head)
			tail_.store(head - offset, std::memory_order_release);
	}

	void set_write_pos(size_t pos) {
		head_.store(pos, std::memory_order_release);
	}

	// Reset can be done by consumer or producer
	void reset() {
		tail_.store(head_.load(std::memory_order_relaxed), std::memory_order_relaxed);
	}

	// Maximum number of elements it can hold
	constexpr size_t max_size() const {
		return max_size_;
	}

private:
	size_t max_size_;
	std::atomic<size_t> head_ = 0;
	std::atomic<size_t> tail_ = 0;
	std::vector<T> buf_;
};
