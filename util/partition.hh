#pragma once
#include "util/fixed_vector.hh"
#include <span>

template<unsigned MaxElements>
struct PartitionTwo {

	FixedVector<unsigned, MaxElements> a{};
	FixedVector<unsigned, MaxElements> b{};

	PartitionTwo(std::span<unsigned> vals) {
		unsigned a_sum = 0;
		unsigned b_sum = 0;

		// The part with the smaller sum gets the next element
		for (auto i = 0u; auto val : vals) {
			if (a_sum <= b_sum) {
				a_sum += val;
				a.push_back(i);
			} else {
				b_sum += val;
				b.push_back(i);
			}
			i++;
		}
	}
};
