#pragma once

#include "EdvarCore.hpp"
namespace Edvar::Math {
static constexpr double PI = 3.14159265358979323846;
static constexpr double TWO_PI = 2.0 * PI;
static constexpr double HALF_PI = PI / 2.0;
static constexpr double INV_PI = 1.0 / PI;
static constexpr double INV_TWO_PI = 1.0 / TWO_PI;
static constexpr double E = 2.71828182845904523536;

static constexpr double LOG2E = 1.44269504088896340736;   // log2(e)
static constexpr double LOG10E = 0.434294481903251827651; // log10(e)
static constexpr double LN2 = 0.693147180559945309417;    // ln(2)
static constexpr double LN10 = 2.30258509299404568402;    // ln(10)

static constexpr double VERY_SMALL_NUMBER = 1e-10;
static constexpr double SMALL_NUMBER = 1e-6;
static constexpr double BIG_NUMBER = 1e+10;
static constexpr double VERY_BIG_NUMBER = 1e+20;

template <typename A, typename B>
EDVAR_CPP_CORE_FORCE_INLINE auto Min(const A& a, const B& b) -> decltype(a < b ? a : b) {
    return (a < b) ? a : b;
}

template <typename A, typename B, typename... Rest>
auto Min(const A& a, const B& b, const Rest&... rest) -> decltype(a) {
    return Min(a, Min(b, rest...));
}

template <typename A, typename B>
EDVAR_CPP_CORE_FORCE_INLINE auto Max(const A& a, const B& b) -> decltype(a > b ? a : b) {
    return (a > b) ? a : b;
}
template <typename A, typename B, typename C>
EDVAR_CPP_CORE_FORCE_INLINE auto Clamp(const A& value, const B& minValue, const C& maxValue) -> decltype(value) {
    return Min(Max(value, minValue), maxValue);
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Lerp(const T& a, const T& b, float t) { return a + (b - a) * t; }
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Saturate(const T& value) {
    return Clamp(value, static_cast<T>(0), static_cast<T>(1));
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T SmoothStep(const T& edge0, const T& edge1, const T& x) {
    T t = Clamp((x - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}
template <typename A> EDVAR_CPP_CORE_FORCE_INLINE auto Abs(const A& value) -> decltype(value < 0 ? -value : value) {
    return (value < 0) ? -value : value;
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Square(const T& value) { return value * value; }
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Cube(const T& value) { return value * value * value; }
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Sign(const T& value) {
    if (value > static_cast<T>(0)) {
        return static_cast<T>(1);
    } else if (value < static_cast<T>(0)) {
        return static_cast<T>(-1);
    } else {
        return static_cast<T>(0);
    }
}

template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T DegToRad(const T& degrees) {
    return degrees * static_cast<T>(PI) / static_cast<T>(180);
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T RadToDeg(const T& radians) {
    return radians * static_cast<T>(180) / static_cast<T>(PI);
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T WrapAngleDegrees(const T& angle) {
    T wrappedAngle = angle;
    while (wrappedAngle <= static_cast<T>(-180)) {
        wrappedAngle += static_cast<T>(360);
    }
    while (wrappedAngle > static_cast<T>(180)) {
        wrappedAngle -= static_cast<T>(360);
    }
    return wrappedAngle;
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T WrapAngleRadians(const T& angle) {
    T wrappedAngle = angle;
    const T twoPi = static_cast<T>(2) * static_cast<T>(PI);
    while (wrappedAngle <= static_cast<T>(-PI)) {
        wrappedAngle += twoPi;
    }
    while (wrappedAngle > static_cast<T>(PI)) {
        wrappedAngle -= twoPi;
    }
    return wrappedAngle;
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE bool IsPowerOfTwo(T value) {
    return (value > 0) && ((value & (value - 1)) == 0);
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T NextPowerOfTwo(T value) {
    if (value <= 0) {
        return 1;
    }
    --value;
    for (size_t i = 1; i < sizeof(T) * 8; i *= 2) {
        value |= value >> i;
    }
    return value + 1;
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T PreviousPowerOfTwo(T value) {
    if (value <= 1) {
        return 1;
    }
    T power = 1;
    while (power * 2 < value) {
        power *= 2;
    }
    return power;
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Factorial(T n) {
    if (n < 0) {
        return 0; // Factorial is not defined for negative numbers
    }
    T result = 1;
    for (T i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}
template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Combination(T n, T k) {
    if (k < 0 || k > n) {
        return 0;
    }
    if (k == 0 || k == n) {
        return 1;
    }
    k = Min(k, n - k); // Take advantage of symmetry
    T result = 1;
    for (T i = 0; i < k; ++i) {
        result = result * (n - i) / (i + 1);
    }
    return result;
}

EDVAR_CPP_CORE_API double Power(double base, double exponent);
EDVAR_CPP_CORE_API float Power(float base, float exponent);

EDVAR_CPP_CORE_API double NaturalLogarithm(double value);
EDVAR_CPP_CORE_API float NaturalLogarithm(float value);

EDVAR_CPP_CORE_API double BinaryLogarithm(double value);
EDVAR_CPP_CORE_API float BinaryLogarithm(float value);

EDVAR_CPP_CORE_API double Logarithm(double value, double base);
EDVAR_CPP_CORE_API float Logarithm(float value, float base);

EDVAR_CPP_CORE_API float SquareRoot(float value);
EDVAR_CPP_CORE_API double SquareRoot(double value);

EDVAR_CPP_CORE_API double InverseSquareRoot(double value);
EDVAR_CPP_CORE_API float InverseSquareRoot(float value);

EDVAR_CPP_CORE_API double Root(double value, double degree);
EDVAR_CPP_CORE_API float Root(float value, float degree);

EDVAR_CPP_CORE_API double InverseRoot(double value, double degree);
EDVAR_CPP_CORE_API float InverseRoot(float value, float degree);

EDVAR_CPP_CORE_API double Exp(double exponent);
EDVAR_CPP_CORE_API float Exp(float exponent);

EDVAR_CPP_CORE_API double Cos(double angleRadians);
EDVAR_CPP_CORE_API float Cos(float angleRadians);

EDVAR_CPP_CORE_API double Sin(double angleRadians);
EDVAR_CPP_CORE_API float Sin(float angleRadians);

EDVAR_CPP_CORE_API double Tan(double angleRadians);
EDVAR_CPP_CORE_API float Tan(float angleRadians);

EDVAR_CPP_CORE_API double ACos(double value);
EDVAR_CPP_CORE_API float ACos(float value);

EDVAR_CPP_CORE_API double ASin(double value);
EDVAR_CPP_CORE_API float ASin(float value);

EDVAR_CPP_CORE_API double ATan(double value);
EDVAR_CPP_CORE_API float ATan(float value);

EDVAR_CPP_CORE_API double ATan2(double y, double x);
EDVAR_CPP_CORE_API float ATan2(float y, float x);

template <typename T, typename U>
inline bool IsNearlyEqual(T a, U b, double tolerance = static_cast<float>(SMALL_NUMBER))
    requires(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>)
{
    return Abs(a - b) <= tolerance;
}
EDVAR_CPP_CORE_API bool IsInfinite(double value);
EDVAR_CPP_CORE_API bool IsInfinite(float value);

EDVAR_CPP_CORE_API bool IsNaN(double value);
EDVAR_CPP_CORE_API bool IsNaN(float value);

template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T CopySign(const T& magnitude, const T& signSource) {
    return (signSource >= static_cast<T>(0)) ? Abs(magnitude) : -Abs(magnitude);
}

template <typename T> EDVAR_CPP_CORE_FORCE_INLINE T Round(const T& value) {
    return value < 0 ? static_cast<T>(value - 0.5) : static_cast<T>(value + 0.5);
}

namespace SIMD {
#define DECLARE_4COMPONENT_SHUFFLES(type)                                                                              \
    type ShuffleXYZW(const type& other) const;                                                                         \
    type ShuffleYWZX(const type& other) const;                                                                         \
    type ShuffleZWXY(const type& other) const;                                                                         \
    type ShuffleWXYZ(const type& other) const;                                                                         \
    type ShuffleXXYY(const type& other) const;                                                                         \
    type ShuffleZZWW(const type& other) const;                                                                         \
    type ShuffleXZXZ(const type& other) const;                                                                         \
    type ShuffleYZYZ(const type& other) const;                                                                         \
    type ShuffleWYWY(const type& other) const;                                                                         \
    type ShuffleXYXY(const type& other) const;                                                                         \
    type ShuffleZWZW(const type& other) const;                                                                         \
    type ShuffleXXXX(const type& other) const;                                                                         \
    type ShuffleYYYY(const type& other) const;                                                                         \
    type ShuffleZZZZ(const type& other) const;                                                                         \
    type ShuffleWWWW(const type& other) const;

#define DECLARE_4COMPONENT_SELF_SHUFFLES_USENONSELF(type)                                                              \
    type ShuffleXYZW() const { return ShuffleXYZW(*this); }                                                            \
    type ShuffleYWZX() const { return ShuffleYWZX(*this); }                                                            \
    type ShuffleZWXY() const { return ShuffleZWXY(*this); }                                                            \
    type ShuffleWXYZ() const { return ShuffleWXYZ(*this); }                                                            \
    type ShuffleXXYY() const { return ShuffleXXYY(*this); }                                                            \
    type ShuffleZZWW() const { return ShuffleZZWW(*this); }                                                            \
    type ShuffleXZXZ() const { return ShuffleXZXZ(*this); }                                                            \
    type ShuffleYZYZ() const { return ShuffleYZYZ(*this); }                                                            \
    type ShuffleWYWY() const { return ShuffleWYWY(*this); }                                                            \
    type ShuffleXYXY() const { return ShuffleXYXY(*this); }                                                            \
    type ShuffleZWZW() const { return ShuffleZWZW(*this); }                                                            \
    type ShuffleXXXX() const { return ShuffleXXXX(*this); }                                                            \
    type ShuffleYYYY() const { return ShuffleYYYY(*this); }                                                            \
    type ShuffleZZZZ() const { return ShuffleZZZZ(*this); }                                                            \
    type ShuffleWWWW() const { return ShuffleWWWW(*this); }

#define DECLARE_4COMPONENT_SELF_SHUFFLES(type)                                                                         \
    type ShuffleXYZW() const;                                                                                          \
    type ShuffleYWZX() const;                                                                                          \
    type ShuffleZWXY() const;                                                                                          \
    type ShuffleWXYZ() const;                                                                                          \
    type ShuffleXXYY() const;                                                                                          \
    type ShuffleZZWW() const;                                                                                          \
    type ShuffleXZXZ() const;                                                                                          \
    type ShuffleYZYZ() const;                                                                                          \
    type ShuffleWYWY() const;                                                                                          \
    type ShuffleXYXY() const;                                                                                          \
    type ShuffleZWZW() const;                                                                                          \
    type ShuffleXXXX() const;                                                                                          \
    type ShuffleYYYY() const;                                                                                          \
    type ShuffleZZZZ() const;                                                                                          \
    type ShuffleWWWW() const;
struct alignas(16) Float_4 {
    Float_4() : X(0), Y(0), Z(0), W(0) {}
    Float_4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}
    union {
        struct {
            float X, Y, Z, W;
        };
        float XYZW[4];
    };

    template <int32_t Index> float Get() const {
        static_assert(Index >= 0 && Index < 4, "Float_4::Get: Index out of bounds.");
        if constexpr (Index == 0)
            return X;
        else if constexpr (Index == 1)
            return Y;
        else if constexpr (Index == 2)
            return Z;
        else /* Index == 3 */
            return W;
    }

    static constexpr int32_t ElementCount = 4;
    static constexpr int32_t PerElementSize = sizeof(float);

    /**
     * @brief Loads a Float_4 from an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 16-byte aligned. 4 floats (16 bytes) will be read.
     * @return The loaded Float_4.
     */
    static Float_4 LoadAligned(const float* ptr);
    /**
     * @brief Loads a Float_4 from an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 4 floats (16 bytes) will be read.
     * @return The loaded Float_4.
     */
    static Float_4 LoadUnaligned(const float* ptr);
    /**
     * @brief Stores the Float_4 to an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 16-byte aligned. 4 floats (16 bytes) will be written.
     */
    void StoreAligned(float* ptr) const;
    /**
     * @brief Stores the Float_4 to an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 4 floats (16 bytes) will be written.
     */
    void StoreUnaligned(float* ptr) const;
    /**
     * @brief Vertically adds two Float_4 vectors. x will be added to other.x, y to other.y, etc.
     * @param other The other Float_4 vector to add.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator+(const Float_4& other) const;
    /**
     * @brief Vertically subtracts two Float_4 vectors. x will be subtracted by other.x, y by other.y, etc.
     * @param other The other Float_4 vector to subtract.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator-(const Float_4& other) const;
    /**
     * @brief Vertically multiplies two Float_4 vectors. x will be multiplied by other.x, y by other.y, etc.
     * @param other The other Float_4 vector to multiply.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator*(const Float_4& other) const;
    /**
     * @brief Vertically divides two Float_4 vectors. x will be divided by other.x, y by other.y, etc.
     * @param other The other Float_4 vector to divide.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator/(const Float_4& other) const;
    /**
     * @brief Bitwise OR operation between two Float_4 vectors. x will be OR'ed with other.x, y with other.y, etc.
     * @param other The other Float_4 vector.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator|(const Float_4& other) const;
    /**
     * @brief Bitwise AND operation between two Float_4 vectors. x will be AND'ed with other.x, y with other.y, etc.
     * @param other The other Float_4 vector.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator&(const Float_4& other) const;
    /**
     * @brief Bitwise XOR operation between two Float_4 vectors. x will be XOR'ed with other.x, y with other.y, etc.
     * @param other The other Float_4 vector.
     * @return The resulting Float_4 vector.
     */
    Float_4 operator^(const Float_4& other) const;

    Float_4 LessThan(const Float_4& other) const;
    Float_4 GreaterThan(const Float_4& other) const;
    Float_4 Equal(const Float_4& other) const;

    Float_4 HorizontalAdd(const Float_4& other) const;
    Float_4 Min(const Float_4& other) const;
    Float_4 Max(const Float_4& other) const;
    /**
     * @brief Computes the dot product of two Float_4 vectors.
     * @param other The other Float_4 vector.
     * @return The resulting Float_4 vector where all components are equal to the dot product result.
     */
    Float_4 Dot(const Float_4& other) const;
    /**
     * @brief Computes the dot product of two Float_4 vectors.
     * @param other The other Float_4 vector.
     * @return The resulting Float_4 vector where only lane 0 is equal to the dot product result. Others are zero.
     */
    Float_4 DotLane0(const Float_4& other) const;
    Float_4 SquareRoot() const;

    Float_4 Normalize() const;
    Float_4 Length() const;
    Float_4 LengthSquared() const;

    Float_4 UnpackInterleaveLowHalf(const Float_4& other) const;
    /**
     * @brief Move the lower 2 single-precision (32-bit) floating-point elements from b to the upper 2 elements of dst,
     * and copy the lower 2 elements from a to the lower 2 elements of dst.
     */
    Float_4 MoveOtherLowerToHigher(const Float_4& other) const;

    static void MatrixMultiply4x4(const Float_4 matrix[4], const Float_4 otherTransposedMatrix[4], Float_4 result[4]);

    DECLARE_4COMPONENT_SHUFFLES(Float_4);
    DECLARE_4COMPONENT_SELF_SHUFFLES_USENONSELF(Float_4);
};
struct alignas(16) Double_2 {
    union {
        struct {
            double X, Y;
        };
        double XY[2];
    };

    template <int32_t Index> double Get() const {
        static_assert(Index >= 0 && Index < 2, "Double_2::Get: Index out of bounds.");
        if constexpr (Index == 0)
            return X;
        else /* Index == 1 */
            return Y;
    }

    static constexpr int32_t ElementCount = 2;
    static constexpr int32_t PerElementSize = sizeof(double);

    /**
     * @brief Loads a Double_2 from an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 16-byte aligned. 2 doubles (16 bytes) will be read.
     * @return The loaded Double_2.
     */
    static Double_2 LoadAligned(const double* ptr);
    /**
     * @brief Loads a Double_2 from an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 2 doubles (16 bytes) will be read.
     * @return The loaded Double_2.
     */
    static Double_2 LoadUnaligned(const double* ptr);
    /**
     * @brief Stores the Double_2 to an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 16-byte aligned. 2 doubles (16 bytes) will be written.
     */
    void StoreAligned(double* ptr) const;
    /**
     * @brief Stores the Double_2 to an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 2 doubles (16 bytes) will be written.
     */
    void StoreUnaligned(double* ptr) const;
    /**
     * @brief Vertically adds two Double_2 vectors. x will be added to other.x, y to other.y, etc.
     * @param other The other Double_2 vector to add.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator+(const Double_2& other) const;
    /**
     * @brief Vertically subtracts two Double_2 vectors. x will be subtracted by other.x, y by other.y, etc.
     * @param other The other Double_2 vector to subtract.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator-(const Double_2& other) const;
    /**
     * @brief Vertically multiplies two Double_2 vectors. x will be multiplied by other.x, y by other.y, etc.
     * @param other The other Double_2 vector to multiply.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator*(const Double_2& other) const;
    /**
     * @brief Vertically divides two Double_2 vectors. x will be divided by other.x, y by other.y, etc.
     * @param other The other Double_2 vector to divide.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator/(const Double_2& other) const;
    /**
     * @brief Bitwise OR operation between two Double_2 vectors. x will be OR'ed with other.x, y with other.y, etc.
     * @param other The other Double_2 vector.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator|(const Double_2& other) const;
    /**
     * @brief Bitwise AND operation between two Double_2 vectors. x will be AND'ed with other.x, y with other.y, etc.
     * @param other The other Double_2 vector.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator&(const Double_2& other) const;
    /**
     * @brief Bitwise XOR operation between two Double_2 vectors. x will be XOR'ed with other.x, y with other.y, etc.
     * @param other The other Double_2 vector.
     * @return The resulting Double_2 vector.
     */
    Double_2 operator^(const Double_2& other) const;

    Double_2 LessThan(const Double_2& other) const;
    Double_2 GreaterThan(const Double_2& other) const;
    Double_2 Equal(const Double_2& other) const;

    Double_2 HorizontalAdd(const Double_2& other) const;
    Double_2 Min(const Double_2& other) const;
    Double_2 Max(const Double_2& other) const;
    /**
     * @brief Computes the dot product of two Double_2 vectors.
     * @param other The other Double_2 vector.
     * @return The resulting Double_2 vector where all components are equal to the dot product result.
     */
    Double_2 Dot(const Double_2& other) const;
    /**
     * @brief Computes the dot product of two Double_2 vectors.
     * @param other The other Double_2 vector.
     * @return The resulting Double_2 vector where only lane 0 is equal to the dot product result. Others are zero.
     */
    Double_2 DotLane0(const Double_2& other) const;
    Double_2 SquareRoot() const;

    Double_2 Normalize() const;
    Double_2 Length() const;
    Double_2 LengthSquared() const;

    Double_2 UnpackInterleaveLowHalf(const Double_2& other) const;
    /**
     * @brief Move the lower 2 single-precision (32-bit) floating-point elements from b to the upper 2 elements of dst,
     * and copy the lower 2 elements from a to the lower 2 elements of dst.
     */
    Double_2 MoveOtherLowerToHigher(const Double_2& other) const;

    // Double_2 specific shuffles (only 2 components: X, Y)
    // Shuffle result format: first component from this, second component from other
    Double_2 ShuffleXY(const Double_2& other) const; // {this.X, other.Y}
    Double_2 ShuffleXX(const Double_2& other) const; // {this.X, other.X}
    Double_2 ShuffleYY(const Double_2& other) const; // {this.Y, other.Y}
    Double_2 ShuffleYX(const Double_2& other) const; // {this.Y, other.X}
};
struct alignas(32) Double_4 {
    Double_4() : X(0), Y(0), Z(0), W(0) {}
    Double_4(double x, double y, double z, double w) : X(x), Y(y), Z(z), W(w) {}
    union {
        struct {
            double X, Y, Z, W;
        };
        double XYZW[4];
#if !EDVAR_CPP_CORE_MATH_ALLOW_SIMD_AVX
        struct {
            Double_2 XY_Double2;
            Double_2 ZW_Double2;
        };
#endif
    };

    template <int32_t Index> double Get() const {
        static_assert(Index >= 0 && Index < 4, "Double_4::Get: Index out of bounds.");
        if constexpr (Index == 0)
            return X;
        else if constexpr (Index == 1)
            return Y;
        else if constexpr (Index == 2)
            return Z;
        else /* Index == 3 */
            return W;
    }

    static constexpr int32_t ElementCount = 4;
    static constexpr int32_t PerElementSize = sizeof(double);

    /**
     * @brief Loads a Double_4 from an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 32-byte aligned. 4 doubles (32 bytes) will be read.
     * @return The loaded Double_4.
     */
    static Double_4 LoadAligned(const double* ptr);
    /**
     * @brief Loads a Double_4 from an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 4 doubles (32 bytes) will be read.
     * @return The loaded Double_4.
     */
    static Double_4 LoadUnaligned(const double* ptr);
    /**
     * @brief Stores the Double_4 to an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 32-byte aligned. 4 doubles (32 bytes) will be written.
     */
    void StoreAligned(double* ptr) const;
    /**
     * @brief Stores the Double_4 to an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 4 doubles (32 bytes) will be written.
     */
    void StoreUnaligned(double* ptr) const;
    /**
     * @brief Vertically adds two Double_4 vectors. x will be added to other.x, y to other.y, etc.
     * @param other The other Double_4 vector to add.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator+(const Double_4& other) const;
    /**
     * @brief Vertically subtracts two Double_4 vectors. x will be subtracted by other.x, y by other.y, etc.
     * @param other The other Double_4 vector to subtract.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator-(const Double_4& other) const;
    /**
     * @brief Vertically multiplies two Double_4 vectors. x will be multiplied by other.x, y by other.y, etc.
     * @param other The other Double_4 vector to multiply.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator*(const Double_4& other) const;
    /**
     * @brief Vertically divides two Double_4 vectors. x will be divided by other.x, y by other.y, etc.
     * @param other The other Double_4 vector to divide.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator/(const Double_4& other) const;
    /**
     * @brief Bitwise OR operation between two Double_4 vectors. x will be OR'ed with other.x, y with other.y, etc.
     * @param other The other Double_4 vector.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator|(const Double_4& other) const;
    /**
     * @brief Bitwise AND operation between two Double_4 vectors. x will be AND'ed with other.x, y with other.y, etc.
     * @param other The other Double_4 vector.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator&(const Double_4& other) const;
    /**
     * @brief Bitwise XOR operation between two Double_4 vectors. x will be XOR'ed with other.x, y with other.y, etc.
     * @param other The other Double_4 vector.
     * @return The resulting Double_4 vector.
     */
    Double_4 operator^(const Double_4& other) const;

    Double_4 LessThan(const Double_4& other) const;
    Double_4 GreaterThan(const Double_4& other) const;
    Double_4 Equal(const Double_4& other) const;

    Double_4 HorizontalAdd(const Double_4& other) const;
    Double_4 Min(const Double_4& other) const;
    Double_4 Max(const Double_4& other) const;
    /**
     * @brief Computes the dot product of two Double_4 vectors.
     * @param other The other Double_4 vector.
     * @return The resulting Double_4 vector where all components are equal to the dot product result.
     */
    Double_4 Dot(const Double_4& other) const;
    /**
     * @brief Computes the dot product of two Double_4 vectors.
     * @param other The other Double_4 vector.
     * @return The resulting Double_4 vector where only lane 0 is equal to the dot product result. Others are zero.
     */
    Double_4 DotLane0(const Double_4& other) const;
    Double_4 SquareRoot() const;

    Double_4 Normalize() const;
    Double_4 Length() const;
    Double_4 LengthSquared() const;

    Double_4 UnpackInterleaveLowHalf(const Double_4& other) const;
    /**
     * @brief Move the lower 2 elements from other to the upper 2 elements of dst,
     * and copy the lower 2 elements from this to the lower 2 elements of dst.
     */
    Double_4 MoveOtherLowerToHigher(const Double_4& other) const;

    static void MatrixMultiply4x4(const Double_4 matrix[4], const Double_4 otherTransposedMatrix[4],
                                  Double_4 result[4]);

    DECLARE_4COMPONENT_SHUFFLES(Double_4);
    DECLARE_4COMPONENT_SELF_SHUFFLES_USENONSELF(Double_4);
};
struct alignas(16) Int32_4 {
    Int32_4() : X(0), Y(0), Z(0), W(0) {}
    Int32_4(int32_t x, int32_t y, int32_t z, int32_t w) : X(x), Y(y), Z(z), W(w) {}
    union {
        struct {
            int32_t X, Y, Z, W;
        };
        int32_t XYZW[4];
    };

    template <int32_t Index> int32_t Get() const {
        static_assert(Index >= 0 && Index < 4, "Int32_4::Get: Index out of bounds.");
        if constexpr (Index == 0)
            return X;
        else if constexpr (Index == 1)
            return Y;
        else if constexpr (Index == 2)
            return Z;
        else /* Index == 3 */
            return W;
    }

    static constexpr int32_t ElementCount = 4;
    static constexpr int32_t PerElementSize = sizeof(int32_t);

    /**
     * @brief Loads an Int32_4 from an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 16-byte aligned. 4 int32_t (16 bytes) will be read.
     * @return The loaded Int32_4.
     */
    static Int32_4 LoadAligned(const int32_t* ptr);
    /**
     * @brief Loads an Int32_4 from an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 4 int32_t (16 bytes) will be read.
     * @return The loaded Int32_4.
     */
    static Int32_4 LoadUnaligned(const int32_t* ptr);
    /**
     * @brief Stores the Int32_4 to an aligned memory address.
     * @param ptr Pointer to the aligned memory address. Must be 16-byte aligned. 4 int32_t (16 bytes) will be written.
     */
    void StoreAligned(int32_t* ptr) const;
    /**
     * @brief Stores the Int32_4 to an unaligned memory address.
     * @param ptr Pointer to the unaligned memory address. 4 int32_t (16 bytes) will be written.
     */
    void StoreUnaligned(int32_t* ptr) const;
    /**
     * @brief Vertically adds two Int32_4 vectors. x will be added to other.x, y to other.y, etc.
     * @param other The other Int32_4 vector to add.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator+(const Int32_4& other) const;
    /**
     * @brief Vertically subtracts two Int32_4 vectors. x will be subtracted by other.x, y by other.y, etc.
     * @param other The other Int32_4 vector to subtract.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator-(const Int32_4& other) const;
    /**
     * @brief Vertically multiplies two Int32_4 vectors. x will be multiplied by other.x, y by other.y, etc.
     * @param other The other Int32_4 vector to multiply.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator*(const Int32_4& other) const;
    /**
     * @brief Bitwise OR operation between two Int32_4 vectors. x will be OR'ed with other.x, y with other.y, etc.
     * @param other The other Int32_4 vector.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator|(const Int32_4& other) const;
    /**
     * @brief Bitwise AND operation between two Int32_4 vectors. x will be AND'ed with other.x, y with other.y, etc.
     * @param other The other Int32_4 vector.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator&(const Int32_4& other) const;
    /**
     * @brief Bitwise XOR operation between two Int32_4 vectors. x will be XOR'ed with other.x, y with other.y, etc.
     * @param other The other Int32_4 vector.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator^(const Int32_4& other) const;
    /**
     * @brief Left shift operation on each component.
     * @param shift Number of bits to shift left.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator<<(int32_t shift) const;
    /**
     * @brief Right shift operation on each component.
     * @param shift Number of bits to shift right.
     * @return The resulting Int32_4 vector.
     */
    Int32_4 operator>>(int32_t shift) const;

    Int32_4 LessThan(const Int32_4& other) const;
    Int32_4 GreaterThan(const Int32_4& other) const;
    Int32_4 Equal(const Int32_4& other) const;

    Int32_4 HorizontalAdd(const Int32_4& other) const;
    Int32_4 Min(const Int32_4& other) const;
    Int32_4 Max(const Int32_4& other) const;

    Int32_4 UnpackInterleaveLowHalf(const Int32_4& other) const;
    Int32_4 UnpackInterleaveHighHalf(const Int32_4& other) const;
    /**
     * @brief Move the lower 2 elements from other to the upper 2 elements of dst,
     * and copy the lower 2 elements from this to the lower 2 elements of dst.
     */
    Int32_4 MoveOtherLowerToHigher(const Int32_4& other) const;

    DECLARE_4COMPONENT_SHUFFLES(Int32_4);
    DECLARE_4COMPONENT_SELF_SHUFFLES(Int32_4);
};

template <typename T>
using SIMDType =
    typename std::conditional_t<std::is_same_v<T, float>, Float_4,
                                std::conditional_t<std::is_same_v<T, double>, Double_4,
                                                   std::conditional_t<std::is_same_v<T, int32_t>, Int32_4, void>>>;
template <typename T> static constexpr bool IsSIMDTypeSupported = !std::is_same_v<SIMDType<T>, void>;

template <typename T> using VectorDataType = SIMDType<T>;
} // namespace SIMD
} // namespace Edvar::Math
