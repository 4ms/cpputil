#pragma once

#ifdef CORE_CA7
#include <arm_neon.h>
#else
#include "soft_neon.hh"
#endif

// This is ported from VCV Rack Vector.hpp and adapted to be used for two-channel NEON vector

template <typename TYPE, int SIZE>
struct StereoValue;


template <>
struct StereoValue<float, 2> {
	using type = float;
	constexpr static int size = 2;

	union {
		float32x2_t __attribute__((aligned(16))) v;
		float __attribute__((aligned(16))) s[2];
	};

	/** Constructs an uninitialized vector. */
	StereoValue() = default;

	/** Constructs a vector from a native `float32x2_t` type. */
	StereoValue(float32x2_t v) : v(v) {}

	/** Constructs a vector with all elements set to `x`. */
	StereoValue(float x) {
		v = vdup_n_f32(x);
	}

	/** Constructs a vector from two scalars. */
	StereoValue(float x1, float x2) {
		float __attribute__((aligned(16))) data[2] = { x1, x2 };
	
		v = vld1_f32(data);
	}

	/** Returns a vector with all 0 bits. */
	static StereoValue zero() {
		return StereoValue(vdup_n_f32(0.f));
	}

	/** Returns a vector with all 1 bits. */
	static StereoValue mask() {
		return StereoValue(vcvt_f32_u32(vceq_s32(vdup_n_s32(0), vdup_n_s32(0))));
	}

	/** Reads an array of 2 values. */
	static StereoValue load(const float* x) {
		return StereoValue(vld1_f32(x));
	}

	/** Writes an array of 2 values. */
	void store(float* x) {
		vst1_f32(x, v);
	}

	/** Accessing vector elements individually is slow and defeats the purpose of vectorizing.
	However, this operator is convenient when writing simple serial code in a non-bottlenecked section.
	*/
	float& operator[](int i) {
		return s[i];
	}
	const float& operator[](int i) const {
		return s[i];
	}

	// Conversions
	StereoValue(StereoValue<int32_t, 2> a);
	// Casts
	static StereoValue cast(StereoValue<int32_t, 2> a);
};


template <>
struct StereoValue<int32_t, 2> {
	using type = int32_t;
	constexpr static int size = 2;

	union {
		int32x2_t __attribute__((aligned(16))) v;
		int32_t __attribute__((aligned(16))) s[2];
	};

	StereoValue() = default;
	StereoValue(int32x2_t v) : v(v) {}
	StereoValue(int32_t x) {
		v = vdup_n_s32(x);
	}
	StereoValue(int32_t x1, int32_t x2) {
		int32_t __attribute__((aligned(16))) data[2] = { x1, x2 };
	
		v = vld1_s32(data);
	}
	static StereoValue zero() {
		return StereoValue(vdup_n_s32(0));
	}
	static StereoValue mask() {
		return StereoValue(vreinterpret_s32_u32(vceq_s32(vdup_n_s32(0), vdup_n_s32(0))));
	}
	static StereoValue load(const int32_t* x) {
		return StereoValue(vld1_s32(x));
	}
	void store(int32_t* x) {
		vst1_s32(x, v);
	}
	int32_t& operator[](int i) {
		return s[i];
	}
	const int32_t& operator[](int i) const {
		return s[i];
	}
	StereoValue(StereoValue<float, 2> a);
	static StereoValue cast(StereoValue<float, 2> a);
};


// Conversions and casts


inline StereoValue<float, 2>::StereoValue(StereoValue<int32_t, 2> a) {
	v = vcvt_f32_s32(a.v);
}

inline StereoValue<int32_t, 2>::StereoValue(StereoValue<float, 2> a) {
	v = vcvt_s32_f32(a.v);
}

inline StereoValue<float, 2> StereoValue<float, 2>::cast(StereoValue<int32_t, 2> a) {
	return StereoValue(vcvt_f32_s32(a.v));
}

inline StereoValue<int32_t, 2> StereoValue<int32_t, 2>::cast(StereoValue<float, 2> a) {
	return StereoValue(vcvt_s32_f32(a.v));
}


// Operator overloads


/** `a @ b` */
#define DECLARE_VECTOR_OPERATOR_INFIX(t, s, operator, func) \
	inline StereoValue<t, s> operator(const StereoValue<t, s>& a, const StereoValue<t, s>& b) { \
		return StereoValue<t, s>(func(a.v, b.v)); \
	}

/** `a @= b` */
#define DECLARE_VECTOR_OPERATOR_INCREMENT(t, s, operator, opfunc) \
	inline StereoValue<t, s>& operator(StereoValue<t, s>& a, const StereoValue<t, s>& b) { \
		return a = opfunc(a, b); \
	}

DECLARE_VECTOR_OPERATOR_INFIX(float, 2, operator+, vadd_f32)
DECLARE_VECTOR_OPERATOR_INFIX(int32_t, 2, operator+, vadd_s32)

DECLARE_VECTOR_OPERATOR_INFIX(float, 2, operator-, vsub_f32)
DECLARE_VECTOR_OPERATOR_INFIX(int32_t, 2, operator-, vsub_s32)

DECLARE_VECTOR_OPERATOR_INFIX(float, 2, operator*, vmul_f32)
DECLARE_VECTOR_OPERATOR_INFIX(int32_t, 2, operator*, vmul_s32)

inline StereoValue<float, 2> operator/(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(vmul_f32(a.v, vrecpe_f32(b.v)));
}
inline StereoValue<int32_t, 2> operator/(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(StereoValue<float, 2>(vcvt_f32_s32(a.v)) * StereoValue<float, 2>(vcvt_f32_s32(a.v)));
}

/* Use these to apply logic, bit masks, and conditions to elements.
Boolean operators on vectors give 0x00000000 for false and 0xffffffff for true, for each vector element.

Examples:

Subtract 1 from value if greater than or equal to 1.

	x -= (x >= 1.f) & 1.f;
*/

/*
inline StereoValue<float, 2> operator^(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return vcvt_s32_f32(a.v) ^ vcvt_s32_f32(b.v);
}
*/
DECLARE_VECTOR_OPERATOR_INFIX(int32_t, 2, operator^, veor_s32)

/*
inline StereoValue<float, 2> operator&(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return vcvt_s32_f32(a.v) & vcvt_s32_f32(b.v);
}
*/
DECLARE_VECTOR_OPERATOR_INFIX(int32_t, 2, operator&, vand_s32)

/*
inline StereoValue<float, 2> operator|(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return vcvt_s32_f32(a.v) | vcvt_s32_f32(b.v);
}
*/
DECLARE_VECTOR_OPERATOR_INFIX(int32_t, 2, operator|, vorr_s32)

DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator+=, operator+)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator+=, operator+)

DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator-=, operator-)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator-=, operator-)

DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator*=, operator*)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator*=, operator*)

DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator/=, operator/)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator/=, operator/)

//DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator^=, operator^)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator^=, operator^)

//DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator&=, operator&)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator&=, operator&)

//DECLARE_VECTOR_OPERATOR_INCREMENT(float, 2, operator|=, operator|)
DECLARE_VECTOR_OPERATOR_INCREMENT(int32_t, 2, operator|=, operator|)

inline StereoValue<float, 2> operator==(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(vcvt_f32_u32(vceq_f32(a.v, b.v)));
}
inline StereoValue<int32_t, 2> operator==(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(vreinterpret_s32_u32(vceq_s32(a.v, b.v)));
}

inline StereoValue<float, 2> operator>=(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(vcvt_f32_u32(vcge_f32(a.v, b.v)));
}
inline StereoValue<int32_t, 2> operator>=(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(vreinterpret_s32_u32(vcge_s32(a.v, b.v)));
}

inline StereoValue<float, 2> operator>(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(vcvt_f32_u32(vcgt_f32(a.v, b.v)));
}
inline StereoValue<int32_t, 2> operator>(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(vreinterpret_s32_u32(vcgt_s32(a.v, b.v)));
}

inline StereoValue<float, 2> operator<=(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(vcvt_f32_u32(vcle_f32(a.v, b.v)));
}
inline StereoValue<int32_t, 2> operator<=(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(vreinterpret_s32_u32(vcle_s32(a.v, b.v)));
}

inline StereoValue<float, 2> operator<(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(vcvt_f32_u32(vclt_f32(a.v, b.v)));
}
inline StereoValue<int32_t, 2> operator<(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(vreinterpret_s32_u32(vclt_s32(a.v, b.v)));
}

inline StereoValue<float, 2> operator!=(const StereoValue<float, 2>& a, const StereoValue<float, 2>& b) {
	return StereoValue<float, 2>(a.v == b.v) ^ StereoValue<float, 2>::mask();
}
inline StereoValue<int32_t, 2> operator!=(const StereoValue<int32_t, 2>& a, const StereoValue<int32_t, 2>& b) {
	return StereoValue<int32_t, 2>(a.v == b.v) ^ StereoValue<int32_t, 2>::mask();
}

/** `+a` */
inline StereoValue<float, 2> operator+(const StereoValue<float, 2>& a) {
	return a;
}
inline StereoValue<int32_t, 2> operator+(const StereoValue<int32_t, 2>& a) {
	return a;
}

/** `-a` */
inline StereoValue<float, 2> operator-(const StereoValue<float, 2>& a) {
	return 0.f - a;
}
inline StereoValue<int32_t, 2> operator-(const StereoValue<int32_t, 2>& a) {
	return 0 - a;
}

/** `++a` */
inline StereoValue<float, 2>& operator++(StereoValue<float, 2>& a) {
	return a += 1.f;
}
inline StereoValue<int32_t, 2>& operator++(StereoValue<int32_t, 2>& a) {
	return a += 1;
}

/** `--a` */
inline StereoValue<float, 2>& operator--(StereoValue<float, 2>& a) {
	return a -= 1.f;
}
inline StereoValue<int32_t, 2>& operator--(StereoValue<int32_t, 2>& a) {
	return a -= 1;
}

/** `a++` */
inline StereoValue<float, 2> operator++(StereoValue<float, 2>& a, int) {
	StereoValue<float, 2> b = a;
	++a;
	return b;
}
inline StereoValue<int32_t, 2> operator++(StereoValue<int32_t, 2>& a, int) {
	StereoValue<int32_t, 2> b = a;
	++a;
	return b;
}

/** `a--` */
inline StereoValue<float, 2> operator--(StereoValue<float, 2>& a, int) {
	StereoValue<float, 2> b = a;
	--a;
	return b;
}
inline StereoValue<int32_t, 2> operator--(StereoValue<int32_t, 2>& a, int) {
	StereoValue<int32_t, 2> b = a;
	--a;
	return b;
}

/** `~a` */
inline StereoValue<float, 2> operator~(const StereoValue<float, 2>& a) {
	return a ^ StereoValue<float, 2>::mask();
}
inline StereoValue<int32_t, 2> operator~(const StereoValue<int32_t, 2>& a) {
	return a ^ StereoValue<int32_t, 2>::mask();
}

/** `a << b` */
/*
inline StereoValue<int32_t, 2> operator<<(const StereoValue<int32_t, 2>& a, const int& b) {
	return StereoValue<int32_t, 2>(_mm_sll_epi32(a.v, _mm_cvtsi32_si128(b)));
}
*/

/** `a >> b` */
/*
inline StereoValue<int32_t, 2> operator>>(const StereoValue<int32_t, 2>& a, const int& b) {
	return StereoValue<int32_t, 2>(_mm_srl_epi32(a.v, _mm_cvtsi32_si128(b)));
}
*/

// Typedefs


using float_2 = StereoValue<float, 2>;
using int32_2 = StereoValue<int32_t, 2>;
