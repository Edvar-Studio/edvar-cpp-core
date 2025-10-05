#pragma once

namespace edvar::math {

static constexpr double PI = 3.14159265358979323846;
static constexpr double TWO_PI = 2.0 * PI;
static constexpr double HALF_PI = PI / 2.0;
static constexpr double E = 2.71828182845904523536;
static constexpr double E_SQUARE = E * E;
static constexpr double GOLDEN_RATIO = 1.618033988749895;

static constexpr double SMALL_NUMBER = 1e-10;
static constexpr double VERY_SMALL_NUMBER = 1e-20;
static constexpr double LARGE_NUMBER = 1e+10;
static constexpr double VERY_LARGE_NUMBER = 1e+20;

inline auto abs(const auto& value) -> decltype(value) { return value < 0 ? -value : value; }
inline auto min(const auto& a, const auto& b) -> decltype(a) { return a < b ? a : b; }
inline auto max(const auto& a, const auto& b) -> decltype(a) { return a > b ? a : b; }
inline auto clamp(const auto& value, const auto& min_value, const auto& max_value) -> decltype(value) {
    return min(max(value, min_value), max_value);
}
inline auto square(const auto& value) -> decltype(value * value) { return value * value; }
inline auto cube(const auto& value) -> decltype(value * value * value) { return value * value * value; }
inline auto floor(const auto& value) -> decltype(value) {
    using return_type = decltype(value);
    if constexpr (meta::is_unsigned<meta::remove_cv<meta::remove_reference<return_type>>>) {
        return static_cast<return_type>(static_cast<uint64>(value));
    }
    return static_cast<decltype(value)>(static_cast<int64>(value));
}
inline auto ceil(const auto& value) -> decltype(value) {
    using return_type = decltype(value);
    if constexpr (meta::is_unsigned<meta::remove_cv<meta::remove_reference<return_type>>>) {
        return static_cast<return_type>(
            static_cast<uint64>(value + (value == static_cast<return_type>(static_cast<uint64>(value)) ? 0 : 1)));
    }
    return static_cast<decltype(value)>(
        static_cast<int64>(value + (value == static_cast<return_type>(static_cast<int64>(value)) ? 0 : 1)));
}

double ln(const double& value);
float ln(const float& value);
inline int ln(const int& value) { return static_cast<int>(math::ln(static_cast<double>(value))); }
inline int64 ln(const int64& value) { return static_cast<int64>(math::ln(static_cast<double>(value))); }

double log(const double& value, const double& base);
inline float log(const float& value, const float& base) {
    return static_cast<float>(math::log(static_cast<double>(value), static_cast<double>(base)));
}
int log(const int& value, const int& base);
int64 log(const int64& value, const int64& base);

double exp(const double& value, const double& tolerance = 1e-10);
inline float exp(const float& value) { return static_cast<float>(math::exp(static_cast<double>(value))); }

int power(const int& value, const int& exponent);
inline double power(const double& value, const double& exponent) { return math::exp(math::ln(value) * exponent); }
inline float power(const float& value, const float& exponent) {
    return static_cast<float>(math::power(static_cast<double>(value), static_cast<double>(exponent)));
}
inline double power(const double& value, const int& exponent) {
    if (exponent == 0) [[unlikely]] {
        return 1.0;
    }
    double result = value;
    for (int i = 1; i < exponent; ++i) {
        result *= value;
    }
    return result;
}

double square_root(const int& value, const double& tolerance = 1e-6);
double square_root(const uint32& value, const double& tolerance = 1e-6);
double square_root(const int64& value, const double& tolerance = 1e-6);
double square_root(const uint64& value, const double& tolerance = 1e-6);
double square_root(const double& value, const double& tolerance = 1e-6);
double square_root(const float& value, const double& tolerance = 1e-6f);

} // namespace edvar::math