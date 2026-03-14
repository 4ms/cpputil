#include "doctest.h"
#include "util/soft_neon.hh"
#include <cmath>

TEST_CASE("float32x4_t: element access") {
	float32x4_t v = {1.f, 2.f, 3.f, 4.f};
	CHECK(v[0] == 1.f);
	CHECK(v[1] == 2.f);
	CHECK(v[2] == 3.f);
	CHECK(v[3] == 4.f);
}

TEST_CASE("float32x2_t: element access") {
	float32x2_t v = {5.f, 6.f};
	CHECK(v[0] == 5.f);
	CHECK(v[1] == 6.f);
}

TEST_CASE("vdupq_n_f32: broadcasts scalar") {
	auto v = vdupq_n_f32(3.14f);
	CHECK(v[0] == 3.14f);
	CHECK(v[1] == 3.14f);
	CHECK(v[2] == 3.14f);
	CHECK(v[3] == 3.14f);
}

TEST_CASE("vld1q_f32: loads array") {
	float arr[4] = {10.f, 20.f, 30.f, 40.f};
	auto v = vld1q_f32(arr);
	CHECK(v[0] == 10.f);
	CHECK(v[1] == 20.f);
	CHECK(v[2] == 30.f);
	CHECK(v[3] == 40.f);
}

TEST_CASE("vmulq_f32: element-wise multiply") {
	float32x4_t a = {1.f, 2.f, 3.f, 4.f};
	float32x4_t b = {2.f, 3.f, 4.f, 5.f};
	auto c = vmulq_f32(a, b);
	CHECK(c[0] == 2.f);
	CHECK(c[1] == 6.f);
	CHECK(c[2] == 12.f);
	CHECK(c[3] == 20.f);
}

TEST_CASE("vmlaq_f32: multiply-accumulate") {
	float32x4_t dst = {0.f, 0.f, 0.f, 0.f};
	float32x4_t a = {1.f, 2.f, 3.f, 4.f};
	float32x4_t b = {1.f, 1.f, 1.f, 1.f};
	auto result = vmlaq_f32(dst, a, b);
	CHECK(result[0] == 1.f);
	CHECK(result[1] == 2.f);
	CHECK(result[2] == 3.f);
	CHECK(result[3] == 4.f);
}

TEST_CASE("vsubq_f32: element-wise subtract") {
	float32x4_t a = {5.f, 6.f, 7.f, 8.f};
	float32x4_t b = {1.f, 2.f, 3.f, 4.f};
	auto c = vsubq_f32(a, b);
	CHECK(c[0] == 4.f);
	CHECK(c[1] == 4.f);
	CHECK(c[2] == 4.f);
	CHECK(c[3] == 4.f);
}

TEST_CASE("vpadd_f32: pairwise add") {
	float32x2_t a = {1.f, 2.f};
	float32x2_t b = {3.f, 4.f};
	auto c = vpadd_f32(a, b);
	CHECK(c[0] == 3.f); // a[0]+a[1]
	CHECK(c[1] == 7.f); // b[0]+b[1]
}

TEST_CASE("vget_low_f32: extracts lower half") {
	float32x4_t v = {1.f, 2.f, 3.f, 4.f};
	auto lo = vget_low_f32(v);
	CHECK(lo[0] == 1.f);
	CHECK(lo[1] == 2.f);
}

TEST_CASE("vget_high_f32: extracts upper half") {
	float32x4_t v = {1.f, 2.f, 3.f, 4.f};
	auto hi = vget_high_f32(v);
	CHECK(hi[0] == 3.f);
	CHECK(hi[1] == 4.f);
}

TEST_CASE("vget_lane_f32: extracts lane") {
	float32x2_t v = {9.f, 11.f};
	CHECK(vget_lane_f32(v, 0) == 9.f);
	CHECK(vget_lane_f32(v, 1) == 11.f);
}

TEST_CASE("vmaxq_f32: element-wise maximum") {
	float32x4_t a = {1.f, 5.f, 3.f, 7.f};
	float32x4_t b = {4.f, 2.f, 6.f, 1.f};
	auto c = vmaxq_f32(a, b);
	CHECK(c[0] == 4.f);
	CHECK(c[1] == 5.f);
	CHECK(c[2] == 6.f);
	CHECK(c[3] == 7.f);
}
