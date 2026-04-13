#pragma once

#include "stereo_value.hh"
#include <algorithm>
#include <complex>
#include <string>
#include <cmath>

// Functions based on instructions
//
// This is a mash-up of functions from VCV Rack and CMSIS DSP,
// adapted to be used with our stereo values.
// Some translations from VCV Rack SIMD to NEON thanks to:
// https://android.googlesource.com/platform/external/pffft/+/3e2ecbdc249e8d9d42923f3955ca88140e5967fc/sse2neon.h
    
#define FAST_MATH_TABLE_SIZE 512

const float sinTable513[FAST_MATH_TABLE_SIZE + 1] = {
   0.00000000f, 0.01227154f, 0.02454123f, 0.03680722f, 0.04906767f, 0.06132074f,
   0.07356456f, 0.08579731f, 0.09801714f, 0.11022221f, 0.12241068f, 0.13458071f,
   0.14673047f, 0.15885814f, 0.17096189f, 0.18303989f, 0.19509032f, 0.20711138f,
   0.21910124f, 0.23105811f, 0.24298018f, 0.25486566f, 0.26671276f, 0.27851969f,
   0.29028468f, 0.30200595f, 0.31368174f, 0.32531029f, 0.33688985f, 0.34841868f,
   0.35989504f, 0.37131719f, 0.38268343f, 0.39399204f, 0.40524131f, 0.41642956f,
   0.42755509f, 0.43861624f, 0.44961133f, 0.46053871f, 0.47139674f, 0.48218377f,
   0.49289819f, 0.50353838f, 0.51410274f, 0.52458968f, 0.53499762f, 0.54532499f,
   0.55557023f, 0.56573181f, 0.57580819f, 0.58579786f, 0.59569930f, 0.60551104f,
   0.61523159f, 0.62485949f, 0.63439328f, 0.64383154f, 0.65317284f, 0.66241578f,
   0.67155895f, 0.68060100f, 0.68954054f, 0.69837625f, 0.70710678f, 0.71573083f,
   0.72424708f, 0.73265427f, 0.74095113f, 0.74913639f, 0.75720885f, 0.76516727f,
   0.77301045f, 0.78073723f, 0.78834643f, 0.79583690f, 0.80320753f, 0.81045720f,
   0.81758481f, 0.82458930f, 0.83146961f, 0.83822471f, 0.84485357f, 0.85135519f,
   0.85772861f, 0.86397286f, 0.87008699f, 0.87607009f, 0.88192126f, 0.88763962f,
   0.89322430f, 0.89867447f, 0.90398929f, 0.90916798f, 0.91420976f, 0.91911385f,
   0.92387953f, 0.92850608f, 0.93299280f, 0.93733901f, 0.94154407f, 0.94560733f,
   0.94952818f, 0.95330604f, 0.95694034f, 0.96043052f, 0.96377607f, 0.96697647f,
   0.97003125f, 0.97293995f, 0.97570213f, 0.97831737f, 0.98078528f, 0.98310549f,
   0.98527764f, 0.98730142f, 0.98917651f, 0.99090264f, 0.99247953f, 0.99390697f,
   0.99518473f, 0.99631261f, 0.99729046f, 0.99811811f, 0.99879546f, 0.99932238f,
   0.99969882f, 0.99992470f, 1.00000000f, 0.99992470f, 0.99969882f, 0.99932238f,
   0.99879546f, 0.99811811f, 0.99729046f, 0.99631261f, 0.99518473f, 0.99390697f,
   0.99247953f, 0.99090264f, 0.98917651f, 0.98730142f, 0.98527764f, 0.98310549f,
   0.98078528f, 0.97831737f, 0.97570213f, 0.97293995f, 0.97003125f, 0.96697647f,
   0.96377607f, 0.96043052f, 0.95694034f, 0.95330604f, 0.94952818f, 0.94560733f,
   0.94154407f, 0.93733901f, 0.93299280f, 0.92850608f, 0.92387953f, 0.91911385f,
   0.91420976f, 0.90916798f, 0.90398929f, 0.89867447f, 0.89322430f, 0.88763962f,
   0.88192126f, 0.87607009f, 0.87008699f, 0.86397286f, 0.85772861f, 0.85135519f,
   0.84485357f, 0.83822471f, 0.83146961f, 0.82458930f, 0.81758481f, 0.81045720f,
   0.80320753f, 0.79583690f, 0.78834643f, 0.78073723f, 0.77301045f, 0.76516727f,
   0.75720885f, 0.74913639f, 0.74095113f, 0.73265427f, 0.72424708f, 0.71573083f,
   0.70710678f, 0.69837625f, 0.68954054f, 0.68060100f, 0.67155895f, 0.66241578f,
   0.65317284f, 0.64383154f, 0.63439328f, 0.62485949f, 0.61523159f, 0.60551104f,
   0.59569930f, 0.58579786f, 0.57580819f, 0.56573181f, 0.55557023f, 0.54532499f,
   0.53499762f, 0.52458968f, 0.51410274f, 0.50353838f, 0.49289819f, 0.48218377f,
   0.47139674f, 0.46053871f, 0.44961133f, 0.43861624f, 0.42755509f, 0.41642956f,
   0.40524131f, 0.39399204f, 0.38268343f, 0.37131719f, 0.35989504f, 0.34841868f,
   0.33688985f, 0.32531029f, 0.31368174f, 0.30200595f, 0.29028468f, 0.27851969f,
   0.26671276f, 0.25486566f, 0.24298018f, 0.23105811f, 0.21910124f, 0.20711138f,
   0.19509032f, 0.18303989f, 0.17096189f, 0.15885814f, 0.14673047f, 0.13458071f,
   0.12241068f, 0.11022221f, 0.09801714f, 0.08579731f, 0.07356456f, 0.06132074f,
   0.04906767f, 0.03680722f, 0.02454123f, 0.01227154f, 0.00000000f, -0.01227154f,
   -0.02454123f, -0.03680722f, -0.04906767f, -0.06132074f, -0.07356456f,
   -0.08579731f, -0.09801714f, -0.11022221f, -0.12241068f, -0.13458071f,
   -0.14673047f, -0.15885814f, -0.17096189f, -0.18303989f, -0.19509032f,
   -0.20711138f, -0.21910124f, -0.23105811f, -0.24298018f, -0.25486566f,
   -0.26671276f, -0.27851969f, -0.29028468f, -0.30200595f, -0.31368174f,
   -0.32531029f, -0.33688985f, -0.34841868f, -0.35989504f, -0.37131719f,
   -0.38268343f, -0.39399204f, -0.40524131f, -0.41642956f, -0.42755509f,
   -0.43861624f, -0.44961133f, -0.46053871f, -0.47139674f, -0.48218377f,
   -0.49289819f, -0.50353838f, -0.51410274f, -0.52458968f, -0.53499762f,
   -0.54532499f, -0.55557023f, -0.56573181f, -0.57580819f, -0.58579786f,
   -0.59569930f, -0.60551104f, -0.61523159f, -0.62485949f, -0.63439328f,
   -0.64383154f, -0.65317284f, -0.66241578f, -0.67155895f, -0.68060100f,
   -0.68954054f, -0.69837625f, -0.70710678f, -0.71573083f, -0.72424708f,
   -0.73265427f, -0.74095113f, -0.74913639f, -0.75720885f, -0.76516727f,
   -0.77301045f, -0.78073723f, -0.78834643f, -0.79583690f, -0.80320753f,
   -0.81045720f, -0.81758481f, -0.82458930f, -0.83146961f, -0.83822471f,
   -0.84485357f, -0.85135519f, -0.85772861f, -0.86397286f, -0.87008699f,
   -0.87607009f, -0.88192126f, -0.88763962f, -0.89322430f, -0.89867447f,
   -0.90398929f, -0.90916798f, -0.91420976f, -0.91911385f, -0.92387953f,
   -0.92850608f, -0.93299280f, -0.93733901f, -0.94154407f, -0.94560733f,
   -0.94952818f, -0.95330604f, -0.95694034f, -0.96043052f, -0.96377607f,
   -0.96697647f, -0.97003125f, -0.97293995f, -0.97570213f, -0.97831737f,
   -0.98078528f, -0.98310549f, -0.98527764f, -0.98730142f, -0.98917651f,
   -0.99090264f, -0.99247953f, -0.99390697f, -0.99518473f, -0.99631261f,
   -0.99729046f, -0.99811811f, -0.99879546f, -0.99932238f, -0.99969882f,
   -0.99992470f, -1.00000000f, -0.99992470f, -0.99969882f, -0.99932238f,
   -0.99879546f, -0.99811811f, -0.99729046f, -0.99631261f, -0.99518473f,
   -0.99390697f, -0.99247953f, -0.99090264f, -0.98917651f, -0.98730142f,
   -0.98527764f, -0.98310549f, -0.98078528f, -0.97831737f, -0.97570213f,
   -0.97293995f, -0.97003125f, -0.96697647f, -0.96377607f, -0.96043052f,
   -0.95694034f, -0.95330604f, -0.94952818f, -0.94560733f, -0.94154407f,
   -0.93733901f, -0.93299280f, -0.92850608f, -0.92387953f, -0.91911385f,
   -0.91420976f, -0.90916798f, -0.90398929f, -0.89867447f, -0.89322430f,
   -0.88763962f, -0.88192126f, -0.87607009f, -0.87008699f, -0.86397286f,
   -0.85772861f, -0.85135519f, -0.84485357f, -0.83822471f, -0.83146961f,
   -0.82458930f, -0.81758481f, -0.81045720f, -0.80320753f, -0.79583690f,
   -0.78834643f, -0.78073723f, -0.77301045f, -0.76516727f, -0.75720885f,
   -0.74913639f, -0.74095113f, -0.73265427f, -0.72424708f, -0.71573083f,
   -0.70710678f, -0.69837625f, -0.68954054f, -0.68060100f, -0.67155895f,
   -0.66241578f, -0.65317284f, -0.64383154f, -0.63439328f, -0.62485949f,
   -0.61523159f, -0.60551104f, -0.59569930f, -0.58579786f, -0.57580819f,
   -0.56573181f, -0.55557023f, -0.54532499f, -0.53499762f, -0.52458968f,
   -0.51410274f, -0.50353838f, -0.49289819f, -0.48218377f, -0.47139674f,
   -0.46053871f, -0.44961133f, -0.43861624f, -0.42755509f, -0.41642956f,
   -0.40524131f, -0.39399204f, -0.38268343f, -0.37131719f, -0.35989504f,
   -0.34841868f, -0.33688985f, -0.32531029f, -0.31368174f, -0.30200595f,
   -0.29028468f, -0.27851969f, -0.26671276f, -0.25486566f, -0.24298018f,
   -0.23105811f, -0.21910124f, -0.20711138f, -0.19509032f, -0.18303989f,
   -0.17096189f, -0.15885814f, -0.14673047f, -0.13458071f, -0.12241068f,
   -0.11022221f, -0.09801714f, -0.08579731f, -0.07356456f, -0.06132074f,
   -0.04906767f, -0.03680722f, -0.02454123f, -0.01227154f, -0.00000000f
};

// Nonstandard convenience functions

inline float ifelse(bool cond, float a, float b) {
	return cond ? a : b;
}

/** Given a mask, returns a if mask is 0xffffffff per element, b if mask is 0x00000000 */
inline float_2 ifelse(float_2 mask, float_2 a, float_2 b) {
	return float_2(vbsl_f32(vreinterpret_u32_f32(mask.v), a.v, b.v));
}
/** Given a mask, returns a if mask is 0xffffffff per element, b if mask is 0x00000000 */
inline int32_2 ifelse(int32_2 mask, int32_2 a, int32_2 b) {
	return int32_2(vbsl_s32(vreinterpret_u32_s32(mask.v), a.v, b.v));
}

// Returns the maximum value between the two lanes of the argument
inline float max2(float_2 val)
{
    return fmax(val[0], val[1]);
}

// Table must be 513 values long.
inline float_2 interpolateTable(const float *table, const float_2 pos) 
{
    int32_2 index = ((int32_2)pos) & 0x1ff;
    float_2 fract = pos - float_2(index);
    float_2 onemfract = 1.f - fract;

    float left = onemfract[0] * table[index[0]] + fract[0] * table[index[0] + 1];
    float right = onemfract[1] * table[index[1]] + fract[1] * table[index[1] + 1];

    return float_2(left, right);
}

/** Returns a vector where element N is all 1's if the N'th bit of `a` is 1, or all 0's if the N'th bit of `a` is 0.
*/
template<typename T>
T movemaskInverse(int a);

template<>
inline int32_2 movemaskInverse<int32_2>(int a) {
	// Pick out N'th bit of `a` and check if it's 1.
	int32_2 mask12= int32_2(1, 2);
	return (mask12 & int32_2(a)) == mask12;
}

template<>
inline float_2 movemaskInverse<float_2>(int a) {
	return float_2::cast(movemaskInverse<int32_2>(a));
}

// a + b * c
inline float_2 multadd(float_2 a, float_2 b, float_2 c) {
	return float_2(vmla_f32(a.v, b.v, c.v));
}

inline float_2 multsub(float_2 a, float_2 b, float_2 c) {
	return float_2(vmls_f32(a.v, b.v, c.v));
}

inline float_2 vtaylor_poly_f32(float_2 x, const float_2 *coeffs)
{
    float_2 A   = multadd(coeffs[0], coeffs[4], x);
    float_2 B   = multadd(coeffs[2], coeffs[6], x);
    float_2 C   = multadd(coeffs[1], coeffs[5], x);
    float_2 D   = multadd(coeffs[3], coeffs[7], x);
    float_2 x2  = x * x;
    float_2 x4  = x2 * x2;
    float_2 res = multadd(multadd(A, B, x2), multadd(C, D, x2), x4);

    return res;
}

// Standard math functions from std::

/* Import std:: math functions into the simd namespace so you can use `sin(T)` etc in templated functions and get both the scalar and vector versions.

Example:

	template <typename T>
	T sin_plus_cos(T x) {
		return sin(x) + cos(x);
	}
*/

using std::fmax;

inline float_2 fmax(float_2 x, float_2 b) {
	return float_2(vmax_f32(x.v, b.v));
}

using std::fmin;

inline float_2 fmin(float_2 x, float_2 b) {
	return float_2(vmin_f32(x.v, b.v));
}

using std::sqrt;

inline float_2 sqrt(float_2 x) 
{
	return ifelse(x > 0, vrecpe_f32(vrsqrte_f32(x.v)), 0);
}

using std::log;

const float_2 log_tab[8] =
{
	-2.29561495781f,
	-2.47071170807f,
	-5.68692588806f,
	-0.165253549814f,
	5.17591238022f,
	0.844007015228f,
	4.58445882797f,
	0.0141278216615f,
};

// TODO: this probably don't work, so we replace it with a simplified operation
inline float_2 log(float_2 x)
{
    return float_2(log(x[0]), log(x[1]));

    static const int32_2 CONST_127{127,127};           // 127
    static const float_2 CONST_LN2{0.6931471805f,0.6931471805f}; // ln(2)

    // Extract exponent
    int32_2 m = vsub_s32( vcvt_s32_f32(vcvt_f32_u32(vshr_n_u32(vcvt_u32_f32(x.v), 23)) ), CONST_127.v);
    float_2 val = vcvt_f32_s32(vsub_s32(vcvt_s32_f32(x.v), vshl_n_s32(m.v, 23)));

    // Polynomial Approximation
    float_2 poly = vtaylor_poly_f32(val, log_tab);

    // Reconstruct
    poly = multadd(poly, int32_2(m), CONST_LN2);

    return poly;
}

using std::log10;

inline float_2 log10(float_2 x) {
	return log(x) / std::log(10.f);
}

using std::log2;

inline float_2 log2(float_2 x) {
	return log(x) / std::log(2.f);
}

using std::exp;

/** Exponent polynomial coefficients */
const float_2 exp_tab[8] =
{
	1.f,
	0.0416598916054f,
	0.500000596046f,
	0.0014122662833f,
	1.00000011921f,
	0.00833693705499f,
	0.166665703058f,
	0.000195780929062f,
};

// TODO: this probably don't work, so we replace it with a simplified operation
inline float_2 exp(float_2 x) 
{
    return float_2(exp(x[0]), exp(x[1]));

    static const float_2 CONST_LN2{0.6931471805f,0.6931471805f}; // ln(2)
    static const float_2 CONST_INV_LN2{1.4426950408f,1.4426950408f}; // 1/ln(2)
    static const float_2 CONST_0{0.f,0.f};
    static const int32_2 CONST_NEGATIVE_126{-126,-126};

    // Perform range reduction [-log(2),log(2)]
	float_2 m = x * CONST_INV_LN2;
    float_2 val = multsub(x, m, CONST_LN2);

    // Polynomial Approximation
    float_2 poly = vtaylor_poly_f32(val, exp_tab);

    // Reconstruct
    poly = vcvt_f32_s32(vqadd_s32(vcvt_s32_f32(poly.v), vqshl_n_s32(int32_2(m).v, 23)));
    poly = vbsl_f32(vclt_s32(int32_2(m).v, CONST_NEGATIVE_126.v), CONST_0.v, poly.v);
  
    return poly;
}

using std::sin;

inline float_2 sin(float_2 x) 
{
	// Special case for small negative inputs 
    /*
	if ((x < 0.0f) && (x >= -1.9e-7f)) 
	{
		return x;
	}
    */

	// input x is in radians 
	// Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi 
	float_2 in = x * 0.159154943092f;

	// Calculation of floor value of input 
	int32_2 n = (int32_2) in;

	// Make negative values towards -infinity 
	n = ifelse(n < 0, n - 1, n);

	// Map input value to [0 1] 
	in = in - (float_2) n;

	// Calculation of index of the table 
	float_2 findex = (float_2) FAST_MATH_TABLE_SIZE * in;

    return interpolateTable(sinTable513, findex);
}

using std::cos;

inline float_2 cos(float_2 x) {
	// input x is in radians 
	// Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi, add 0.25 (pi/2) to read sine table 
	float_2 in = x * 0.159154943092f + 0.25f;

	// Calculation of floor value of input 
	int32_2 n = (int32_2) in;

	// Make negative values towards -infinity 
	n = ifelse(n < 0, n - 1, n);

	// Map input value to [0 1] 
	in = in - (float_2) n;

	// Calculation of index of the table 
	float_2 findex = (float_2) FAST_MATH_TABLE_SIZE * in;

    return interpolateTable(sinTable513, findex);
}

using std::tan;

inline float_2 tan(float_2 x) {

    //return float_2(std::tan(x[0]), std::tan(x[1]));
	//return sin(x) / cos(x); //?

    // Approximation: https://andrewkay.name/blog/post/efficiently-approximating-tan-x/
    static const float pisqby4 = 2.4674011002723397f;
    static const float adjpisqby4 = 2.471688400562703f;
    static const float adj1minus8bypisq = 0.189759681063053f;
    float_2 xsq = x * x;

    float_2 d = pisqby4 - xsq;
    d = fmax(d, 0.0000000000000001f);

    return x * (adjpisqby4 - adj1minus8bypisq * xsq) / d;
}

using std::tanh;

inline float_2 tanh(float_2 x) {
	return 1 - (2.f * (1.f / (1.f + exp(x * 2.f)))); 
}

using std::atan;

/* inline float_2 atan(float_2 x) {
	return float_2(sse_mathfun_atan_ps(x.v));
} */

using std::atan2;

/* inline float_2 atan2(float_2 x, float_2 y) {
	return float_2(sse_mathfun_atan2_ps(x.v, y.v));
} */


/* Rounding mode macros. */
#define _MM_FROUND_TO_NEAREST_INT 0x00
#define _MM_FROUND_TO_NEG_INF 0x01
#define _MM_FROUND_TO_POS_INF 0x02
#define _MM_FROUND_TO_ZERO 0x03
#define _MM_FROUND_CUR_DIRECTION 0x04
#define _MM_FROUND_NO_EXC 0x08

inline float_2 neonRound(float_2 a, int rounding)
{
    switch (rounding) {
    case (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC):
        return vcvt_f32_s32(vcvt_s32_f32(a.v));
    case (_MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC):
        return float_2(floorf(a.s[0]), floorf(a.s[1]));
    case (_MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC):
        return float_2(ceilf(a.s[0]), ceilf(a.s[1]));
    case (_MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC):
    {
        float_2 zero{};
        float_2 neg_inf(floorf(a.s[0]), floorf(a.s[1]));
        float_2 pos_inf(ceilf(a.s[0]), ceilf(a.s[1]));

        return vbsl_f32(vcle_f32(a.v, zero.v), neg_inf.v, pos_inf.v);
    }
    default:  //_MM_FROUND_CUR_DIRECTION
        return float_2(roundf(a.s[0]), roundf(a.s[1]));
    }
}

using std::trunc;

inline float_2 trunc(float_2 a) {
	return neonRound(a, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
}

using std::floor;

inline float_2 floor(float_2 a)
{
    return neonRound(a, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
}

using std::ceil;

inline float_2 ceil(float_2 a) {
	return neonRound(a, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
}

using std::round;

inline float_2 round(float_2 a) {
	return neonRound(a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

using std::fmod;

inline float_2 fmod(float_2 a, float_2 b) {
	return a - floor(a / b) * b;
}

using std::hypot;

inline float_2 hypot(float_2 a, float_2 b) {
	return sqrt(a * a + b * b);
}

using std::fabs;

inline float_2 fabs(float_2 a) {
	return float_2(vabs_f32(a.v));
}

using std::abs;

inline float_2 abs(float_2 a) {
	return fabs(a);
}

inline float_2 abs(std::complex<float_2> a) {
	return hypot(a.real(), a.imag());
}

using std::arg;

/* inline float_2 arg(std::complex<float_2> a) {
	return atan2(a.imag(), a.real());
} */

using std::pow;

inline float_2 pow(float_2 a, float_2 b) {
    // TODO: update this when exp() and log() are fixed
    return float_2(pow(a[0], b[0]), pow(a[0], b[0]));
	//return exp(b * log(a));
}

inline float_2 pow(float a, float_2 b) {
    // TODO: update this when exp() and log() are fixed
    return float_2(pow(a, b[0]), pow(a, b[0]));
	//return exp(b * std::log(a));
}

template<typename T>
T pow(T a, int b) {
	// Optimal with `-O3 -funsafe-math-optimizations` when b is known at compile-time
	T p = 1;
	for (int i = 1; i <= b; i *= 2) {
		if (i & b)
			p *= a;
		a *= a;
	}
	return p;
}

using std::isnan;

inline float_2 isnan(float_2 a) {
    return a.v == vdup_n_f32(NAN);
}

using std::isinf;

inline float_2 isinf(float_2 a) {
    return a.v == vdup_n_f32(INFINITY);
}

inline void print_2(std::string l, float_2 v)
{
	printf("%s: %f %f\n", l.c_str(), v[0], v[1]);
}

inline float_2 exponentialBipolar80Pade_5_4(float_2 x)
{
    return (0.109568f * x + 0.281588f * pow(x, 3.f) + 0.133841f * pow(x, 5.f)) / (1.f - 0.630374f * pow(x, 2.f) + 0.166271f * pow(x, 4.f));
}

// 0.f < x < 1.f
inline float_2 sin2pi_pade_05_5_4(float_2 x)
{
    x -= 0.5f;

    return (-6.283185307f * x + 33.19863968f * pow(x, 3.f) - 32.44191367f * pow(x, 5.f)) / (1 + 1.296008659f * pow(x, 2.f) + 0.7028072946f * pow(x, 4.f));
}

// equal sum crossfade, -1 <= p <= 1
inline float_2 equalSumCrossfade(float_2 a, float_2 b, const float p)
{
    return a * (0.5f * (1.f - p)) + b * (0.5f * (1.f + p));
}

// equal power crossfade, -1 <= p <= 1
inline float_2 equalPowerCrossfade(float_2 a, float_2 b, float_2 p)
{
    return fmin(exponentialBipolar80Pade_5_4(p + 1), 1.f) * b + fmin(exponentialBipolar80Pade_5_4(1 - p), 1.f) * a;
}
