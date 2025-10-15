#include "math/math.h"
#include "diagnostics/assertion.h"
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

double cos(const double& radians) {
    // Normalize the angle to the range [0, 2π)
    double angle = radians - (2.0 * PI) * floor(radians / (2.0 * PI));

    // Use the Taylor series expansion for cos(x) around 0
    double term = 1.0; // First term of the series
    double result = term;
    double x_squared = angle * angle;

    for (int n = 1; n <= 10; ++n) {
        term *= -x_squared / ((2 * n - 1) * (2 * n)); // Calculate the next term
        result += term;
    }

    return result;
}

double sin(const double& radians) {
    // Normalize the angle to the range [0, 2π)
    double angle = radians - (2.0 * PI) * floor(radians / (2.0 * PI));

    // Use the Taylor series expansion for sin(x) around 0
    double term = angle; // First term of the series
    double result = term;
    double x_squared = angle * angle;

    for (int n = 1; n <= 10; ++n) {
        term *= -x_squared / ((2 * n) * (2 * n + 1)); // Calculate the next term
        result += term;
    }

    return result;
}

double tan(const double& radians) {
    double cosine = cos(radians);
    if (cosine == 0.0) {
        edvar::diagnostics::check(false, L"tan() undefined for angles where cos() is zero.");
        return 0.0; // Undefined, return 0.0 to avoid division by zero
    }
    return sin(radians) / cosine;
}

double acos(const double& value) {
    if (value < -1.0 || value > 1.0) {
        edvar::diagnostics::check(false, L"acos() input out of domain [-1, 1].");
        return 0.0; // Out of domain, return 0.0
    }
    // Use the Taylor series expansion for acos(x) around 0
    double term = value;             // First term of the series
    double result = (PI / 2) - term; // acos(0) = π/2
    double x_squared = value * value;

    for (int n = 1; n <= 10; ++n) {
        term *= x_squared * (2 * n - 1) / (2 * n); // Calculate the next term
        result -= term / (2 * n + 1);
    }

    return result;
}

double asin(const double& value) {
    if (value < -1.0 || value > 1.0) {
        edvar::diagnostics::check(false, L"asin() input out of domain [-1, 1].");
        return 0.0; // Out of domain, return 0.0
    }
    // Use the Taylor series expansion for asin(x) around 0
    double term = value; // First term of the series
    double result = term;
    double x_squared = value * value;

    for (int n = 1; n <= 10; ++n) {
        term *= x_squared * (2 * n - 1) / (2 * n); // Calculate the next term
        result += term / (2 * n + 1);
    }

    return result;
}

double atan(const double& value) {
    // Use the Taylor series expansion for atan(x) around 0
    if (value > 1.0) {
        return (PI / 2) - atan(1.0 / value);
    } else if (value < -1.0) {
        return -(PI / 2) - atan(1.0 / value);
    }

    double term = value; // First term of the series
    double result = term;
    double x_squared = value * value;

    for (int n = 1; n <= 10; ++n) {
        term *= -x_squared; // Calculate the next term
        result += term / (2 * n + 1);
    }

    return result;
}

} // namespace edvar::math