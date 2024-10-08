#include "doctest.h"
#include "util/math.hh"
#include <cmath>

TEST_CASE("Testing map_value()") {
	SUBCASE("basic float mapping usage") {
		auto mapped = MathTools::map_value(0.25f, 0.f, 1.f, 600.f, 700.f);
		CHECK(mapped == 625.f);
	}
	SUBCASE("map a float from a float range to an integer range") {
		auto mapped = MathTools::map_value(1.25f, 1.f, 2.f, 600, 700);
		CHECK(mapped == 625);
	}
	SUBCASE("map a float from an integer range to a float range") {
		auto mapped = MathTools::map_value(1.25f, 1, 2, -100.f, -200.f);
		CHECK(mapped == -125.f);
	}
	SUBCASE("map an integer from an integer range to a float range") {
		auto mapped = MathTools::map_value(25, 10, 70, -100.f, -200.f);
		CHECK(mapped == -125.f);
	}
	SUBCASE("map an integer from an float range to a integer range") {
		auto mapped = MathTools::map_value(25, 10.0, 70.0, -100, -200);
		CHECK(mapped == -125);
	}
	SUBCASE("map to a zero-width range is always the same number") {
		int low = 10;
		int high = 70;
		for (int i = low; i <= high; i++) {
			auto mapped = MathTools::map_value(i, 10, 70, 666, 666);
			CHECK(mapped == 666);
		}
	}
	SUBCASE("map an out-of-range number to a zero-width range is also the same number") {
		auto mapped = MathTools::map_value(-1000000, 10, 70, 666, 666);
		CHECK(mapped == 666);

		mapped = MathTools::map_value(7000000, 10, 70, 666, 666);
		CHECK(mapped == 666);
	}
	SUBCASE("map an out-of-range number produces an out-of-range result") {
		auto mapped = MathTools::map_value(-1000000, 10L, 70L, 0, 10000);
		CHECK(mapped < 0);

		mapped = MathTools::map_value(7000000, 10L, 70L, 0, 10000);
		CHECK(mapped > 10000);
	}
}

TEST_CASE("Testing is_power_of_2()") {
	CHECK(MathTools::is_power_of_2(1));
	CHECK(MathTools::is_power_of_2(2));
	CHECK(MathTools::is_power_of_2(4));
	CHECK(MathTools::is_power_of_2(8));
	CHECK(MathTools::is_power_of_2(16));
	CHECK(MathTools::is_power_of_2(32));
	//...
	CHECK(MathTools::is_power_of_2(0x10000000));
	CHECK(MathTools::is_power_of_2(0x20000000));
	CHECK(MathTools::is_power_of_2(0x40000000));
	CHECK(MathTools::is_power_of_2(0x80000000));

	CHECK_FALSE(MathTools::is_power_of_2(0));
	CHECK_FALSE(MathTools::is_power_of_2(3));
	CHECK_FALSE(MathTools::is_power_of_2(5));
	CHECK_FALSE(MathTools::is_power_of_2(6));
	CHECK_FALSE(MathTools::is_power_of_2(7));
	//...
	CHECK_FALSE(MathTools::is_power_of_2(0xFFFFFFFF));
	CHECK_FALSE(MathTools::is_power_of_2(0xFFFFFFFE));
	CHECK_FALSE(MathTools::is_power_of_2(0xFFFFFFFD));
	CHECK_FALSE(MathTools::is_power_of_2(0xFFFFFFFC));
	CHECK_FALSE(MathTools::is_power_of_2(0xFFFFFFFB));
	CHECK_FALSE(MathTools::is_power_of_2(0xFFFFFFFA));
	CHECK_FALSE(MathTools::is_power_of_2(0xBFFFFFFF));
	CHECK_FALSE(MathTools::is_power_of_2(0xC0000000));
	CHECK_FALSE(MathTools::is_power_of_2(0xC0000001));
	CHECK_FALSE(MathTools::is_power_of_2(0x7FFFFFFF));
	CHECK_FALSE(MathTools::is_power_of_2(0x80000001));
}

TEST_CASE("Log2 template") {

	SUBCASE("Check normal powers of 2") {
		CHECK(MathTools::Log2<1>::val == 0);
		CHECK(MathTools::Log2<2>::val == 1);
		CHECK(MathTools::Log2<4>::val == 2);
		CHECK(MathTools::Log2<8>::val == 3);
		CHECK(MathTools::Log2<16>::val == 4);
		//...
		CHECK(MathTools::Log2<0x20000000>::val == 29);
		CHECK(MathTools::Log2<0x40000000>::val == 30);
		CHECK(MathTools::Log2<0x80000000>::val == 31);
	}

	SUBCASE("Lowest closest integer (floor) of log2(val) is returned") {
		CHECK((int)(MathTools::Log2<3>::val) == (int)(MathTools::Log2<2>::val));
		CHECK((int)(MathTools::Log2<5>::val) == (int)(MathTools::Log2<4>::val));
		CHECK((int)(MathTools::Log2<6>::val) == (int)(MathTools::Log2<4>::val));
		CHECK((int)(MathTools::Log2<7>::val) == (int)(MathTools::Log2<4>::val));
		CHECK((int)(MathTools::Log2<9>::val) == (int)(MathTools::Log2<8>::val));
		CHECK((int)(MathTools::Log2<15>::val) == (int)(MathTools::Log2<8>::val));
	}
}

TEST_CASE("Testing randomNumber()") {
	SUBCASE("results are in range") {
		int reps = 100;
		while (reps--) {
			auto x = MathTools::randomNumber(-4.f, 4.f);
			CHECK(x >= -4.f);
			CHECK(x <= 4.f);
		}
	}
	SUBCASE("if minNum==maxNum, result will always be minNum") {
		int reps = 100;
		while (reps--) {
			auto x = MathTools::randomNumber(222.f, 222.f);
			CHECK(x == 222.f);
		}
	}
	SUBCASE("results are always < maxNum (unless minNum==maxNum)") {
		SUBCASE("...with floats") {
			int reps = 100;
			int num_high_x = 0;
			while (reps--) {
				double low = 1.0, high = 2.0;
				auto x = MathTools::randomNumber(low, high);
				if (x == doctest::Approx(high).epsilon(0.00001))
					num_high_x++;
			}
			CHECK(num_high_x == 0);
		}

		SUBCASE("...with integers") {
			int reps = 100;
			int num_high_x = 0;
			while (reps--) {
				auto x = MathTools::randomNumber(1UL, 2UL);
				if (x == 2UL)
					num_high_x++;
			}
			CHECK(num_high_x == 0);
		}
	}

	SUBCASE("Roughly equal distribution") {
		int reps = 1000;
		unsigned cnt[3] = {0, 0, 0};
		while (reps--) {

			auto x = MathTools::randomNumber<unsigned int>(1, 11);
			if (x == 1)
				cnt[0]++;
			if (x == 5)
				cnt[1]++;
			if (x == 10)
				cnt[2]++;
		}
		// on average, each one should get 10 counts
		// ..let's say it can be +/-7
		CHECK(cnt[0] > 30);
		CHECK(cnt[1] > 30);
		CHECK(cnt[2] > 30);
		CHECK(cnt[0] < 170);
		CHECK(cnt[1] < 170);
		CHECK(cnt[2] < 170);
	}
}
TEST_CASE("math_tests: wrapping") {
	uint32_t a = 0;
	CHECK(1 == MathTools::wrap<5>(a + 1));
	a++;
	CHECK(2 == MathTools::wrap<5>(a + 1));
	a++;
	CHECK(3 == MathTools::wrap<5>(a + 1));
	a++;
	CHECK(4 == MathTools::wrap<5>(a + 1));
	a++;
	CHECK(0 == MathTools::wrap<5>(a + 1));
}

TEST_CASE("math_tests: ipow_tests") {
	SUBCASE("1 to any power is 1") {
		CHECK(1 == MathTools::ipow(1, 0));
		// CHECK(1 == MathTools::ipow(1, -10)); //FixMe: CRASH!
		CHECK(1 == MathTools::ipow(1, 1));
		CHECK(1 == MathTools::ipow(1, 11111));
	}
	SUBCASE("Powers of 2") {
		CHECK(1 == MathTools::ipow(2, 0));
		CHECK(2 == MathTools::ipow(2, 1));
		CHECK(4 == MathTools::ipow(2, 2));
		CHECK(32768 == MathTools::ipow(2, 15));
		CHECK(65536 == MathTools::ipow(2, 16));
		CHECK(0x80000000U == MathTools::ipow(2, 31));
		SUBCASE("2^32 overflows to 0") {
			CHECK(0x00000000 == MathTools::ipow(2, 32));
		}
	}
}

TEST_CASE("math_tests: sizeof_type_test") {
	uint8_t u8 = 0;
	int8_t i8 = 0;
	uint16_t u16 = 0;
	int16_t i16 = 0;

	CHECK(32767 == MathTools::bipolar_type_range(u16));
	CHECK(32767 == MathTools::bipolar_type_range(i16));

	CHECK(127 == MathTools::bipolar_type_range(u8));
	CHECK(127 == MathTools::bipolar_type_range(i8));
}

TEST_CASE("swap_bytes tests") {
	uint16_t hw1 = 0x1234;
	uint16_t hw2 = 0x0140;
	CHECK(MathTools::swap_bytes_and_combine(hw1, hw2) == 0x34124001);

	uint32_t w = 0x12345678;
	CHECK(MathTools::swap_bytes32(w) == 0x78563412);

	uint16_t hw = 0x9876;
	CHECK(MathTools::swap_bytes16(hw) == 0x7698);
}

TEST_CASE("log2int") {
	SUBCASE("Log(0) is undefined, but in MathTools Log2Int(0) = 0") {
		CHECK(MathTools::log2_floor(0) == 0); // Is this important?
	}

	SUBCASE("Check powers of 2") {
		CHECK(MathTools::log2_floor(1) == 0);
		CHECK(MathTools::log2_floor(2) == 1);
		CHECK(MathTools::log2_floor(4) == 2);
		CHECK(MathTools::log2_floor(8) == 3);
		CHECK(MathTools::log2_floor(16) == 4);
		//...
		CHECK(MathTools::log2_floor(0x20000000) == 29);
		CHECK(MathTools::log2_floor(0x40000000) == 30);
		CHECK(MathTools::log2_floor(0x80000000) == 31);
	}

	SUBCASE("Lowest closest integer (floor) of log2(val) is returned") {
		CHECK(MathTools::log2_floor(3) == MathTools::log2_floor(2));

		CHECK(MathTools::log2_floor(5) == MathTools::log2_floor(4));
		CHECK(MathTools::log2_floor(6) == MathTools::log2_floor(4));
		CHECK(MathTools::log2_floor(7) == MathTools::log2_floor(4));

		CHECK(MathTools::log2_floor(9) == MathTools::log2_floor(8));
		CHECK(MathTools::log2_floor(15) == MathTools::log2_floor(8));
	}
}

TEST_CASE("cos") {
	CHECK(MathTools::cos(0) == doctest::Approx(cosf(0)));
	CHECK(MathTools::cos(M_PI / 2.f) == doctest::Approx(cosf(M_PI / 2.f)));
	CHECK(MathTools::cos(M_PI) == doctest::Approx(cosf(M_PI)));
	CHECK(MathTools::cos(2.f * M_PI) == doctest::Approx(cosf(2.f * M_PI)));
	CHECK(MathTools::cos(1) == doctest::Approx(cosf(1)));
	CHECK(MathTools::cos(999) == doctest::Approx(cosf(999)));
}

TEST_CASE("tan") {
	CHECK(MathTools::tan(0) == doctest::Approx(tanf(0)));
	CHECK(MathTools::tan(M_PI / 2.f) == doctest::Approx(tanf(M_PI / 2.f)));
	CHECK(MathTools::tan(M_PI) == doctest::Approx(tanf(M_PI)));
	CHECK(MathTools::tan(2.f * M_PI) == doctest::Approx(tanf(2.f * M_PI)));
	CHECK(MathTools::tan(1) == doctest::Approx(tanf(1)));
	CHECK(MathTools::tan(999) == doctest::Approx(tanf(999)));
}

TEST_CASE("pow2") {
	float x;
	x = 0;
	CHECK(MathTools::pow2(x) == doctest::Approx(powf(2.f, x)));
	x = 1;
	CHECK(MathTools::pow2(x) == doctest::Approx(powf(2.f, x)));
	x = 2;
	CHECK(MathTools::pow2(x) == doctest::Approx(powf(2.f, x)));
	x = 1.1;
	CHECK(MathTools::pow2(x) == doctest::Approx(powf(2.f, x)));
	x = 0.001;
	CHECK(MathTools::pow2(x) == doctest::Approx(powf(2.f, x)));
	x = 15.324234;
	CHECK(MathTools::pow2(x) == doctest::Approx(powf(2.f, x)));
}

TEST_CASE("hysteresis_gate") {
	float input;
	float output = 0.f;

	input = 0.f;
	output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
	CHECK(output == 0.f);

	input = 0.f;
	output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
	CHECK(output == 0.f);

	THEN("Fully high input --> high output") {
		input = 1.f;
		output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
		CHECK(output == 1.f);

		THEN("input goes fully low --> low output") {
			input = 0.f;
			output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
			CHECK(output == 0.f);

			THEN("input rises to 0.5 --> low output") {
				input = 0.5f;
				output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
				CHECK(output == 0.f);

				THEN("input rises to 0.6001 --> high output") {
					input = 0.6001f;
					output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
					CHECK(output == 1.f);

					THEN("input falls to 0.5 --> high output") {
						input = 0.5f;
						output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
						CHECK(output == 1.f);

						THEN("input falls to 0.3999 --> low output") {
							input = 0.3999f;
							output = MathTools::hysteresis_gate(0.4f, 0.6f, output, input);
							CHECK(output == 0.f);
						}
					}
				}
			}
		}
	}
}

TEST_CASE("hysteresis_feedback") {
	float input;
	float output = 0.f;

	input = 0.f;
	output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
	CHECK(output == 0.f);

	input = 0.f;
	output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
	CHECK(output == 0.f);

	THEN("Fully high input --> high output") {
		input = 1.f;
		output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
		CHECK(output == 1.f);

		THEN("input goes fully low --> low output") {
			input = 0.f;
			output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
			CHECK(output == 0.f);

			THEN("input rises to 0.5 --> low output") {
				input = 0.5f;
				output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
				CHECK(output == 0.f);

				THEN("input rises to 0.6001 --> high output") {
					input = 0.6001f;
					output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
					CHECK(output == 1.f);

					THEN("input falls to 0.5 --> high output") {
						input = 0.5f;
						output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
						CHECK(output == 1.f);

						THEN("input falls to 0.3999 --> low output") {
							input = 0.3999f;
							output = MathTools::hysteresis_feedback(0.1f, 0.5f, output, input);
							CHECK(output == 0.f);
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Plateau") {

	CHECK(MathTools::plateau(2058, 20, 2048) == 0);

	SUBCASE("plateau center 100, width 20 means 90 to 110 = 0") {
		CHECK(MathTools::plateau(90, 20, 100) == 0);
		CHECK(MathTools::plateau(110, 20, 100) == 0);
	}

	SUBCASE("One away from plateau is -1 and +1") {
		CHECK(MathTools::plateau(89, 20, 100) == -1);
		CHECK(MathTools::plateau(111, 20, 100) == 1);
	}

	SUBCASE("Accepts negatives for val") {
		CHECK(MathTools::plateau(0, 20, 100) == -90);
		CHECK(MathTools::plateau(-1, 20, 100) == -91);
		CHECK(MathTools::plateau(-100, 20, 100) == -190);
	}

	SUBCASE("Accepts negatives for center") {
		CHECK(MathTools::plateau(-100, 20, -100) == 0);
		CHECK(MathTools::plateau(-90, 20, -100) == 0);
		CHECK(MathTools::plateau(-110, 20, -100) == 0);
		CHECK(MathTools::plateau(0, 20, -100) == 90);
	}

	SUBCASE("Negative width is broken") {
		// CHECK(MathTools::plateau(100, -20, 100) == -2147483638);
	}
}

TEST_CASE("interpolate<>(ints)") {
	CHECK(MathTools::interpolate<100>(500, 600, 0) == 500);
	CHECK(MathTools::interpolate<100>(500, 600, 50) == 550);
	CHECK(MathTools::interpolate<100>(500, 600, 100) == 600);

	SUBCASE("negative phase continues linearly") {
		CHECK(MathTools::interpolate<100>(500, 600, -100) == 400);
	}
}

TEST_CASE("interpolate(floats)") {
	CHECK(MathTools::interpolate(500, 600, 0.f) == 500);
	CHECK(MathTools::interpolate(500, 600, 0.50) == 550);
	CHECK(MathTools::interpolate(500, 600, 1.00) == 600);
	SUBCASE("negative phase continues linearly") {
		CHECK(MathTools::interpolate(500, 600, -1.f) == 400);
	}
}

TEST_CASE("interpolate3 floats") {
	CHECK(MathTools::interpolate3(500, 600, 0.f) == 500);
	CHECK(MathTools::interpolate3(500, 600, 0.50) == 550);
	CHECK(MathTools::interpolate3(500, 600, 1.00) == 600);
	SUBCASE("negative phase continues linearly") {
		CHECK(MathTools::interpolate3(500, 600, -1.f) == 400);
	}
}

TEST_CASE("array_adj_diff") {
	constexpr std::array<int16_t, 4> x{1, 10, 15, 35};
	std::array d = MathTools::array_adj_diff(x);
	CHECK(d[0] == 9);
	CHECK(d[1] == 5);
	CHECK(d[2] == 20);
	CHECK(d.size() == 3);
}

TEST_CASE("is_finite_fastmath") {
	union F {
		float f;
		uint32_t x;
	};

	[[maybe_unused]] auto check_print = [](F a) {
		(void)a;
		// float f = a.f;
		// printf("%g", a.f);
		// printf("\tfin? %s", MathTools::is_finite_fastmath(f)? "Y": "");
		// printf("\tinf? %s", MathTools::is_inf_fastmath(f)? "Y": "");
		// printf("\n");
	};

	F a;
	a.x = 0xFFFFFFFE; //-nan
	CHECK(!MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.x = 0x7FFFFFFF; //nan
	CHECK(!MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = NAN; //nan
	CHECK(!MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = 1.f / 0.f; //inf
	CHECK(!MathTools::is_finite_fastmath(a.f));
	CHECK(MathTools::is_inf_fastmath(a.f));

	a.x = 0x0; // finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = 3; // finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = 1e-36; // finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = 1e-46; // finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = 1e38; // finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = 1e48; //inf
	CHECK(!MathTools::is_finite_fastmath(a.f));
	CHECK(MathTools::is_inf_fastmath(a.f));

	a.f = 3; //finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = -1e-36; //finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = -1e-46; //finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = -1e38; //finite
	CHECK(MathTools::is_finite_fastmath(a.f));
	CHECK(!MathTools::is_inf_fastmath(a.f));

	a.f = -1e48; //inf
	CHECK(!MathTools::is_finite_fastmath(a.f));
	CHECK(MathTools::is_inf_fastmath(a.f));
}
