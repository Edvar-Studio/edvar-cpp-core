#include "Math/Math.hpp"
#include <cmath>

#ifndef EDVAR_CPP_CORE_MATH_ALLOW_SIMD
#    define EDVAR_CPP_CORE_MATH_ALLOW_SIMD 1 // Enable SIMD by default, this can be disabled
#endif

#ifndef EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
#    define EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX 1 // Enable AVX by default, this can be disabled
#endif

#ifndef EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS
#    ifdef _MSC_VER
#        define EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS EDVAR_CPP_CORE_MATH_ALLOW_SIMD
#    else
#        define EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS 0
#    endif
#endif

#if !defined(EDVAR_CPP_CORE_MATH_GCC_INTRINSICS) && !EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS
#    if (defined(__GNUC__) || defined(__clang__))
#        define EDVAR_CPP_CORE_MATH_GCC_INTRINSICS EDVAR_CPP_CORE_MATH_ALLOW_SIMD
#    else
#        define EDVAR_CPP_CORE_MATH_GCC_INTRINSICS 0
#    endif
#endif

#if defined(__SSE__) || defined(_M_AMD64) || defined(_M_X64) || defined(_M_IX86) || defined(__i386__) ||               \
    defined(__x86_64__)
#    define EDVAR_CPP_CORE_MATH_X86_SIMD EDVAR_CPP_CORE_MATH_ALLOW_SIMD
#    define EDVAR_CPP_CORE_MATH_ARM64_SIMD 0
#    define EDVAR_CPP_CORE_MATH_ARM_SIMD 0
#elif defined(_M_ARM64) || defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__)
#    define EDVAR_CPP_CORE_MATH_ARM64_SIMD EDVAR_CPP_CORE_MATH_ALLOW_SIMD
#    define EDVAR_CPP_CORE_MATH_X86_SIMD 0
#    define EDVAR_CPP_CORE_MATH_ARM_SIMD 0
#elif defined(_M_ARM) || defined(__arm__)
#    define EDVAR_CPP_CORE_MATH_ARM_SIMD EDVAR_CPP_CORE_MATH_ALLOW_SIMD
#    define EDVAR_CPP_CORE_MATH_ARM64_SIMD 0
#    define EDVAR_CPP_CORE_MATH_X86_SIMD 0
#endif

#if EDVAR_CPP_CORE_MATH_X86_SIMD
#    if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS
#        include <immintrin.h>
#    elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS
#        include <x86intrin.h>
#    endif
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
#    include <arm_neon.h>
#endif

namespace Edvar::Math {
double Power(double base, double exponent) { return std::pow(base, exponent); }
float Power(float base, float exponent) { return std::pow(base, exponent); }

double NaturalLogarithm(double value) { return std::log(value); }
float NaturalLogarithm(float value) { return std::log(value); }

double BinaryLogarithm(double value) { return std::log2(value); }
float BinaryLogarithm(float value) { return std::log2(value); }

double Logarithm(double value, double base) { return std::log(value) / std::log(base); }
float Logarithm(float value, float base) { return std::log(value) / std::log(base); }

float SquareRoot(float value) { return std::sqrt(value); }
double SquareRoot(double value) { return std::sqrt(value); }

double InverseSquareRoot(double value) { return 1.0 / std::sqrt(value); }
float InverseSquareRoot(float value) { return 1.0f / std::sqrt(value); }

EDVAR_CPP_CORE_API double Exp(double exponent) { return std::exp(exponent); }
EDVAR_CPP_CORE_API float Exp(float exponent) { return std::exp(exponent); }

double Root(double value, double degree) { return std::pow(value, 1.0 / degree); }
float Root(float value, float degree) { return std::pow(value, 1.0f / degree); }

double InverseRoot(double value, double degree) { return 1.0 / std::pow(value, 1.0 / degree); }
float InverseRoot(float value, float degree) { return 1.0f / std::pow(value, 1.0f / degree); }
namespace SIMD {

#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
Float_4 CreateFloat_4FromMM128(__m128 value) {
    Float_4 result;
    _mm_store_ps(result.XYZW, value);
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
Float_4 CreateFloat_4FromV4SF(v4sf value) {
    Float_4 result;
    __builtin_ia32_storps(result.XYZW, value);
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_ARM64_SIMD
Float_4 CreateFloat_4FromFloat32x4(float32x4_t value) {
    Float_4 result;
    vst1q_f32(result.XYZW, value);
    return result;
}
#endif

Float_4 Float_4::operator+(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_add_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_addps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vaddq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vadd_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{X + other.X, Y + other.Y, Z + other.Z, W + other.W};
#endif
}
Float_4 Float_4::operator-(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_sub_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_subps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vsubq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vsub_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{X - other.X, Y - other.Y, Z - other.Z, W - other.W};
#endif
}
Float_4 Float_4::operator*(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_mul_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_mulps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vmulq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vmul_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{X * other.X, Y * other.Y, Z * other.Z, W * other.W};
#endif
}
Float_4 Float_4::operator/(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_div_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_divps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vdivq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vdiv_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{X / other.X, Y / other.Y, Z / other.Z, W / other.W};
#endif
}
Float_4 Float_4::LoadAligned(const float* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_load_ps(ptr));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_loadaps(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(vld1q_f32(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(vld1_f32(ptr));
#else
    return Float_4{ptr[0], ptr[1], ptr[2], ptr[3]};
#endif
}
Float_4 Float_4::LoadUnaligned(const float* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_loadu_ps(ptr));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_loadups(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(vld1q_f32(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(vld1_f32(ptr));
#else
    return Float_4{ptr[0], ptr[1], ptr[2], ptr[3]};
#endif
}
void Float_4::StoreAligned(float* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    _mm_store_ps(ptr, *reinterpret_cast<const __m128*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __builtin_ia32_storeaps(ptr, *reinterpret_cast<const v4sf*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    vst1q_f32(ptr, *reinterpret_cast<const float32x4_t*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    vst1_f32(ptr, *reinterpret_cast<const float32x4_t*>(XYZW));
#else
    ptr[0] = X;
    ptr[1] = Y;
    ptr[2] = Z;
    ptr[3] = W;
#endif
}
void Float_4::StoreUnaligned(float* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    _mm_storeu_ps(ptr, *reinterpret_cast<const __m128*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __builtin_ia32_storeups(ptr, *reinterpret_cast<const v4sf*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    vst1q_f32(ptr, *reinterpret_cast<const float32x4_t*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    vst1_f32(ptr, *reinterpret_cast<const float32x4_t*>(XYZW));
#else
    ptr[0] = X;
    ptr[1] = Y;
    ptr[2] = Z;
    ptr[3] = W;
#endif
}
Float_4 Float_4::LessThan(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_cmplt_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_cmpltps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vcgtq_f32(*reinterpret_cast<const float32x4_t*>(other.XYZW), *reinterpret_cast<const float32x4_t*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vcgt_f32(*reinterpret_cast<const float32x4_t*>(other.XYZW), *reinterpret_cast<const float32x4_t*>(XYZW)));
#else
    return Float_4{(X < other.X) ? 1.0f : 0.0f, (Y < other.Y) ? 1.0f : 0.0f, (Z < other.Z) ? 1.0f : 0.0f,
                   (W < other.W) ? 1.0f : 0.0f};
#endif
}
Float_4 Float_4::GreaterThan(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_cmpgt_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_cmpgtps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vcgtq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vcgt_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{(X > other.X) ? 1.0f : 0.0f, (Y > other.Y) ? 1.0f : 0.0f, (Z > other.Z) ? 1.0f : 0.0f,
                   (W > other.W) ? 1.0f : 0.0f};
#endif
}

Float_4 Float_4::Equal(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_cmpeq_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_cmpeqps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vceqq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vceq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{(X == other.X) ? 1.0f : 0.0f, (Y == other.Y) ? 1.0f : 0.0f, (Z == other.Z) ? 1.0f : 0.0f,
                   (W == other.W) ? 1.0f : 0.0f};
#endif
}
Float_4 Float_4::HorizontalAdd(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_hadd_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_haddps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float32x4_t a = *reinterpret_cast<const float32x4_t*>(XYZW);
    float32x4_t b = *reinterpret_cast<const float32x4_t*>(other.XYZW);
    return CreateFloat_4FromFloat32x4(vpaddq_f32(a, b));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    float32x4_t a = *reinterpret_cast<const float32x4_t*>(XYZW);
    float32x4_t b = *reinterpret_cast<const float32x4_t*>(other.XYZW);
    return CreateFloat_4FromFloat32x4(vpadd_f32(a, b));
#else
    return Float_4{(X + Y), (Z + W), (other.X + other.Y), (other.Z + other.W)};
#endif
}

Float_4 Float_4::Min(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_min_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_minps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vminq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vmin_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{(X < other.X) ? X : other.X, (Y < other.Y) ? Y : other.Y, (Z < other.Z) ? Z : other.Z,
                   (W < other.W) ? W : other.W};
#endif
}

Float_4 Float_4::Max(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_max_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_maxps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vmaxq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vmax_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{(X > other.X) ? X : other.X, (Y > other.Y) ? Y : other.Y, (Z > other.Z) ? Z : other.Z,
                   (W > other.W) ? W : other.W};
#endif
}
Float_4 Float_4::Dot(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_dp_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), 0xFF));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_dpps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), 0xFF));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(vdupq_n_f32(vaddvq_f32(
        vmulq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)))));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    float32x4_t mul =
        vmul_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW));
    float32x2_t lowHigh = vpadd_f32(vget_low_f32(mul), vget_high_f32(mul));
    float32x2_t sum = vpadd_f32(lowHigh, lowHigh);
    return CreateFloat_4FromFloat32x4(vdupq_n_f32(vget_lane_f32(sum, 0)));
#else
    float dotProduct = (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W);
    return Float_4{dotProduct, dotProduct, dotProduct, dotProduct};
#endif
}
Float_4 Float_4::SquareRoot() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_sqrt_ps(*reinterpret_cast<const __m128*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_sqrtps(*reinterpret_cast<const v4sf*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(vsqrtq_f32(*reinterpret_cast<const float32x4_t*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(vsqrt_f32(*reinterpret_cast<const float32x4_t*>(XYZW)));
#else
    return Float_4{std::sqrt(X), std::sqrt(Y), std::sqrt(Z), std::sqrt(W)};
#endif
}
Float_4 Float_4::Normalize() const { return (*this) / Length(); }

Float_4 Float_4::Length() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128 mul = _mm_mul_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(XYZW));
    __m128 shuf1 = _mm_movehdup_ps(mul);
    __m128 sum1 = _mm_add_ps(mul, shuf1);
    __m128 shuf2 = _mm_movehl_ps(shuf1, sum1);
    __m128 sum2 = _mm_add_ss(sum1, shuf2);
    __m128 sqrt = _mm_sqrt_ss(sum2);
    return CreateFloat_4FromMM128(_mm_shuffle_ps(sqrt, sqrt, 0x00));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4sf mul = __builtin_ia32_mulps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(XYZW));
    v4sf shuf1 = __builtin_ia32_movehdups(mul);
    v4sf sum1 = __builtin_ia32_addps(mul, shuf1);
    v4sf shuf2 = __builtin_ia32_movehlps(shuf1, sum1);
    v4sf sum2 = __builtin_ia32_addss(sum1, shuf2);
    v4sf sqrt = __builtin_ia32_sqrtss(sum2);
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(sqrt, sqrt, 0x00));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float32x4_t mul =
        vmulq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(XYZW));
    float32x2_t low = vget_low_f32(mul);
    float32x2_t high = vget_high_f32(mul);
    float32x2_t sum1 = vpadd_f32(low, high);
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    float32x4_t sqrt = vdupq_n_f32(vsqrt_f32(sum2)[0]);
    return CreateFloat_4FromFloat32x4(sqrt);
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    float32x4_t mul =
        vmul_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(XYZW));
    float32x2_t low = vget_low_f32(mul);
    float32x2_t high = vget_high_f32(mul);
    float32x2_t sum1 = vpadd_f32(low, high);
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    float32x4_t sqrt = vdupq_n_f32(vsqrt_f32(sum2)[0]);
    return CreateFloat_4FromFloat32x4(sqrt);
#else
    float length = std::sqrt((X * X) + (Y * Y) + (Z * Z) + (W * W));
    return Float_4{length, length, length, length};
#endif
}
Float_4 Float_4::LengthSquared() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128 mul = _mm_mul_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(XYZW));
    __m128 shuf1 = _mm_movehdup_ps(mul);
    __m128 sum1 = _mm_add_ps(mul, shuf1);
    __m128 shuf2 = _mm_movehl_ps(shuf1, sum1);
    __m128 sum2 = _mm_add_ss(sum1, shuf2);
    return CreateFloat_4FromMM128(_mm_shuffle_ps(sum2, sum2, 0x00));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4sf mul = __builtin_ia32_mulps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(XYZW));
    v4sf shuf1 = __builtin_ia32_movehdups(mul);
    v4sf sum1 = __builtin_ia32_addps(mul, shuf1);
    v4sf shuf2 = __builtin_ia32_movehlps(shuf1, sum1);
    v4sf sum2 = __builtin_ia32_addss(sum1, shuf2);
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(sum2, sum2, 0x00));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float32x4_t mul =
        vmulq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(XYZW));
    float32x2_t low = vget_low_f32(mul);
    float32x2_t high = vget_high_f32(mul);
    float32x2_t sum1 = vpadd_f32(low, high);
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    float32x4_t result = vdupq_n_f32(vget_lane_f32(sum2, 0));
    return CreateFloat_4FromFloat32x4(result);
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    float32x4_t mul =
        vmul_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(XYZW));
    float32x2_t low = vget_low_f32(mul);
    float32x2_t high = vget_high_f32(mul);
    float32x2_t sum1 = vpadd_f32(low, high);
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    float32x4_t result = vdupq_n_f32(vget_lane_f32(sum2, 0));
    return CreateFloat_4FromFloat32x4(result);
#else
    float lengthSquared = (X * X) + (Y * Y) + (Z * Z) + (W * W);
    return Float_4{lengthSquared, lengthSquared, lengthSquared, lengthSquared};
#endif
}

Float_4 Float_4::ShuffleXYZW(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(3, 2, 1, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(3, 2, 1, 0)));
// #elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
//     float32x4_t a = *reinterpret_cast<const float32x4_t*>(XYZW);
//     float32x4_t b = *reinterpret_cast<const float32x4_t*>(other.XYZW);
//     return CreateFloat_4FromFloat32x4(vcombine_f32(vget_low_f32(a), vget_low_f32(b)));
// #elif EDVAR_CPP_CORE_MATH_ARM_SIMD
//     float32x4_t a = *reinterpret_cast<const float32x4_t*>(XYZW);
//     float32x4_t b = *reinterpret_cast<const float32x4_t*>(other.XYZW);
//     return CreateFloat_4FromFloat32x4(vcombine_f32(vget_low_f32(a), vget_low_f32(b)));
#else
    return Float_4{X, Y, other.Z, other.W};
#endif
}
Float_4 Float_4::ShuffleYWZX(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(0, 2, 3, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(0, 2, 3, 1)));
// #elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
//     float32x4_t a = *reinterpret_cast<const float32x4_t*>(XYZW);
//     float32x4_t b = *reinterpret_cast<const float32x4_t*>(other.XYZW);
//     return CreateFloat_4FromFloat32x4(vcombine_f32(vget_high_f32(a), vget_low_f32(b)));
// #elif EDVAR_CPP_CORE_MATH_ARM_SIMD
//     float32x4_t a = *reinterpret_cast<const float32x4_t*>(XYZW);
//     float32x4_t b = *reinterpret_cast<const float32x4_t*>(other.XYZW);
//     return CreateFloat_4FromFloat32x4(vcombine_f32(vget_high_f32(a), vget_low_f32(b)));
#else
    return Float_4{Y, W, other.Z, other.X};
#endif
}
Float_4 Float_4::ShuffleZWXY(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(1, 0, 3, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(1, 0, 3, 2)));
#else
    return Float_4{Z, W, other.X, other.Y};
#endif
}
Float_4 Float_4::ShuffleWXYZ(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(2, 1, 0, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(2, 1, 0, 3)));
#else
    return Float_4{W, X, other.Y, other.Z};
#endif
}
Float_4 Float_4::ShuffleXXYY(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(1, 1, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(1, 1, 0, 0)));
#else
    return Float_4{X, X, other.Y, other.Y};
#endif
}
Float_4 Float_4::ShuffleZZWW(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(3, 3, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(3, 3, 2, 2)));
#else
    return Float_4{Z, Z, other.W, other.W};
#endif
}
Float_4 Float_4::ShuffleXZXZ(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(2, 0, 2, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(2, 0, 2, 0)));
#else
    return Float_4{X, Z, other.X, other.Z};
#endif
}
Float_4 Float_4::ShuffleYZYZ(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(2, 1, 2, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(2, 1, 2, 1)));
#else
    return Float_4{Y, Z, other.Y, other.Z};
#endif
}
Float_4 Float_4::ShuffleWYWY(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(1, 3, 1, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(1, 3, 1, 3)));
#else
    return Float_4{W, Y, other.W, other.Y};
#endif
}
Float_4 Float_4::ShuffleXYXY(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(1, 0, 1, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(1, 0, 1, 0)));
#else
    return Float_4{X, Y, other.X, other.Y};
#endif
}
Float_4 Float_4::ShuffleZWZW(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(3, 2, 3, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(3, 2, 3, 2)));
#else
    return Float_4{Z, W, other.Z, other.W};
#endif
}
Float_4 Float_4::ShuffleXXXX(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(0, 0, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(0, 0, 0, 0)));
#else
    return Float_4{X, X, other.X, other.X};
#endif
}
Float_4 Float_4::ShuffleYYYY(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(1, 1, 1, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(1, 1, 1, 1)));
#else
    return Float_4{Y, Y, other.Y, other.Y};
#endif
}
Float_4 Float_4::ShuffleZZZZ(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(2, 2, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(2, 2, 2, 2)));
#else
    return Float_4{Z, Z, other.Z, other.Z};
#endif
}
Float_4 Float_4::ShuffleWWWW(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(_mm_shuffle_ps(
        *reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), _MM_SHUFFLE(3, 3, 3, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(__builtin_ia32_shufps(
        *reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), _MM_SHUFFLE(3, 3, 3, 3)));
#else
    return Float_4{W, W, other.W, other.W};
#endif
}

void Float_4::MatrixMultiply4x4(const Float_4 matrix[4], const Float_4 otherTransposedMatrix[4], Float_4 result[4]) {
    for (int i = 0; i < 4; ++i) {
        const Float_4& rowF = matrix[i];

        const Float_4& col0F = otherTransposedMatrix[0];
        const Float_4& col1F = otherTransposedMatrix[1];
        const Float_4& col2F = otherTransposedMatrix[2];
        const Float_4& col3F = otherTransposedMatrix[3];

        const Float_4 d0F = rowF.DotLane0(col0F);
        const Float_4 d1F = rowF.DotLane0(col1F);
        const Float_4 d2F = rowF.DotLane0(col2F);
        const Float_4 d3F = rowF.DotLane0(col3F);

        const Float_4 loF = d0F.UnpackInterleaveLowHalf(d1F);
        const Float_4 hiF = d2F.UnpackInterleaveLowHalf(d3F);
        result[i] = loF.MoveOtherLowerToHigher(hiF);
    }
}

Float_4 Float_4::operator|(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_or_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_orps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vorrq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vor_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{std::bit_cast<float>(std::bit_cast<uint32_t>(X) | std::bit_cast<uint32_t>(other.X)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(Y) | std::bit_cast<uint32_t>(other.Y)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(Z) | std::bit_cast<uint32_t>(other.Z)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(W) | std::bit_cast<uint32_t>(other.W))};
#endif
}
Float_4 Float_4::operator&(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_and_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_andps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        vandq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vand_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{std::bit_cast<float>(std::bit_cast<uint32_t>(X) & std::bit_cast<uint32_t>(other.X)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(Y) & std::bit_cast<uint32_t>(other.Y)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(Z) & std::bit_cast<uint32_t>(other.Z)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(W) & std::bit_cast<uint32_t>(other.W))};
#endif
}
Float_4 Float_4::operator^(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_xor_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_xorps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateFloat_4FromFloat32x4(
        veorq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        veor_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{std::bit_cast<float>(std::bit_cast<uint32_t>(X) ^ std::bit_cast<uint32_t>(other.X)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(Y) ^ std::bit_cast<uint32_t>(other.Y)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(Z) ^ std::bit_cast<uint32_t>(other.Z)),
                   std::bit_cast<float>(std::bit_cast<uint32_t>(W) ^ std::bit_cast<uint32_t>(other.W))};
#endif
}
Float_4 Float_4::DotLane0(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_dp_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW), 0xF1));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_dpps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW), 0xF1));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float32x4_t mul =
        vmulq_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW));
    float32x2_t low = vget_low_f32(mul);
    float32x2_t high = vget_high_f32(mul);
    float32x2_t sum1 = vpaddq_f32(low, high);
    float32x2_t sum2 = vpaddq_f32(sum1, sum1);
    float32x4_t result = vdupq_n_f32(vget_lane_f32(sum2, 0));
    return CreateFloat_4FromFloat32x4(result);
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    float32x4_t mul =
        vmul_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW));
    float32x2_t low = vget_low_f32(mul);
    float32x2_t high = vget_high_f32(mul);
    float32x2_t sum1 = vpadd_f32(low, high);
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    float32x4_t result = vdup_n_f32(vget_lane_f32(sum2, 0));
    return CreateFloat_4FromFloat32x4(result);
#else
    float dot = (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W);
    return Float_4{dot, 0.0f, 0.0f, 0.0f};
#endif
}
Float_4 Float_4::UnpackInterleaveLowHalf(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_unpacklo_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_unpcklps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateFloat_4FromFloat32x4(
        vzip1q_f32(*reinterpret_cast<const float32x4_t*>(XYZW), *reinterpret_cast<const float32x4_t*>(other.XYZW)));
#else
    return Float_4{X, other.X, Y, other.Y};
#endif
}
Float_4 Float_4::MoveOtherLowerToHigher(const Float_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromMM128(
        _mm_movelh_ps(*reinterpret_cast<const __m128*>(XYZW), *reinterpret_cast<const __m128*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateFloat_4FromV4SF(
        __builtin_ia32_movelhps(*reinterpret_cast<const v4sf*>(XYZW), *reinterpret_cast<const v4sf*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    float32x2_t low = vget_low_f32(*reinterpret_cast<const float32x4_t*>(XYZW));
    float32x2_t high = vget_low_f32(*reinterpret_cast<const float32x4_t*>(other.XYZW));
    return CreateFloat_4FromFloat32x4(vcombine_f32(low, high));
#else
    return Float_4{X, Y, other.X, other.Y};
#endif
}
// Double_2 helper functions
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
Double_2 CreateDouble_2FromMM128D(__m128d value) {
    Double_2 result;
    _mm_store_pd(result.XY, value);
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
Double_2 CreateDouble_2FromV2DF(v2df value) {
    Double_2 result;
    __builtin_ia32_storepd(result.XY, value);
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_ARM64_SIMD
Double_2 CreateDouble_2FromFloat64x2(float64x2_t value) {
    Double_2 result;
    vst1q_f64(result.XY, value);
    return result;
}
#endif

Double_2 Double_2::operator+(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_add_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_addpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vaddq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{X + other.X, Y + other.Y};
#endif
}
Double_2 Double_2::operator-(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_sub_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_subpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vsubq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{X - other.X, Y - other.Y};
#endif
}
Double_2 Double_2::operator*(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_mul_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_mulpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vmulq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{X * other.X, Y * other.Y};
#endif
}
Double_2 Double_2::operator/(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_div_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_divpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vdivq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{X / other.X, Y / other.Y};
#endif
}
Double_2 Double_2::LoadAligned(const double* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(_mm_load_pd(ptr));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(__builtin_ia32_loadapd(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(vld1q_f64(ptr));
#else
    return Double_2{ptr[0], ptr[1]};
#endif
}
Double_2 Double_2::LoadUnaligned(const double* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(_mm_loadu_pd(ptr));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(__builtin_ia32_loadupd(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(vld1q_f64(ptr));
#else
    return Double_2{ptr[0], ptr[1]};
#endif
}
void Double_2::StoreAligned(double* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    _mm_store_pd(ptr, *reinterpret_cast<const __m128d*>(XY));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __builtin_ia32_storeapd(ptr, *reinterpret_cast<const v2df*>(XY));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    vst1q_f64(ptr, *reinterpret_cast<const float64x2_t*>(XY));
#else
    ptr[0] = X;
    ptr[1] = Y;
#endif
}
void Double_2::StoreUnaligned(double* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    _mm_storeu_pd(ptr, *reinterpret_cast<const __m128d*>(XY));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __builtin_ia32_storeupd(ptr, *reinterpret_cast<const v2df*>(XY));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    vst1q_f64(ptr, *reinterpret_cast<const float64x2_t*>(XY));
#else
    ptr[0] = X;
    ptr[1] = Y;
#endif
}
Double_2 Double_2::LessThan(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_cmplt_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_cmpltpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vcgtq_f64(*reinterpret_cast<const float64x2_t*>(other.XY), *reinterpret_cast<const float64x2_t*>(XY)));
#else
    return Double_2{(X < other.X) ? 1.0 : 0.0, (Y < other.Y) ? 1.0 : 0.0};
#endif
}
Double_2 Double_2::GreaterThan(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_cmpgt_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_cmpgtpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vcgtq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{(X > other.X) ? 1.0 : 0.0, (Y > other.Y) ? 1.0 : 0.0};
#endif
}

Double_2 Double_2::Equal(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_cmpeq_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_cmpeqpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vceqq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{(X == other.X) ? 1.0 : 0.0, (Y == other.Y) ? 1.0 : 0.0};
#endif
}
Double_2 Double_2::HorizontalAdd(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_hadd_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_haddpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float64x2_t a = *reinterpret_cast<const float64x2_t*>(XY);
    float64x2_t b = *reinterpret_cast<const float64x2_t*>(other.XY);
    return CreateDouble_2FromFloat64x2(vpaddq_f64(a, b));
#else
    return Double_2{(X + Y), (other.X + other.Y)};
#endif
}

Double_2 Double_2::Min(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_min_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_minpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vminq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{(X < other.X) ? X : other.X, (Y < other.Y) ? Y : other.Y};
#endif
}

Double_2 Double_2::Max(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_max_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_maxpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vmaxq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{(X > other.X) ? X : other.X, (Y > other.Y) ? Y : other.Y};
#endif
}
Double_2 Double_2::Dot(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_dp_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY), 0x33));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_dppd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY), 0x33));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float64x2_t mul =
        vmulq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY));
    double dotProduct = vaddvq_f64(mul);
    return CreateDouble_2FromFloat64x2(vdupq_n_f64(dotProduct));
#else
    double dotProduct = (X * other.X) + (Y * other.Y);
    return Double_2{dotProduct, dotProduct};
#endif
}
Double_2 Double_2::SquareRoot() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(_mm_sqrt_pd(*reinterpret_cast<const __m128d*>(XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(__builtin_ia32_sqrtpd(*reinterpret_cast<const v2df*>(XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(vsqrtq_f64(*reinterpret_cast<const float64x2_t*>(XY)));
#else
    return Double_2{std::sqrt(X), std::sqrt(Y)};
#endif
}
Double_2 Double_2::Normalize() const { return (*this) / Length(); }

Double_2 Double_2::Length() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128d mul = _mm_mul_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(XY));
    __m128d shuf = _mm_shuffle_pd(mul, mul, 0x01);
    __m128d sum = _mm_add_sd(mul, shuf);
    __m128d sqrt = _mm_sqrt_sd(sum, sum);
    return CreateDouble_2FromMM128D(_mm_shuffle_pd(sqrt, sqrt, 0x00));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2df mul = __builtin_ia32_mulpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(XY));
    v2df shuf = __builtin_ia32_shufpd(mul, mul, 0x01);
    v2df sum = __builtin_ia32_addsd(mul, shuf);
    v2df sqrt = __builtin_ia32_sqrtsd(sum);
    return CreateDouble_2FromV2DF(__builtin_ia32_shufpd(sqrt, sqrt, 0x00));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float64x2_t mul = vmulq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(XY));
    double length = std::sqrt(vaddvq_f64(mul));
    return CreateDouble_2FromFloat64x2(vdupq_n_f64(length));
#else
    double length = std::sqrt((X * X) + (Y * Y));
    return Double_2{length, length};
#endif
}
Double_2 Double_2::LengthSquared() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128d mul = _mm_mul_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(XY));
    __m128d shuf = _mm_shuffle_pd(mul, mul, 0x01);
    __m128d sum = _mm_add_sd(mul, shuf);
    return CreateDouble_2FromMM128D(_mm_shuffle_pd(sum, sum, 0x00));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2df mul = __builtin_ia32_mulpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(XY));
    v2df shuf = __builtin_ia32_shufpd(mul, mul, 0x01);
    v2df sum = __builtin_ia32_addsd(mul, shuf);
    return CreateDouble_2FromV2DF(__builtin_ia32_shufpd(sum, sum, 0x00));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float64x2_t mul = vmulq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(XY));
    double lengthSquared = vaddvq_f64(mul);
    return CreateDouble_2FromFloat64x2(vdupq_n_f64(lengthSquared));
#else
    double lengthSquared = (X * X) + (Y * Y);
    return Double_2{lengthSquared, lengthSquared};
#endif
}

// Double_2 shuffles - _mm_shuffle_pd takes 2-bit immediate to select which element from each vector
// Bit 0: selects element from first vector (0=X, 1=Y)
// Bit 1: selects element from second vector (0=X, 1=Y)
Double_2 Double_2::ShuffleXY(const Double_2& other) const {
    // 0x0: this.X (bit 0=0), other.Y (bit 1=0) -> Wait, bit 1=0 means other.X
    // Correct: 0x0 = this.X, other.X BUT we want this.X, other.Y
    // So we need: bit 0=0 (this.X), bit 1=1 (other.Y) = 0x2
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_shuffle_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY), 0x0));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_shufpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY), 0x0));
#else
    return Double_2{X, other.Y};
#endif
}
Double_2 Double_2::ShuffleXX(const Double_2& other) const {
    // bit 0=0 (this.X), bit 1=0 (other.X) = 0x0
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_shuffle_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY), 0x0));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_shufpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY), 0x0));
#else
    return Double_2{X, other.X};
#endif
}
Double_2 Double_2::ShuffleYY(const Double_2& other) const {
    // bit 0=1 (this.Y), bit 1=1 (other.Y) = 0x3
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_shuffle_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY), 0x3));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_shufpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY), 0x3));
#else
    return Double_2{Y, other.Y};
#endif
}
Double_2 Double_2::ShuffleYX(const Double_2& other) const {
    // bit 0=1 (this.Y), bit 1=0 (other.X) = 0x1
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_shuffle_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY), 0x1));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_shufpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY), 0x1));
#else
    return Double_2{Y, other.X};
#endif
}

Double_2 Double_2::operator|(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_or_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_orpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vorrq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{std::bit_cast<double>(std::bit_cast<uint64_t>(X) | std::bit_cast<uint64_t>(other.X)),
                    std::bit_cast<double>(std::bit_cast<uint64_t>(Y) | std::bit_cast<uint64_t>(other.Y))};
#endif
}
Double_2 Double_2::operator&(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_and_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_andpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vandq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{std::bit_cast<double>(std::bit_cast<uint64_t>(X) & std::bit_cast<uint64_t>(other.X)),
                    std::bit_cast<double>(std::bit_cast<uint64_t>(Y) & std::bit_cast<uint64_t>(other.Y))};
#endif
}
Double_2 Double_2::operator^(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_xor_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_xorpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        veorq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{std::bit_cast<double>(std::bit_cast<uint64_t>(X) ^ std::bit_cast<uint64_t>(other.X)),
                    std::bit_cast<double>(std::bit_cast<uint64_t>(Y) ^ std::bit_cast<uint64_t>(other.Y))};
#endif
}
Double_2 Double_2::DotLane0(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_dp_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY), 0x31));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_dppd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY), 0x31));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    float64x2_t mul =
        vmulq_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY));
    double dot = vaddvq_f64(mul);
    return CreateDouble_2FromFloat64x2(vsetq_lane_f64(dot, vdupq_n_f64(0.0), 0));
#else
    double dot = (X * other.X) + (Y * other.Y);
    return Double_2{dot, 0.0};
#endif
}
Double_2 Double_2::UnpackInterleaveLowHalf(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_unpacklo_pd(*reinterpret_cast<const __m128d*>(XY), *reinterpret_cast<const __m128d*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_unpcklpd(*reinterpret_cast<const v2df*>(XY), *reinterpret_cast<const v2df*>(other.XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(
        vzip1q_f64(*reinterpret_cast<const float64x2_t*>(XY), *reinterpret_cast<const float64x2_t*>(other.XY)));
#else
    return Double_2{X, other.X};
#endif
}
Double_2 Double_2::MoveOtherLowerToHigher(const Double_2& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromMM128D(
        _mm_move_sd(*reinterpret_cast<const __m128d*>(other.XY), *reinterpret_cast<const __m128d*>(XY)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateDouble_2FromV2DF(
        __builtin_ia32_movsd(*reinterpret_cast<const v2df*>(other.XY), *reinterpret_cast<const v2df*>(XY)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateDouble_2FromFloat64x2(vcombine_f64(vget_low_f64(*reinterpret_cast<const float64x2_t*>(XY)),
                                                    vget_low_f64(*reinterpret_cast<const float64x2_t*>(other.XY))));
#else
    return Double_2{X, other.X};
#endif
}

// Int32_4 helper functions
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
Int32_4 CreateInt32_4FromMM128I(__m128i value) {
    Int32_4 result;
    _mm_store_si128(reinterpret_cast<__m128i*>(result.XYZW), value);
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
Int32_4 CreateInt32_4FromV4SI(v4si value) {
    Int32_4 result;
    __builtin_ia32_storedqu(reinterpret_cast<char*>(result.XYZW), reinterpret_cast<v16qi>(value));
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_ARM64_SIMD
Int32_4 CreateInt32_4FromInt32x4(int32x4_t value) {
    Int32_4 result;
    vst1q_s32(result.XYZW, value);
    return result;
}
#endif

// Double_4 helper functions
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
Double_4 CreateDouble_4FromMM256D(__m256d value) {
    Double_4 result;
    _mm256_store_pd(result.XYZW, value);
    return result;
}
#endif
#if EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
Double_4 CreateDouble_4FromV4DF(v4df value) {
    Double_4 result;
    *reinterpret_cast<v4df*>(result.XYZW) = value;
    return result;
}
#endif

// Double_4 implementations
Double_4 Double_4::LoadAligned(const double* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(_mm256_load_pd(ptr));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(*reinterpret_cast<const v4df*>(ptr));
#else
    Double_4 result;
    result.XY_Double2 = Double_2::LoadAligned(ptr);
    result.ZW_Double2 = Double_2::LoadAligned(ptr + 2);
    return result;
#endif
}
Double_4 Double_4::LoadUnaligned(const double* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(_mm256_loadu_pd(ptr));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(__builtin_ia32_loadupd256(ptr));
#else
    Double_4 result;
    result.XY_Double2 = Double_2::LoadUnaligned(ptr);
    result.ZW_Double2 = Double_2::LoadUnaligned(ptr + 2);
    return result;
#endif
}
void Double_4::StoreAligned(double* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    _mm256_store_pd(ptr, *reinterpret_cast<const __m256d*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    *reinterpret_cast<v4df*>(ptr) = *reinterpret_cast<const v4df*>(XYZW);
#else
    XY_Double2.StoreAligned(ptr);
    ZW_Double2.StoreAligned(ptr + 2);
#endif
}
void Double_4::StoreUnaligned(double* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    _mm256_storeu_pd(ptr, *reinterpret_cast<const __m256d*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    __builtin_ia32_storeupd256(ptr, *reinterpret_cast<const v4df*>(XYZW));
#else
    XY_Double2.StoreUnaligned(ptr);
    ZW_Double2.StoreUnaligned(ptr + 2);
#endif
}
Double_4 Double_4::operator+(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_add_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_addpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 + other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 + other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::operator-(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_sub_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_subpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 - other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 - other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::operator*(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_mul_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_mulpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 * other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 * other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::operator/(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_div_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_divpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 / other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 / other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::operator|(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_or_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_orpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 | other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 | other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::operator&(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_and_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_andpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 & other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 & other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::operator^(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_xor_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_xorpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2 ^ other.XY_Double2;
    result.ZW_Double2 = ZW_Double2 ^ other.ZW_Double2;
    return result;
#endif
}
Double_4 Double_4::LessThan(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(_mm256_cmp_pd(*reinterpret_cast<const __m256d*>(XYZW),
                                                  *reinterpret_cast<const __m256d*>(other.XYZW), _CMP_LT_OQ));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_cmpltpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.LessThan(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.LessThan(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::GreaterThan(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(_mm256_cmp_pd(*reinterpret_cast<const __m256d*>(XYZW),
                                                  *reinterpret_cast<const __m256d*>(other.XYZW), _CMP_GT_OQ));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_cmpgtpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.GreaterThan(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.GreaterThan(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::Equal(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(_mm256_cmp_pd(*reinterpret_cast<const __m256d*>(XYZW),
                                                  *reinterpret_cast<const __m256d*>(other.XYZW), _CMP_EQ_OQ));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_cmpeqpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.Equal(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.Equal(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::HorizontalAdd(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_hadd_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_haddpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.HorizontalAdd(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.HorizontalAdd(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::Min(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_min_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_minpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.Min(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.Min(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::Max(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_max_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_maxpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.Max(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.Max(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::Dot(const Double_4& other) const {
    double dot = (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W);
    return Double_4{dot, dot, dot, dot};
}
Double_4 Double_4::DotLane0(const Double_4& other) const {
    double dot = (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W);
    return Double_4{dot, 0.0, 0.0, 0.0};
}
Double_4 Double_4::SquareRoot() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(_mm256_sqrt_pd(*reinterpret_cast<const __m256d*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(__builtin_ia32_sqrtpd256(*reinterpret_cast<const v4df*>(XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.SquareRoot();
    result.ZW_Double2 = ZW_Double2.SquareRoot();
    return result;
#endif
}
Double_4 Double_4::Normalize() const {
    Double_4 len = Length();
    return *this / len;
}
Double_4 Double_4::Length() const { return Dot(*this).SquareRoot(); }
Double_4 Double_4::LengthSquared() const { return Dot(*this); }
Double_4 Double_4::UnpackInterleaveLowHalf(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromMM256D(
        _mm256_unpacklo_pd(*reinterpret_cast<const __m256d*>(XYZW), *reinterpret_cast<const __m256d*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    return CreateDouble_4FromV4DF(
        __builtin_ia32_unpcklpd256(*reinterpret_cast<const v4df*>(XYZW), *reinterpret_cast<const v4df*>(other.XYZW)));
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2.UnpackInterleaveLowHalf(other.XY_Double2);
    result.ZW_Double2 = ZW_Double2.UnpackInterleaveLowHalf(other.ZW_Double2);
    return result;
#endif
}
Double_4 Double_4::MoveOtherLowerToHigher(const Double_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    // AVX doesn't have a direct equivalent, use permute + blend
    __m256d perm = _mm256_permute2f128_pd(*reinterpret_cast<const __m256d*>(XYZW),
                                          *reinterpret_cast<const __m256d*>(other.XYZW), 0x20);
    return CreateDouble_4FromMM256D(perm);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
    v4df perm = __builtin_ia32_vperm2f128_pd256(*reinterpret_cast<const v4df*>(XYZW),
                                                *reinterpret_cast<const v4df*>(other.XYZW), 0x20);
    return CreateDouble_4FromV4DF(perm);
#else
    Double_4 result;
    result.XY_Double2 = XY_Double2;
    result.ZW_Double2 = other.XY_Double2;
    return result;
#endif
}
void Double_4::MatrixMultiply4x4(const Double_4 matrix[4], const Double_4 otherTransposedMatrix[4],
                                 Double_4 result[4]) {
    for (int i = 0; i < 4; ++i) {
        const Double_4& rowF = matrix[i];

        const Double_4& col0F = otherTransposedMatrix[0];
        const Double_4& col1F = otherTransposedMatrix[1];
        const Double_4& col2F = otherTransposedMatrix[2];
        const Double_4& col3F = otherTransposedMatrix[3];

        const Double_4 d0F = rowF.DotLane0(col0F);
        const Double_4 d1F = rowF.DotLane0(col1F);
        const Double_4 d2F = rowF.DotLane0(col2F);
        const Double_4 d3F = rowF.DotLane0(col3F);

        const Double_4 loF = d0F.UnpackInterleaveLowHalf(d1F);
        const Double_4 hiF = d2F.UnpackInterleaveLowHalf(d3F);
        result[i] = loF.MoveOtherLowerToHigher(hiF);
    }
}
// Double_4 shuffles
Double_4 Double_4::ShuffleXYZW(const Double_4& other) const { return Double_4{X, Y, other.Z, other.W}; }
Double_4 Double_4::ShuffleYWZX(const Double_4& other) const { return Double_4{Y, W, other.Z, other.X}; }
Double_4 Double_4::ShuffleZWXY(const Double_4& other) const { return Double_4{Z, W, other.X, other.Y}; }
Double_4 Double_4::ShuffleWXYZ(const Double_4& other) const { return Double_4{W, X, other.Y, other.Z}; }
Double_4 Double_4::ShuffleXXYY(const Double_4& other) const { return Double_4{X, X, other.Y, other.Y}; }
Double_4 Double_4::ShuffleZZWW(const Double_4& other) const { return Double_4{Z, Z, other.W, other.W}; }
Double_4 Double_4::ShuffleXZXZ(const Double_4& other) const { return Double_4{X, Z, other.X, other.Z}; }
Double_4 Double_4::ShuffleYZYZ(const Double_4& other) const { return Double_4{Y, Z, other.Y, other.Z}; }
Double_4 Double_4::ShuffleWYWY(const Double_4& other) const { return Double_4{W, Y, other.W, other.Y}; }
Double_4 Double_4::ShuffleXYXY(const Double_4& other) const { return Double_4{X, Y, other.X, other.Y}; }
Double_4 Double_4::ShuffleZWZW(const Double_4& other) const { return Double_4{Z, W, other.Z, other.W}; }
Double_4 Double_4::ShuffleXXXX(const Double_4& other) const { return Double_4{X, X, other.X, other.X}; }
Double_4 Double_4::ShuffleYYYY(const Double_4& other) const { return Double_4{Y, Y, other.Y, other.Y}; }
Double_4 Double_4::ShuffleZZZZ(const Double_4& other) const { return Double_4{Z, Z, other.Z, other.Z}; }
Double_4 Double_4::ShuffleWWWW(const Double_4& other) const { return Double_4{W, W, other.W, other.W}; }

Int32_4 Int32_4::operator+(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_add_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_paddd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vaddq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vadd_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{X + other.X, Y + other.Y, Z + other.Z, W + other.W};
#endif
}
Int32_4 Int32_4::operator-(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_sub_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_psubd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vsubq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vsub_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{X - other.X, Y - other.Y, Z - other.Z, W - other.W};
#endif
}
Int32_4 Int32_4::operator*(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_mullo_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pmulld128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vmulq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vmul_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{X * other.X, Y * other.Y, Z * other.Z, W * other.W};
#endif
}
Int32_4 Int32_4::LoadAligned(const int32_t* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_load_si128(reinterpret_cast<const __m128i*>(ptr)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(*reinterpret_cast<const v4si*>(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vld1q_s32(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vld1_s32(ptr));
#else
    return Int32_4{ptr[0], ptr[1], ptr[2], ptr[3]};
#endif
}
Int32_4 Int32_4::LoadUnaligned(const int32_t* ptr) {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_loaddqu(reinterpret_cast<const char*>(ptr)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vld1q_s32(ptr));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vld1_s32(ptr));
#else
    return Int32_4{ptr[0], ptr[1], ptr[2], ptr[3]};
#endif
}
void Int32_4::StoreAligned(int32_t* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    _mm_store_si128(reinterpret_cast<__m128i*>(ptr), *reinterpret_cast<const __m128i*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    *reinterpret_cast<v4si*>(ptr) = *reinterpret_cast<const v4si*>(XYZW);
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    vst1q_s32(ptr, *reinterpret_cast<const int32x4_t*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    vst1_s32(ptr, *reinterpret_cast<const int32x4_t*>(XYZW));
#else
    ptr[0] = X;
    ptr[1] = Y;
    ptr[2] = Z;
    ptr[3] = W;
#endif
}
void Int32_4::StoreUnaligned(int32_t* ptr) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), *reinterpret_cast<const __m128i*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __builtin_ia32_storedqu(reinterpret_cast<char*>(ptr),
                            reinterpret_cast<const v16qi&>(*reinterpret_cast<const v4si*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    vst1q_s32(ptr, *reinterpret_cast<const int32x4_t*>(XYZW));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    vst1_s32(ptr, *reinterpret_cast<const int32x4_t*>(XYZW));
#else
    ptr[0] = X;
    ptr[1] = Y;
    ptr[2] = Z;
    ptr[3] = W;
#endif
}
Int32_4 Int32_4::LessThan(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_cmplt_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pcmpgtd128(*reinterpret_cast<const v4si*>(other.XYZW), *reinterpret_cast<const v4si*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vreinterpretq_s32_u32(
        vcltq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW))));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vreinterpret_s32_u32(
        vclt_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW))));
#else
    return Int32_4{(X < other.X) ? -1 : 0, (Y < other.Y) ? -1 : 0, (Z < other.Z) ? -1 : 0, (W < other.W) ? -1 : 0};
#endif
}
Int32_4 Int32_4::GreaterThan(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_cmpgt_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pcmpgtd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vreinterpretq_s32_u32(
        vcgtq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW))));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vreinterpret_s32_u32(
        vcgt_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW))));
#else
    return Int32_4{(X > other.X) ? -1 : 0, (Y > other.Y) ? -1 : 0, (Z > other.Z) ? -1 : 0, (W > other.W) ? -1 : 0};
#endif
}

Int32_4 Int32_4::Equal(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_cmpeq_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pcmpeqd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vreinterpretq_s32_u32(
        vceqq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW))));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vreinterpret_s32_u32(
        vceq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW))));
#else
    return Int32_4{(X == other.X) ? -1 : 0, (Y == other.Y) ? -1 : 0, (Z == other.Z) ? -1 : 0, (W == other.W) ? -1 : 0};
#endif
}

Int32_4 Int32_4::Min(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_min_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pminsd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vminq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vmin_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{(X < other.X) ? X : other.X, (Y < other.Y) ? Y : other.Y, (Z < other.Z) ? Z : other.Z,
                   (W < other.W) ? W : other.W};
#endif
}

Int32_4 Int32_4::Max(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_max_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pmaxsd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vmaxq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vmax_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{(X > other.X) ? X : other.X, (Y > other.Y) ? Y : other.Y, (Z > other.Z) ? Z : other.Z,
                   (W > other.W) ? W : other.W};
#endif
}

Int32_4 Int32_4::operator|(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_or_si128(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_por128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vorrq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vorr_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{X | other.X, Y | other.Y, Z | other.Z, W | other.W};
#endif
}
Int32_4 Int32_4::operator&(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_and_si128(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pand128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vandq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        vand_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{X & other.X, Y & other.Y, Z & other.Z, W & other.W};
#endif
}
Int32_4 Int32_4::operator^(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_xor_si128(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_pxor128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        veorq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(
        veor_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#else
    return Int32_4{X ^ other.X, Y ^ other.Y, Z ^ other.Z, W ^ other.W};
#endif
}
Int32_4 Int32_4::operator<<(int32_t shift) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_slli_epi32(*reinterpret_cast<const __m128i*>(XYZW), shift));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pslldi128(*reinterpret_cast<const v4si*>(XYZW), shift));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vshlq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), vdupq_n_s32(shift)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vshl_s32(*reinterpret_cast<const int32x4_t*>(XYZW), vdup_n_s32(shift)));
#else
    return Int32_4{X << shift, Y << shift, Z << shift, W << shift};
#endif
}
Int32_4 Int32_4::operator>>(int32_t shift) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_srai_epi32(*reinterpret_cast<const __m128i*>(XYZW), shift));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_psradi128(*reinterpret_cast<const v4si*>(XYZW), shift));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vshlq_s32(*reinterpret_cast<const int32x4_t*>(XYZW), vdupq_n_s32(-shift)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vshl_s32(*reinterpret_cast<const int32x4_t*>(XYZW), vdup_n_s32(-shift)));
#else
    return Int32_4{X >> shift, Y >> shift, Z >> shift, W >> shift};
#endif
}

Int32_4 Int32_4::UnpackInterleaveLowHalf(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_unpacklo_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_punpckldq128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vzip1q_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2x2_t zipped = vzip_s32(vget_low_s32(*reinterpret_cast<const int32x4_t*>(XYZW)),
                                  vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW)));
    return CreateInt32_4FromInt32x4(vcombine_s32(zipped.val[0], zipped.val[1]));
#else
    return Int32_4{X, other.X, Y, other.Y};
#endif
}
Int32_4 Int32_4::UnpackInterleaveHighHalf(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_unpackhi_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_punpckhdq128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(
        vzip2q_s32(*reinterpret_cast<const int32x4_t*>(XYZW), *reinterpret_cast<const int32x4_t*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2x2_t zipped = vzip_s32(vget_high_s32(*reinterpret_cast<const int32x4_t*>(XYZW)),
                                  vget_high_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW)));
    return CreateInt32_4FromInt32x4(vcombine_s32(zipped.val[0], zipped.val[1]));
#else
    return Int32_4{Z, other.Z, W, other.W};
#endif
}
Int32_4 Int32_4::HorizontalAdd(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(
        _mm_hadd_epi32(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(
        __builtin_ia32_phaddd128(*reinterpret_cast<const v4si*>(XYZW), *reinterpret_cast<const v4si*>(other.XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    int32x4_t a = *reinterpret_cast<const int32x4_t*>(XYZW);
    int32x4_t b = *reinterpret_cast<const int32x4_t*>(other.XYZW);
    return CreateInt32_4FromInt32x4(vpaddq_s32(a, b));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x4_t a = *reinterpret_cast<const int32x4_t*>(XYZW);
    int32x4_t b = *reinterpret_cast<const int32x4_t*>(other.XYZW);
    int32x2_t a_padd = vpadd_s32(vget_low_s32(a), vget_high_s32(a));
    int32x2_t b_padd = vpadd_s32(vget_low_s32(b), vget_high_s32(b));
    return CreateInt32_4FromInt32x4(vcombine_s32(a_padd, b_padd));
#else
    return Int32_4{(X + Y), (Z + W), (other.X + other.Y), (other.Z + other.W)};
#endif
}
Int32_4 Int32_4::MoveOtherLowerToHigher(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i lo =
        _mm_unpacklo_epi64(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW));
    return CreateInt32_4FromMM128I(lo);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2di lo =
        __builtin_ia32_punpcklqdq128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(lo));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2_t low = vget_low_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t high = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(low, high));
#else
    return Int32_4{X, Y, other.X, other.Y};
#endif
}

Int32_4 Int32_4::ShuffleXYZW(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i lo =
        _mm_unpacklo_epi64(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW));
    return CreateInt32_4FromMM128I(lo);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2di lo =
        __builtin_ia32_punpcklqdq128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(lo));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    int32x2_t thisLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisLo, otherLo));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2_t thisLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisLo, otherLo));
#else
    return Int32_4{X, Y, other.Z, other.W};
#endif
}
Int32_4 Int32_4::ShuffleYWZX(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(3, 3, 3, 1));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(0, 2, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(3, 3, 3, 1));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(0, 2, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Y, W, other.Z, other.X};
    return LoadUnaligned(vals);
#else
    return Int32_4{Y, W, other.Z, other.X};
#endif
}
Int32_4 Int32_4::ShuffleZWXY(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i hi =
        _mm_unpackhi_epi64(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW));
    return CreateInt32_4FromMM128I(hi);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2di hi =
        __builtin_ia32_punpckhqdq128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(hi));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    int32x2_t thisHi = vget_high_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisHi, otherLo));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2_t thisHi = vget_high_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisHi, otherLo));
#else
    return Int32_4{Z, W, other.X, other.Y};
#endif
}
Int32_4 Int32_4::ShuffleWXYZ(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(0, 0, 0, 3));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(2, 1, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(0, 0, 0, 3));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(2, 1, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {W, X, other.Y, other.Z};
    return LoadUnaligned(vals);
#else
    return Int32_4{W, X, other.Y, other.Z};
#endif
}
Int32_4 Int32_4::ShuffleXXYY(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(0, 0, 0, 0));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(1, 1, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(0, 0, 0, 0));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(1, 1, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {X, X, other.Y, other.Y};
    return LoadUnaligned(vals);
#else
    return Int32_4{X, X, other.Y, other.Y};
#endif
}
Int32_4 Int32_4::ShuffleZZWW(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 2, 0, 0));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(3, 3, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 2, 0, 0));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(3, 3, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Z, Z, other.W, other.W};
    return LoadUnaligned(vals);
#else
    return Int32_4{Z, Z, other.W, other.W};
#endif
}
Int32_4 Int32_4::ShuffleXZXZ(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 0, 0, 0));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(2, 0, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 0, 0, 0));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(2, 0, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {X, Z, other.X, other.Z};
    return LoadUnaligned(vals);
#else
    return Int32_4{X, Z, other.X, other.Z};
#endif
}
Int32_4 Int32_4::ShuffleYZYZ(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 1, 0, 0));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(2, 1, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 1, 0, 0));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(2, 1, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Y, Z, other.Y, other.Z};
    return LoadUnaligned(vals);
#else
    return Int32_4{Y, Z, other.Y, other.Z};
#endif
}
Int32_4 Int32_4::ShuffleWYWY(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i thisShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 3, 0, 0));
    __m128i otherShuf = _mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(other.XYZW), _MM_SHUFFLE(1, 3, 0, 0));
    __m128i result = _mm_unpacklo_epi64(thisShuf, otherShuf);
    return CreateInt32_4FromMM128I(result);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v4si thisShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 3, 0, 0));
    v4si otherShuf = __builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(other.XYZW), _MM_SHUFFLE(1, 3, 0, 0));
    v2di result =
        __builtin_ia32_punpcklqdq128(reinterpret_cast<const v2di&>(thisShuf), reinterpret_cast<const v2di&>(otherShuf));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(result));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {W, Y, other.W, other.Y};
    return LoadUnaligned(vals);
#else
    return Int32_4{W, Y, other.W, other.Y};
#endif
}
Int32_4 Int32_4::ShuffleXYXY(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i lo =
        _mm_unpacklo_epi64(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW));
    return CreateInt32_4FromMM128I(lo);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2di lo =
        __builtin_ia32_punpcklqdq128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(lo));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    int32x2_t thisLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisLo, otherLo));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2_t thisLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherLo = vget_low_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisLo, otherLo));
#else
    return Int32_4{X, Y, other.X, other.Y};
#endif
}
Int32_4 Int32_4::ShuffleZWZW(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    __m128i hi =
        _mm_unpackhi_epi64(*reinterpret_cast<const __m128i*>(XYZW), *reinterpret_cast<const __m128i*>(other.XYZW));
    return CreateInt32_4FromMM128I(hi);
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    v2di hi =
        __builtin_ia32_punpckhqdq128(*reinterpret_cast<const v2di*>(XYZW), *reinterpret_cast<const v2di*>(other.XYZW));
    return CreateInt32_4FromV4SI(reinterpret_cast<const v4si&>(hi));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    int32x2_t thisHi = vget_high_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherHi = vget_high_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisHi, otherHi));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32x2_t thisHi = vget_high_s32(*reinterpret_cast<const int32x4_t*>(XYZW));
    int32x2_t otherHi = vget_high_s32(*reinterpret_cast<const int32x4_t*>(other.XYZW));
    return CreateInt32_4FromInt32x4(vcombine_s32(thisHi, otherHi));
#else
    return Int32_4{Z, W, other.Z, other.W};
#endif
}
Int32_4 Int32_4::ShuffleXXXX(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(0, 0, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(0, 0, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(X));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdup_n_s32(X));
#else
    return Int32_4{X, X, other.X, other.X};
#endif
}
Int32_4 Int32_4::ShuffleYYYY(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 1, 1, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 1, 1, 1)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(Y));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdup_n_s32(Y));
#else
    return Int32_4{Y, Y, other.Y, other.Y};
#endif
}
Int32_4 Int32_4::ShuffleZZZZ(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 2, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 2, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(Z));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdup_n_s32(Z));
#else
    return Int32_4{Z, Z, other.Z, other.Z};
#endif
}
Int32_4 Int32_4::ShuffleWWWW(const Int32_4& other) const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(3, 3, 3, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(3, 3, 3, 3)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(W));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdup_n_s32(W));
#else
    return Int32_4{W, W, other.W, other.W};
#endif
}

// Self-shuffle implementations (single vector shuffles)
Int32_4 Int32_4::ShuffleXYZW() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(3, 2, 1, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(3, 2, 1, 0)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    return *this;
#else
    return Int32_4{X, Y, Z, W};
#endif
}

Int32_4 Int32_4::ShuffleYWZX() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(0, 2, 3, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(0, 2, 3, 1)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Y, W, Z, X};
    return LoadUnaligned(vals);
#else
    return Int32_4{Y, W, Z, X};
#endif
}

Int32_4 Int32_4::ShuffleZWXY() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 0, 3, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 0, 3, 2)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vrev64q_s32(*reinterpret_cast<const int32x4_t*>(XYZW)));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vrev64q_s32(*reinterpret_cast<const int32x4_t*>(XYZW)));
#else
    return Int32_4{Z, W, X, Y};
#endif
}

Int32_4 Int32_4::ShuffleWXYZ() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 1, 0, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 1, 0, 3)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {W, X, Y, Z};
    return LoadUnaligned(vals);
#else
    return Int32_4{W, X, Y, Z};
#endif
}

Int32_4 Int32_4::ShuffleXXYY() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 1, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 1, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {X, X, Y, Y};
    return LoadUnaligned(vals);
#else
    return Int32_4{X, X, Y, Y};
#endif
}

Int32_4 Int32_4::ShuffleZZWW() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(3, 3, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(3, 3, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Z, Z, W, W};
    return LoadUnaligned(vals);
#else
    return Int32_4{Z, Z, W, W};
#endif
}

Int32_4 Int32_4::ShuffleXZXZ() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 0, 2, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 0, 2, 0)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {X, Z, X, Z};
    return LoadUnaligned(vals);
#else
    return Int32_4{X, Z, X, Z};
#endif
}

Int32_4 Int32_4::ShuffleYZYZ() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 1, 2, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 1, 2, 1)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Y, Z, Y, Z};
    return LoadUnaligned(vals);
#else
    return Int32_4{Y, Z, Y, Z};
#endif
}

Int32_4 Int32_4::ShuffleWYWY() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 3, 1, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 3, 1, 3)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {W, Y, W, Y};
    return LoadUnaligned(vals);
#else
    return Int32_4{W, Y, W, Y};
#endif
}

Int32_4 Int32_4::ShuffleXYXY() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 0, 1, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 0, 1, 0)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {X, Y, X, Y};
    return LoadUnaligned(vals);
#else
    return Int32_4{X, Y, X, Y};
#endif
}

Int32_4 Int32_4::ShuffleZWZW() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(3, 2, 3, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(3, 2, 3, 2)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD || EDVAR_CPP_CORE_MATH_ARM_SIMD
    int32_t vals[4] = {Z, W, Z, W};
    return LoadUnaligned(vals);
#else
    return Int32_4{Z, W, Z, W};
#endif
}

Int32_4 Int32_4::ShuffleXXXX() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(0, 0, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(0, 0, 0, 0)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(X));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(X));
#else
    return Int32_4{X, X, X, X};
#endif
}

Int32_4 Int32_4::ShuffleYYYY() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(1, 1, 1, 1)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(1, 1, 1, 1)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(Y));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(Y));
#else
    return Int32_4{Y, Y, Y, Y};
#endif
}

Int32_4 Int32_4::ShuffleZZZZ() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(2, 2, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(2, 2, 2, 2)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(Z));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(Z));
#else
    return Int32_4{Z, Z, Z, Z};
#endif
}

Int32_4 Int32_4::ShuffleWWWW() const {
#if EDVAR_CPP_CORE_MATH_MSVC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromMM128I(_mm_shuffle_epi32(*reinterpret_cast<const __m128i*>(XYZW), _MM_SHUFFLE(3, 3, 3, 3)));
#elif EDVAR_CPP_CORE_MATH_GCC_INTRINSICS && EDVAR_CPP_CORE_MATH_X86_SIMD
    return CreateInt32_4FromV4SI(__builtin_ia32_pshufd(*reinterpret_cast<const v4si*>(XYZW), _MM_SHUFFLE(3, 3, 3, 3)));
#elif EDVAR_CPP_CORE_MATH_ARM64_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(W));
#elif EDVAR_CPP_CORE_MATH_ARM_SIMD
    return CreateInt32_4FromInt32x4(vdupq_n_s32(W));
#else
    return Int32_4{W, W, W, W};
#endif
}
} // namespace SIMD

double Cos(double angleRadians) { return std::cos(angleRadians); }
float Cos(float angleRadians) { return std::cos(angleRadians); }
double Sin(double angleRadians) { return std::sin(angleRadians); }
float Sin(float angleRadians) { return std::sin(angleRadians); }
double Tan(double angleRadians) { return std::tan(angleRadians); }
float Tan(float angleRadians) { return std::tan(angleRadians); }
double ACos(double value) { return std::acos(value); }
float ACos(float value) { return std::acos(value); }
double ASin(double value) { return std::asin(value); }
float ASin(float value) { return std::asin(value); }
float ATan(float value) { return std::atan(value); }
double ATan(double value) { return std::atan(value); }
double ATan2(double y, double x) { return std::atan2(y, x); }
float ATan2(float y, float x) { return std::atan2(y, x); }
bool IsInfinite(double value) { return std::isinf(value); }
bool IsInfinite(float value) { return std::isinf(value); }
bool IsNaN(double value) { return std::isnan(value); }
bool IsNaN(float value) { return std::isnan(value); }
} // namespace Edvar::Math