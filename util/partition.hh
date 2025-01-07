#pragma once
#include "util/fixed_vector.hh"
#include <span>

// Splits a list of elements into N groups, the sum of each group being roughly equal.
// The **indices** of the original elements are stored resulting `parts`
template<unsigned NumPartitions, unsigned MaxElements, typename T = unsigned>
struct Partition {
	// parts[N] contains the indices of the elements whose sums are roughly equal to other parts[]
	std::array<FixedVector<unsigned, MaxElements>, NumPartitions> parts{};

	Partition() = default;

	Partition(std::span<T> vals) {
		calc_partitions(vals);
	}

	void calc_partitions(std::span<T> vals, std::span<const T> initial_vals = {}) {
		for (auto &part : parts) {
			part.clear();
		}

		struct IdVal {
			T val;
			unsigned id;
		};

		FixedVector<IdVal, MaxElements> ordered;
		for (auto val : vals) {
			ordered.push_back({val, (unsigned)ordered.size()});
		}

		std::ranges::sort(ordered, std::greater{}, &IdVal::val);

		std::array<T, NumPartitions> sums{};

		for (auto i = 0u; auto init_val : initial_vals) {
			sums[i++] = init_val;
		}

		// The part with the smallest sum gets the next element
		for (auto [val, i] : ordered) {
			auto min_sum = std::ranges::min_element(sums);
			auto idx = std::ranges::distance(sums.begin(), min_sum);
			*min_sum += val;
			parts[idx].push_back(i);
		}
	}
};
