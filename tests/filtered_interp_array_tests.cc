#include "doctest.h"
#include "util/filter.hh"
#include "util/filtered_interp_array.hh"

// NoFilter<float> passes values through unchanged, giving exact predictable outputs.
using TestArray1 = FilteredInterpArray<1, NoFilter<float>>;
using TestArray2 = FilteredInterpArray<2, NoFilter<float>>;

TEST_CASE("filtered_interp_array: outputs_zero_before_mark") {
	TestArray1 arr;
	arr.set_num_updates(4);
	float out = -1.f;
	arr.add_new_readings([](unsigned) { return 99.f; });
	arr.get_interp_values([&](unsigned, float v) { out = v; });
	CHECK(out == 0.f);
}

TEST_CASE("filtered_interp_array: no_change_without_mark") {
	TestArray1 arr;
	arr.set_num_updates(4);
	float out = -1.f;
	for (int i = 0; i < 8; i++) {
		arr.add_new_readings([](unsigned) { return 8.f; });
		arr.get_interp_values([&](unsigned, float v) { out = v; });
		CHECK(out == 0.f);
	}
}

TEST_CASE("filtered_interp_array: basic_interpolation") {
	TestArray1 arr;
	arr.set_num_updates(4);
	arr.mark_new_data_ready();
	float out = 0.f;
	auto output_fn = [&](unsigned, float v) { out = v; };
	auto read_fn = [](unsigned) { return 8.f; };

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out == doctest::Approx(2.f));

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out == doctest::Approx(4.f));

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out == doctest::Approx(6.f));

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out == 8.f); // snap: exact match, no floating-point accumulation
}

TEST_CASE("filtered_interp_array: snaps_to_exact_target") {
	// Verify that the final snap writes target_val directly rather than
	// relying on accumulated floating-point steps.
	TestArray1 arr;
	arr.set_num_updates(3);
	arr.mark_new_data_ready();
	float out = 0.f;
	for (int i = 0; i < 3; i++) {
		arr.add_new_readings([](unsigned) { return 1.f / 3.f; });
		arr.get_interp_values([&](unsigned, float v) { out = v; });
	}
	CHECK(out == 1.f / 3.f);
}

TEST_CASE("filtered_interp_array: holds_target_after_snap") {
	TestArray1 arr;
	arr.set_num_updates(4);
	arr.mark_new_data_ready();
	float out = 0.f;
	auto output_fn = [&](unsigned, float v) { out = v; };
	auto read_fn = [](unsigned) { return 8.f; };

	for (int i = 0; i < 4; i++) {
		arr.add_new_readings(read_fn);
		arr.get_interp_values(output_fn);
	}
	CHECK(out == 8.f);

	for (int i = 0; i < 8; i++) {
		arr.add_new_readings(read_fn);
		arr.get_interp_values(output_fn);
		CHECK(out == 8.f);
	}
}

TEST_CASE("filtered_interp_array: multi_channel_interpolates_independently") {
	TestArray2 arr;
	arr.set_num_updates(4);
	arr.mark_new_data_ready();
	float out0 = 0.f, out1 = 0.f;
	auto output_fn = [&](unsigned i, float v) {
		if (i == 0)
			out0 = v;
		else
			out1 = v;
	};
	// ch0 target=4 (step=1), ch1 target=8 (step=2)
	auto read_fn = [](unsigned i) { return i == 0 ? 4.f : 8.f; };

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out0 == doctest::Approx(1.f));
	CHECK(out1 == doctest::Approx(2.f));

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out0 == doctest::Approx(2.f));
	CHECK(out1 == doctest::Approx(4.f));

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out0 == doctest::Approx(3.f));
	CHECK(out1 == doctest::Approx(6.f));

	arr.add_new_readings(read_fn);
	arr.get_interp_values(output_fn);
	CHECK(out0 == 4.f); // snap
	CHECK(out1 == 8.f); // snap
}

TEST_CASE("filtered_interp_array: adapts_num_updates_to_measured_period") {
	// The ISR fires every 2 frames, but set_num_updates(4) initially.
	// Cycle 1: step_size uses old num_updates=4 (transient).
	// Cycle 2: step_size uses old num_updates=4 still (one-cycle lag), but
	//          num_updates is updated to 2 for the overshoot check.
	// Cycle 3: step_size now uses num_updates=2, giving correct midpoint.
	TestArray1 arr;
	arr.set_num_updates(4);
	float out = 0.f;
	auto output_fn = [&](unsigned, float v) { out = v; };

	// --- Cycle 1: ISR fires, target=8, num_updates=4 ---
	arr.mark_new_data_ready();
	arr.add_new_readings([](unsigned) { return 8.f; }); // count=0 -> skip set_num_updates
	arr.get_interp_values(output_fn);					// count=1, step=8/4=2, out=2
	CHECK(out == doctest::Approx(2.f));
	arr.add_new_readings([](unsigned) { return 8.f; });
	arr.get_interp_values(output_fn); // count=2, out=4
	CHECK(out == doctest::Approx(4.f));
	// ISR fires after 2 frames; count=2 at entry to next cycle

	// --- Cycle 2: ISR fires, target=16, transient cycle ---
	arr.mark_new_data_ready();
	arr.add_new_readings([](unsigned) { return 16.f; }); // count=2 -> set_num_updates(2), step=(16-4)/4=3
	arr.get_interp_values(output_fn);					 // count=1, out=4+3=7
	CHECK(out == doctest::Approx(7.f));					 // transient: step used old num_updates=4
	arr.add_new_readings([](unsigned) { return 16.f; });
	arr.get_interp_values(output_fn); // count=2 >= num_updates=2: snap=16
	CHECK(out == 16.f);

	// --- Cycle 3: ISR fires, target=0, adapted cycle ---
	// num_updates is now 2; set_new_value will use it for step_size
	arr.mark_new_data_ready();
	arr.add_new_readings([](unsigned) { return 0.f; }); // count=2 -> set_num_updates(2), step=(0-16)/2=-8
	arr.get_interp_values(output_fn);					// count=1, out=16-8=8
	CHECK(out == doctest::Approx(8.f));					// correctly at the midpoint
	arr.add_new_readings([](unsigned) { return 0.f; });
	arr.get_interp_values(output_fn); // count=2 >= num_updates=2: snap=0
	CHECK(out == 0.f);
}

TEST_CASE("filtered_interp_array: mark_multiple_times_is_processed_once") {
	TestArray1 arr;
	arr.set_num_updates(4);
	arr.mark_new_data_ready();
	arr.mark_new_data_ready(); // redundant: flag is already set

	int call_count = 0;
	float out = 0.f;
	arr.add_new_readings([](unsigned) { return 8.f; });
	arr.get_interp_values([&](unsigned, float v) {
		out = v;
		call_count++;
	});

	CHECK(call_count == 1);				// output_fn called exactly once (1 channel)
	CHECK(out == doctest::Approx(2.f)); // normal step, not doubled
}

TEST_CASE("filtered_interp_array: second_mark_before_add_uses_latest_reading") {
	// If mark fires multiple times before add_new_readings, the flag is still
	// processed exactly once and read_fn is called at add_new_readings time.
	TestArray1 arr;
	arr.set_num_updates(4);
	float out = 0.f;
	int call_count = 0;
	auto output_fn = [&](unsigned, float v) {
		out = v;
		call_count++;
	};

	arr.mark_new_data_ready();
	arr.add_new_readings([](unsigned) { return 4.f; });
	arr.get_interp_values(output_fn);
	CHECK(out == doctest::Approx(1.f)); // stepped once toward 4.f

	call_count = 0;
	arr.mark_new_data_ready();
	arr.mark_new_data_ready(); // redundant second mark before next add
	// add_new_readings reads the current value from read_fn (8.f), processed once.
	// _update_count is 1 here, so set_num_updates(1) fires -> snap on next get.
	arr.add_new_readings([](unsigned) { return 8.f; });
	arr.get_interp_values(output_fn);
	CHECK(call_count == 1); // flag processed exactly once (1 channel, 1 call)
	CHECK(out == 8.f);		// snapped to 8.f (num_updates adapted to 1)
}

TEST_CASE("filtered_interp_array: size_returns_N") {
	static_assert(FilteredInterpArray<1, NoFilter<float>>::size() == 1);
	static_assert(FilteredInterpArray<3, NoFilter<float>>::size() == 3);
	static_assert(FilteredInterpArray<10, NoFilter<float>>::size() == 10);
}
