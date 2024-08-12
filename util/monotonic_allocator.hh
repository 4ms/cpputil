#pragma once
#include <cstdint>
#include <span>

class MonotonicAllocator {
public:
	MonotonicAllocator(std::span<uint8_t> arena_)
		: arena(arena_) {
	}

	uint8_t *allocate(std::size_t size) {
		auto bytesRemaining = arena.size() - offset;
		if (bytesRemaining > size) {
			auto result = &arena[offset];
			offset += size;

			return result;
		} else {
			return nullptr;
		}
	}

	void reset() {
		offset = 0;
	}

	size_t free_space() {
		return arena.size() - offset;
	}

	size_t total_size() {
		return arena.size();
	}

private:
	std::span<uint8_t> arena;
	std::size_t offset{};
};
