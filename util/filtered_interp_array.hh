#pragma once
#include "util/interp_param.hh"
#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <span>

template<typename FilterT, typename T>
concept IsSimpleFilter = requires(FilterT v, T t) {
	{
		v.add_val(t)
	} -> std::convertible_to<T>;
};

template<size_t N, typename Filter, typename T = float>
requires IsSimpleFilter<Filter, T>
struct FilteredInterpArray {

	void mark_new_data_ready() {
		_new_data_ready = true;
	}

	// If new data is ready, filter each raw reading
	// and set it as the new interpolation target.
	// read_fn: (unsigned i) -> T
	void add_new_readings(auto read_fn) {
		if (_new_data_ready) {
			_new_data_ready = false;

			for (unsigned i = 0; i < N; i++) {
				if (_update_count > 0)
					_interps[i].set_num_updates(_update_count);

				_interps[i].set_new_value(_filters[i].add_val(read_fn(i)));
			}
			_update_count = 0;
		}
	}

	// Advance interpolation and write output values.
	// output_fn: (unsigned i, T val) -> void
	void get_interp_values(auto output_fn) {
		_update_count++;

		bool did_overshoot = (_update_count >= _interps[0].get_num_updates());

		if (did_overshoot) {
			for (unsigned i = 0; i < N; i++) {
				_interps[i].cur_val = _interps[i].target_val;
				output_fn(i, _interps[i].target_val);
			}
		} else {
			for (unsigned i = 0; i < N; i++)
				output_fn(i, _interps[i].next());
		}
	}

	void set_num_updates(unsigned n) {
		for (auto &interp : _interps)
			interp.set_num_updates(n);
	}

	static constexpr size_t size() {
		return N;
	}

private:
	std::array<InterpParamVariable<T>, N> _interps{};
	std::array<Filter, N> _filters{};
	std::atomic<bool> _new_data_ready{false};
	unsigned _update_count{0};
};
