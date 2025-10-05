#include "math/math.h"
#include "math/simd_support.h"

namespace edvar::math {

double ln(const double& value) {
    if (value <= 0.0) [[unlikely]] {
        return 0.0;
    }
    if (value == 1.0) [[unlikely]] {
        return 0.0;
    }
    if (value == E) [[unlikely]] {
        return 1.0;
    }
    if (value == E_SQUARE) [[unlikely]] {
        return 2.0;
    }
    // Fast natural logarithm approximation using bit manipulation and a polynomial
    union {
        double d;
        uint64 i;
    } u = {value};

    // Extract exponent and mantissa
    int exponent = ((u.i >> 52) & 0x7FF) - 1023;
    double mantissa = (u.i & 0xFFFFFFFFFFFFF) / static_cast<double>(0x10000000000000) + 1.0;

    // Polynomial approximation for ln(mantissa) in [1,2)
    // ln(mantissa) ≈ (mantissa - 1) - 0.5*(mantissa - 1)^2 + (1/3)*(mantissa - 1)^3
    double x = mantissa - 1.0;
    double x2 = x * x;
    double x3 = x2 * x;
    double ln_mantissa = x - 0.5 * x2 + (1.0 / 3.0) * x3;

    // ln(value) = ln(mantissa * 2^exponent) = ln(mantissa) + exponent * ln(2)
    constexpr double LN2 = 0.6931471805599453;
    return ln_mantissa + exponent * LN2;
}

double log(const double& value, const double& base) {
    if (base <= 1.0) [[unlikely]] {
        return 0.0;
    }
    return ln(value) / ln(base);
}
int64 log(const int64& value, const int64& base) {
    if (base <= 1) [[unlikely]] {
        return 0;
    }
    uint64 result = 0;
    uint64 current = value;
    while (current >= base) {
        current /= base;
        result++;
    }
    return result;
}
int log(const int& value, const int& base) {
    if (base <= 1) [[unlikely]] {
        return 0;
    }
    uint32 result = 0;
    uint32 current = value;
    while (current >= base) {
        current /= base;
        result++;
    }
    return result;
}
float ln(const float& value) {
    if (value <= 0.0f) [[unlikely]] {
        return 0.0f;
    }
    if (value == 1.0f) [[unlikely]] {
        return 0.0f;
    }
    if (value == static_cast<float>(E)) [[unlikely]] {
        return 1.0f;
    }
    if (value == static_cast<float>(E_SQUARE)) [[unlikely]] {
        return 2.0f;
    }
    // Fast natural logarithm approximation using bit manipulation and a polynomial
    union {
        float f;
        uint32_t i;
    } u = {value};

    // Extract exponent and mantissa
    int exponent = ((u.i >> 23) & 0xFF) - 127;
    float mantissa = (u.i & 0x7FFFFF) / static_cast<float>(0x800000) + 1.0f;

    // Polynomial approximation for ln(mantissa) in [1,2)
    // ln(mantissa) ≈ (mantissa - 1) - 0.5*(mantissa - 1)^2 + (1/3)*(mantissa - 1)^3
    float x = mantissa - 1.0f;
    float x2 = x * x;
    float x3 = x2 * x;
    float ln_mantissa = x - 0.5f * x2 + (1.0f / 3.0f) * x3;

    // ln(value) = ln(mantissa * 2^exponent) = ln(mantissa) + exponent * ln(2)
    constexpr float LN2 = 0.69314718f;
    return ln_mantissa + exponent * LN2;
}
int power(const int& value, const int& exponent) {
    if (exponent == 0) [[unlikely]] {
        return 1;
    }
    int result = value;
    for (int i = 1; i < exponent; ++i) {
        result *= value;
    }
    return result;
}
double exp(const double& value, const double& tolerance) {
    if (value == 0.0) [[unlikely]] {
        return 1.0;
    }
    if (value == 1.0) [[unlikely]] {
        return E;
    }
    if (value == 2.0) [[unlikely]] {
        return E_SQUARE;
    }
    // Exponential function approximation using Taylor series expansion with tolerance
    double result = 1.0;
    double term = 1.0;
    for (int n = 1; n <= 20; ++n) {
        term *= value / n;
        if (math::abs(term) < tolerance)
            break;
        result += term;
    }
    return result;
}

double square_root(const uint32& value, const double& tolerance) {
    simd::simd_128d vec(0, static_cast<double>(value));
    vec.sqrt_inline();
    return vec.y;
}

double square_root(const int& value, const double& tolerance) {
    simd::simd_128d vec(0, static_cast<double>(value));
    vec.sqrt_inline();
    return vec.y;
}

double square_root(const float& value, const double& tolerance) {
    simd::simd_128f vec(0, 0, 0, value);
    vec.sqrt_inline();
    return vec.z;
}

double square_root(const double& value, const double& tolerance) {
    simd::simd_128d vec(0, value);
    vec.sqrt_inline();
    return vec.y;
}

double square_root(const int64& value, const double& tolerance) {
    simd::simd_128d vec(0, static_cast<double>(value));
    vec.sqrt_inline();
    return vec.y;
}

double square_root(const uint64& value, const double& tolerance) {
    simd::simd_128d vec(0, static_cast<double>(value));
    vec.sqrt_inline();
    return vec.y;
}

} // namespace edvar::math