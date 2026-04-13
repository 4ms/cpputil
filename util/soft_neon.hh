#pragma once
#include "util/math.hh"

struct float32x4_t {
	float v[4];
	float &operator[](unsigned i) {
		return v[i];
	}
	float operator[](unsigned i) const {
		return v[i];
	}
};

struct float32x2_t {
	float v[2];
	float &operator[](unsigned i) {
		return v[i];
	}
	float operator[](unsigned i) const {
		return v[i];
	}
};

struct int32x2_t {
	int32_t v[2];
	int32_t &operator[](unsigned i) {
		return v[i];
	}
	int32_t operator[](unsigned i) const {
		return v[i];
	}
};

struct uint32x2_t {
	uint32_t v[2];
	uint32_t &operator[](unsigned i) {
		return v[i];
	}
	uint32_t operator[](unsigned i) const {
		return v[i];
	}
};

// Set all lanes to the same value
inline float32x4_t vdupq_n_f32(float a) {
	// out[0] = out[1] = out[2] = out[3] = a;
	return {a, a, a, a};
}

// Set all lanes to the same value
inline float32x2_t vdup_n_f32(float a) {
	// out[0] = out[1] = a;
	return {a, a};
}

// Set all lanes to the same value
inline int32x2_t vdup_n_s32(int32_t a) {
	// out[0] = out[1] = a;
	return {a, a};
}

// Load
inline float32x4_t vld1q_f32(const float a[4]) {
	return {a[0], a[1], a[2], a[3]};
}

// Load
inline float32x2_t vld1_f32(const float a[2]) {
	return {a[0], a[1]};
}

// Load
inline int32x2_t vld1_s32(const int32_t a[2]) {
	return {a[0], a[1]};
}

// Store
inline void vst1_f32(float *ptr, float32x2_t val) {
	ptr[0] = val.v[0]; 
	ptr[1] = val.v[1]; 
}

// Store
inline void vst1_s32(int32_t *ptr, int32x2_t val) {
	ptr[0] = val.v[0]; 
	ptr[1] = val.v[1]; 
}

// Multiplication
inline float32x4_t vmulq_f32(float32x4_t a, float32x4_t b) {
	return {
		a.v[0] * b.v[0],
		a.v[1] * b.v[1],
		a.v[2] * b.v[2],
		a.v[3] * b.v[3],
	};
}

// Multiplication
inline float32x2_t vmul_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] * b.v[0],
		a.v[1] * b.v[1],
	};
}

// Multiplication
inline int32x2_t vmul_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] * b.v[0],
		a.v[1] * b.v[1],
	};
}

// Division
inline float32x2_t vdiv_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] / b.v[0],
		a.v[1] / b.v[1],
	};
}

// Division
inline int32x2_t vdiv_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] / b.v[0],
		a.v[1] / b.v[1],
	};
}

// Multiply-accumulate (dst + a * b)
inline float32x4_t vmlaq_f32(float32x4_t &dst, float32x4_t a, float32x4_t b) {
	dst.v[0] += a.v[0] * b.v[0];
	dst.v[1] += a.v[1] * b.v[1];
	dst.v[2] += a.v[2] * b.v[2];
	dst.v[3] += a.v[3] * b.v[3];

	return {
		dst.v[0],
		dst.v[1],
		dst.v[2],
		dst.v[3],
	};
}

// Multiply-accumulate (dst + a * b)
inline float32x2_t vmla_f32(float32x2_t &dst, float32x2_t a, float32x2_t b) {
	dst.v[0] += a.v[0] * b.v[0];
	dst.v[1] += a.v[1] * b.v[1];

	return {
		dst.v[0],
		dst.v[1],
	};
}

// Multiply-subtract (dst - a * b)
inline float32x2_t vmls_f32(float32x2_t &dst, float32x2_t a, float32x2_t b) {
	dst.v[0] -= a.v[0] * b.v[0];
	dst.v[1] -= a.v[1] * b.v[1];

	return {
		dst.v[0],
		dst.v[1],
	};
}

// Saturation addition
inline int32x2_t vqadd_s32(int32x2_t a, int32x2_t b)
{
    int32x2_t c;
    for (int i = 0; i < 2; i++)
    {
        int32_t tmp = a.v[i] + b.v[i];
        tmp = (tmp > INT16_MAX) ? INT16_MAX : tmp;
        tmp = (tmp < INT16_MIN) ? INT16_MIN : tmp;
        c.v[i] = (int16_t)tmp;
    }

    return c;
}

// Pairwise addition
inline float32x2_t vpadd_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] + a.v[1],
		b.v[0] + b.v[1],
	};
}

// Addition
inline float32x2_t vadd_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] + b.v[0],
		a.v[1] + b.v[1],
	};
}

// Addition
inline int32x2_t vadd_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] + b.v[0],
		a.v[1] + b.v[1],
	};
}

// Subtraction
inline float32x4_t vsubq_f32(float32x4_t a, float32x4_t b) {
	return {
		a.v[0] - b.v[0], 
		a.v[1] - b.v[1], 
		a.v[2] - b.v[2], 
		a.v[3] - b.v[3]
	};
}

// Subtraction
inline float32x2_t vsub_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] - b.v[0],
		a.v[1] - b.v[1],
	};
}

// Subtraction
inline int32x2_t vsub_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] - b.v[0],
		a.v[1] - b.v[1],
	};
}

// Split vectors
inline float32x2_t vget_low_f32(float32x4_t a) {
	return {a.v[0], a.v[1]};
}

// Split vectors
inline float32x2_t vget_high_f32(float32x4_t a) {
	return {a.v[2], a.v[3]};
}

// Extract one element from vector
inline float vget_lane_f32(float32x2_t a, unsigned int lane) {
	return a.v[lane];
}

// Extract one element from vector
inline uint32_t vget_lane_u32(uint32x2_t v, const int lane) {
	return v[lane];
}

// Extract one element from vector
inline int32_t vget_lane_s32(int32x2_t v, const int lane) {
	return v[lane];
}

// Set vector lane
inline void vset_lane_f32(float a, float32x2_t v, const int lane) {
	v[lane] = a;
}

// Maximum
inline float32x4_t vmaxq_f32(float32x4_t a, float32x4_t b) {
	return {
		MathTools::max(a.v[0], b.v[0]),
		MathTools::max(a.v[1], b.v[1]),
		MathTools::max(a.v[2], b.v[2]),
		MathTools::max(a.v[3], b.v[3]),
	};
}

// Maximum
inline float32x2_t vmax_f32(float32x2_t a, float32x2_t b) {
	return {
		MathTools::max(a.v[0], b.v[0]),
		MathTools::max(a.v[1], b.v[1]),
	};
}

// Minimum
inline float32x2_t vmin_f32(float32x2_t a, float32x2_t b) {
	return {
		MathTools::min(a.v[0], b.v[0]),
		MathTools::min(a.v[1], b.v[1]),
	};
}

// Xor
inline int32x2_t veor_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] ^ b.v[0],
		a.v[1] ^ b.v[1],
	};
}

// And
inline int32x2_t vand_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] & b.v[0],
		a.v[1] & b.v[1],
	};
}

// Or
inline int32x2_t vorr_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] | b.v[0],
		a.v[1] | b.v[1],
	};
}

// Greater then
inline uint32x2_t vcgt_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] > b.v[0],
		a.v[1] > b.v[1],
	};
}

// Greater then
inline uint32x2_t vcgt_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] > b.v[0],
		a.v[1] > b.v[1],
	};
}

// Greater then or equal to
inline uint32x2_t vcge_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] >= b.v[0],
		a.v[1] >= b.v[1],
	};
}

// Greater then or equal to
inline uint32x2_t vcge_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] >= b.v[0],
		a.v[1] >= b.v[1],
	};
}

// Less then
inline uint32x2_t vclt_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] < b.v[0],
		a.v[1] < b.v[1],
	};
}

// Less then
inline uint32x2_t vclt_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] < b.v[0],
		a.v[1] < b.v[1],
	};
}

// Less then or equal to
inline uint32x2_t vcle_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] <= b.v[0],
		a.v[1] <= b.v[1],
	};
}

// Less then or equal to
inline uint32x2_t vcle_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] <= b.v[0],
		a.v[1] <= b.v[1],
	};
}

// Equal to
inline uint32x2_t vceq_f32(float32x2_t a, float32x2_t b) {
	return {
		a.v[0] == b.v[0],
		a.v[1] == b.v[1],
	};
}

// Equal to
inline uint32x2_t vceq_s32(int32x2_t a, int32x2_t b) {
	return {
		a.v[0] == b.v[0],
		a.v[1] == b.v[1],
	};
}

// Convert from uint32x2_t to float32x2_t
inline float32x2_t vcvt_f32_u32(uint32x2_t a) {
	return {
		float(a.v[0]),
		float(a.v[1]),
	};
}

// Convert from int32x2_t to float32x2_t
inline float32x2_t vcvt_f32_s32(int32x2_t a) {
	return {
		float(a.v[0]),
		float(a.v[1]),
	};
}

// Convert from float32x2_t to int32x2_t
inline int32x2_t vcvt_s32_f32(float32x2_t a) {
	return {
		int32_t(a.v[0]),
		int32_t(a.v[1]),
	};
}

// Convert from float32x2_t to uint32x2_t
inline uint32x2_t vcvt_u32_f32(float32x2_t a) {
	return {
		uint32_t(a.v[0]),
		uint32_t(a.v[1]),
	};
}

// Reinterpret cast from int to float
inline float32x2_t vreinterpret_f32_s32(int32x2_t a) {
	return {
		(float)a[0],
		(float)a[1],
	};
}

// Reinterpret cast from float to int
inline int32x2_t vreinterpret_s32_f32(float32x2_t a) {
	return {
		(int32_t)a[0],
		(int32_t)a[1],
	};
}

// Reinterpret cast from uint to int
inline int32x2_t vreinterpret_s32_u32(uint32x2_t a) {
	return {
		(int32_t)a[0],
		(int32_t)a[1],
	};
}

// Reinterpret cast from int to uint
inline uint32x2_t vreinterpret_u32_s32(int32x2_t a) {
	return {
		(uint32_t)a[0],
		(uint32_t)a[1],
	};
}

// Reinterpret cast from float to uint
inline uint32x2_t vreinterpret_u32_f32(float32x2_t a) {
	return {
		(uint32_t)a[0],
		(uint32_t)a[1],
	};
}

// Bitwise select
inline float32x2_t vbsl_f32(uint32x2_t a, float32x2_t b, float32x2_t c) {
	return {
		a.v[0] ? b.v[0] : c.v[0],
		a.v[1] ? b.v[1] : c.v[1],
	};
}

// Bitwise select
inline int32x2_t vbsl_s32(uint32x2_t a, int32x2_t b, int32x2_t c) {
	return {
		a.v[0] ? b.v[0] : c.v[0],
		a.v[1] ? b.v[1] : c.v[1],
	};
}

// Reciprocal estimate
inline float32x2_t vrecpe_f32(float32x2_t a) {
	return {
		1.f / a.v[0],
		1.f / a.v[1],
	};
}

// Reciprocal square-root estimate
inline float32x2_t vrsqrte_f32(float32x2_t a) {
	return {
		1.f / std::sqrt(a.v[0]),
		1.f / std::sqrt(a.v[1]),
	};
}

// Absolute value
inline float32x2_t vabs_f32(float32x2_t a) {
	return {
		std::fabs(a.v[0]),
		std::fabs(a.v[1]),
	};
}

// Vector saturating shift left
inline int32x2_t vqshl_n_s32(int32x2_t a, const int n) {
	return {
		(a.v[0] << n) >> n,
		(a.v[1] << n) >> n,
	};
}

// Vector shift right
inline uint32x2_t vshr_n_u32(uint32x2_t a, const int n) {
	return {
		a.v[0] >> n,
		a.v[1] >> n,
	};
}

// Vector shift left
inline int32x2_t vshl_n_s32(int32x2_t a, const int n) {
	return {
		a.v[0] << n,
		a.v[1] << n,
	};
}