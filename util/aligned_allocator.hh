#include <cstddef>
#include <cstdlib>

template<typename T, std::size_t Alignment = 64>
class AlignedAllocator {
public:
	using value_type = T;

	AlignedAllocator() noexcept = default;

	template<typename U>
	AlignedAllocator(const AlignedAllocator<U, Alignment> &) noexcept {
	}

	T *allocate(std::size_t n) {
		if (n == 0)
			return nullptr;

		std::size_t size = n * sizeof(T);
		std::size_t aligned_size = ((size + Alignment - 1) / Alignment) * Alignment;

		void *ptr = aligned_alloc(Alignment, aligned_size);
		if (!ptr)
			return nullptr;

		return static_cast<T *>(ptr);
	}

	void deallocate(T *p, std::size_t) noexcept {
		std::free(p);
	}

	template<typename U>
	struct rebind {
		using other = AlignedAllocator<U, Alignment>;
	};

	bool operator==(const AlignedAllocator &) const noexcept {
		return true;
	}
	bool operator!=(const AlignedAllocator &) const noexcept {
		return false;
	}
};
