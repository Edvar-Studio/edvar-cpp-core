#pragma once
#include "Math.hpp"

namespace Edvar::Math {
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Vector4;
// Forward declarations
template <typename T>
    requires(std::is_arithmetic_v<T>)
struct Vector3;
template <typename T>
    requires(std::is_arithmetic_v<T>)
struct Vector2;

struct Quaternion;
struct Rotator;
struct Transform;
// End forward declarations

template <typename T>
    requires(std::is_arithmetic_v<T>)
struct Vector3 {

    // Constructors
    Vector3();
    explicit Vector3(T all);
    explicit Vector3(T x, T y, T z);
    explicit Vector3(const Vector2<T>& vec2, T z = 0);
    explicit Vector3(const Vector4<T>& vec4);

    // Static constants
    static const Vector3<T> Zero;
    static const Vector3<T> One;
    static const Vector3<T> UnitX;
    static const Vector3<T> UnitY;
    static const Vector3<T> UnitZ;
    static const Vector3<T> Forward;
    static const Vector3<T> Right;
    static const Vector3<T> Up;
    static const Vector3<T> Back;
    static const Vector3<T> Left;
    static const Vector3<T> Down;

    // Arithmetic operators
    Vector3<T> operator+(const Vector3<T>& other) const;
    Vector3<T> operator-(const Vector3<T>& other) const;
    Vector3<T> operator*(const Vector3<T>& other) const;
    Vector3<T> operator/(const Vector3<T>& other) const;
    Vector3<T> operator*(T scalar) const;
    Vector3<T> operator/(T scalar) const;
    Vector3<T> operator-() const;

    // Compound assignment operators
    Vector3<T>& operator+=(const Vector3<T>& other);
    Vector3<T>& operator-=(const Vector3<T>& other);
    Vector3<T>& operator*=(const Vector3<T>& other);
    Vector3<T>& operator/=(const Vector3<T>& other);
    Vector3<T>& operator*=(T scalar);
    Vector3<T>& operator/=(T scalar);

    // Comparison operators
    bool operator==(const Vector3<T>& other) const;
    bool operator!=(const Vector3<T>& other) const;

    // Array access
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    // Vector operations
    T Dot(const Vector3<T>& other) const;
    Vector3<T> Cross(const Vector3<T>& other) const;
    T Length() const;
    T LengthSquared() const;
    T Distance(const Vector3<T>& other) const;
    T DistanceSquared(const Vector3<T>& other) const;
    Vector3<T> Normalized() const;
    void Normalize();
    bool IsNormalized(T tolerance = static_cast<T>(1e-4)) const;
    bool IsZero(T tolerance = static_cast<T>(1e-6)) const;
    bool IsNearlyEqual(const Vector3<T>& other, T tolerance = static_cast<T>(1e-4)) const;

    // Interpolation
    Vector3<T> Lerp(const Vector3<T>& target, T alpha) const;
    Vector3<T> Slerp(const Vector3<T>& target, T alpha) const;

    // Projection and reflection
    Vector3<T> ProjectOnto(const Vector3<T>& other) const;
    Vector3<T> ProjectOntoNormal(const Vector3<T>& normal) const;
    Vector3<T> Reflect(const Vector3<T>& normal) const;

    // Component-wise operations
    Vector3<T> Abs() const;
    Vector3<T> Min(const Vector3<T>& other) const;
    Vector3<T> Max(const Vector3<T>& other) const;
    Vector3<T> Clamp(const Vector3<T>& min, const Vector3<T>& max) const;
    T GetMin() const;
    T GetMax() const;
    T GetAbsMin() const;
    T GetAbsMax() const;

    // Angle operations
    T AngleBetween(const Vector3<T>& other) const;
    T AngleBetweenNormalized(const Vector3<T>& other) const;

    // Rotation operations
    Vector3<T> RotateAngleAxis(T angle, const Vector3<T>& axis) const;

    // Conversion operations
    Rotator ToRotator() const;
    Quaternion ToQuaternion() const;
    Vector2<T> ToVector2() const;

    // Utility functions
    void Set(T x, T y, T z);
    Vector3<T> GetSignVector() const;
    Vector3<T> Reciprocal() const;
    bool AllComponentsEqual(T tolerance = static_cast<T>(1e-4)) const;
    Vector3<T> SnapToGrid(T gridSize) const;

    // Static utility functions
    static Vector3<T> Min(const Vector3<T>& a, const Vector3<T>& b);
    static Vector3<T> Max(const Vector3<T>& a, const Vector3<T>& b);
    static Vector3<T> Clamp(const Vector3<T>& value, const Vector3<T>& min, const Vector3<T>& max);
    static T Dot(const Vector3<T>& a, const Vector3<T>& b);
    static Vector3<T> Cross(const Vector3<T>& a, const Vector3<T>& b);
    static T Distance(const Vector3<T>& a, const Vector3<T>& b);
    static T DistanceSquared(const Vector3<T>& a, const Vector3<T>& b);
    static Vector3<T> Lerp(const Vector3<T>& start, const Vector3<T>& end, T alpha);
    static Vector3<T> Slerp(const Vector3<T>& start, const Vector3<T>& end, T alpha);

    union {
        struct {
            T X, Y, Z;
        };
        T XYZ[3];
    };
};

// Scalar multiplication from left side
template <typename T>
    requires(std::is_arithmetic_v<T>)
Vector3<T> operator*(T scalar, const Vector3<T>& vec);

// ============================================================================
// Vector4
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Vector4 {
    using SIMDType = SIMD::VectorDataType<T>;

    // Constructors
    Vector4();
    explicit Vector4(T all);
    explicit Vector4(T x, T y, T z, T w);
    explicit Vector4(const Vector2<T>& vec2, T z = 0, T w = 0);
    explicit Vector4(const Vector3<T>& vec3, T w = 0);
    explicit Vector4(const SIMDType& simd);

    // Static constants
    static const Vector4<T> Zero;
    static const Vector4<T> One;
    static const Vector4<T> UnitX;
    static const Vector4<T> UnitY;
    static const Vector4<T> UnitZ;
    static const Vector4<T> UnitW;

    // Arithmetic operators
    Vector4<T> operator+(const Vector4<T>& other) const;
    Vector4<T> operator-(const Vector4<T>& other) const;
    Vector4<T> operator*(const Vector4<T>& other) const;
    Vector4<T> operator/(const Vector4<T>& other) const;
    Vector4<T> operator*(T scalar) const;
    Vector4<T> operator/(T scalar) const;
    Vector4<T> operator-() const;

    // Compound assignment operators
    Vector4<T>& operator+=(const Vector4<T>& other);
    Vector4<T>& operator-=(const Vector4<T>& other);
    Vector4<T>& operator*=(const Vector4<T>& other);
    Vector4<T>& operator/=(const Vector4<T>& other);
    Vector4<T>& operator*=(T scalar);
    Vector4<T>& operator/=(T scalar);

    // Comparison operators
    bool operator==(const Vector4<T>& other) const;
    bool operator!=(const Vector4<T>& other) const;

    // Array access
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    // Vector operations
    T Dot(const Vector4<T>& other) const;
    T Length() const;
    T LengthSquared() const;
    T Distance(const Vector4<T>& other) const;
    T DistanceSquared(const Vector4<T>& other) const;
    Vector4<T> Normalized() const;
    void Normalize();
    bool IsNormalized(T tolerance = static_cast<T>(SMALL_NUMBER)) const;
    bool IsZero(T tolerance = static_cast<T>(SMALL_NUMBER)) const;
    bool IsNearlyEqual(const Vector4<T>& other, T tolerance = static_cast<T>(SMALL_NUMBER)) const;

    // Interpolation
    Vector4<T> Lerp(const Vector4<T>& target, T alpha) const;

    // Projection
    Vector4<T> ProjectOnto(const Vector4<T>& other) const;
    Vector4<T> ProjectOntoNormal(const Vector4<T>& normal) const;

    // Component-wise operations
    Vector4<T> Abs() const;
    Vector4<T> Min(const Vector4<T>& other) const;
    Vector4<T> Max(const Vector4<T>& other) const;
    Vector4<T> Clamp(const Vector4<T>& min, const Vector4<T>& max) const;
    T GetMin() const;
    T GetMax() const;
    T GetAbsMin() const;
    T GetAbsMax() const;

    // Conversion operations
    Vector2<T> ToVector2() const;
    Vector3<T> ToVector3() const;

    // Utility functions
    void Set(T x, T y, T z, T w);
    Vector4<T> GetSignVector() const;
    Vector4<T> Reciprocal() const;
    bool AllComponentsEqual(T tolerance = static_cast<T>(SMALL_NUMBER)) const;
    Vector4<T> SnapToGrid(T gridSize) const;

    // Static utility functions
    static Vector4<T> Min(const Vector4<T>& a, const Vector4<T>& b);
    static Vector4<T> Max(const Vector4<T>& a, const Vector4<T>& b);
    static Vector4<T> Clamp(const Vector4<T>& value, const Vector4<T>& min, const Vector4<T>& max);
    static T Dot(const Vector4<T>& a, const Vector4<T>& b);
    static T Distance(const Vector4<T>& a, const Vector4<T>& b);
    static T DistanceSquared(const Vector4<T>& a, const Vector4<T>& b);
    static Vector4<T> Lerp(const Vector4<T>& start, const Vector4<T>& end, T alpha);

    union {
        struct {
            T X, Y, Z, W;
        };
        T XYZW[4];
        SIMDType Data;
    };
};

// Scalar multiplication from left side
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector4<T> operator*(T scalar, const Vector4<T>& vec);

// ============================================================================
// Vector2
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
struct Vector2 {

    // Constructors
    Vector2();
    explicit Vector2(T all);
    explicit Vector2(T x, T y);
    explicit Vector2(const Vector3<T>& vec3);
    explicit Vector2(const Vector4<T>& vec4);

    // Static constants
    static const Vector2<T> Zero;
    static const Vector2<T> One;
    static const Vector2<T> UnitX;
    static const Vector2<T> UnitY;
    static const Vector2<T> Right;
    static const Vector2<T> Up;
    static const Vector2<T> Left;
    static const Vector2<T> Down;

    // Arithmetic operators
    Vector2<T> operator+(const Vector2<T>& other) const;
    Vector2<T> operator-(const Vector2<T>& other) const;
    Vector2<T> operator*(const Vector2<T>& other) const;
    Vector2<T> operator/(const Vector2<T>& other) const;
    Vector2<T> operator*(T scalar) const;
    Vector2<T> operator/(T scalar) const;
    Vector2<T> operator-() const;

    // Compound assignment operators
    Vector2<T>& operator+=(const Vector2<T>& other);
    Vector2<T>& operator-=(const Vector2<T>& other);
    Vector2<T>& operator*=(const Vector2<T>& other);
    Vector2<T>& operator/=(const Vector2<T>& other);
    Vector2<T>& operator*=(T scalar);
    Vector2<T>& operator/=(T scalar);

    // Comparison operators
    bool operator==(const Vector2<T>& other) const;
    bool operator!=(const Vector2<T>& other) const;

    // Array access
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    // Vector operations
    T Dot(const Vector2<T>& other) const;
    T Cross(const Vector2<T>& other) const; // 2D cross product (returns scalar Z component)
    T Length() const;
    T LengthSquared() const;
    T Distance(const Vector2<T>& other) const;
    T DistanceSquared(const Vector2<T>& other) const;
    Vector2<T> Normalized() const;
    void Normalize();
    bool IsNormalized(T tolerance = static_cast<T>(SMALL_NUMBER)) const;
    bool IsZero(T tolerance = static_cast<T>(SMALL_NUMBER)) const;
    bool IsNearlyEqual(const Vector2<T>& other, T tolerance = static_cast<T>(SMALL_NUMBER)) const;

    // Interpolation
    Vector2<T> Lerp(const Vector2<T>& target, T alpha) const;

    // Projection and reflection
    Vector2<T> ProjectOnto(const Vector2<T>& other) const;
    Vector2<T> ProjectOntoNormal(const Vector2<T>& normal) const;
    Vector2<T> Reflect(const Vector2<T>& normal) const;

    // Component-wise operations
    Vector2<T> Abs() const;
    Vector2<T> Min(const Vector2<T>& other) const;
    Vector2<T> Max(const Vector2<T>& other) const;
    Vector2<T> Clamp(const Vector2<T>& min, const Vector2<T>& max) const;
    T GetMin() const;
    T GetMax() const;
    T GetAbsMin() const;
    T GetAbsMax() const;

    // Angle operations
    T AngleBetween(const Vector2<T>& other) const;
    T AngleBetweenNormalized(const Vector2<T>& other) const;
    T GetAngle() const; // Angle from positive X axis

    // Rotation operations
    Vector2<T> Rotate(T angleRadians) const;
    Vector2<T> RotateAround(const Vector2<T>& pivot, T angleRadians) const;
    Vector2<T> Perpendicular() const; // Returns perpendicular vector (rotated 90 degrees CCW)

    // Conversion operations
    Vector3<T> ToVector3(T z = 0) const;

    // Utility functions
    void Set(T x, T y);
    Vector2<T> GetSignVector() const;
    Vector2<T> Reciprocal() const;
    bool AllComponentsEqual(T tolerance = static_cast<T>(SMALL_NUMBER)) const;
    Vector2<T> SnapToGrid(T gridSize) const;

    // Static utility functions
    static Vector2<T> Min(const Vector2<T>& a, const Vector2<T>& b);
    static Vector2<T> Max(const Vector2<T>& a, const Vector2<T>& b);
    static Vector2<T> Clamp(const Vector2<T>& value, const Vector2<T>& min, const Vector2<T>& max);
    static T Dot(const Vector2<T>& a, const Vector2<T>& b);
    static T Cross(const Vector2<T>& a, const Vector2<T>& b);
    static T Distance(const Vector2<T>& a, const Vector2<T>& b);
    static T DistanceSquared(const Vector2<T>& a, const Vector2<T>& b);
    static Vector2<T> Lerp(const Vector2<T>& start, const Vector2<T>& end, T alpha);

    union {
        struct {
            T X, Y;
        };
        T XY[2];
    };
};

// Scalar multiplication from left side
template <typename T>
    requires(std::is_arithmetic_v<T>)
Vector2<T> operator*(T scalar, const Vector2<T>& vec);

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
} // namespace Edvar::Math
