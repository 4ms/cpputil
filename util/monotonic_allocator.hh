#pragma once
#include <cstddef>
#include <cstdint>
#include <span>

class MonotonicAllocator {
public:
	// All allocations are aligned up to at least this many bytes
	static constexpr std::size_t DefaultAlign = 8;

	static_assert((DefaultAlign & (DefaultAlign - 1)) == 0, "DefaultAlign must be a power of 2");
	static_assert(DefaultAlign != 0, "DefaultAlign must be non-zero");

	MonotonicAllocator(std::span<uint8_t> arena_)
		: arena(arena_) {
	}

	uint8_t *allocate(std::size_t size, std::size_t align = DefaultAlign) {
		auto aligned_offset = (offset + (align - 1)) & ~(align - 1);
		if (aligned_offset <= arena.size() && (arena.size() - aligned_offset) > size) {
			auto result = &arena[aligned_offset];
			offset = aligned_offset + size;

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
