#pragma once
#include "Math/Math.hpp"
#include "Vector.hpp"

namespace Edvar::Math {

// ============================================================================
// Constructors
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>::Vector3() : X(static_cast<T>(0)), Y(static_cast<T>(0)), Z(static_cast<T>(0)) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>::Vector3(T all) : X(all), Y(all), Z(all) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>::Vector3(T x, T y, T z) : X(x), Y(y), Z(z) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>::Vector3(const Vector2<T>& vec2, T z) : X(vec2.X), Y(vec2.Y), Z(z) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>::Vector3(const Vector4<T>& vec4) : X(vec4.X), Y(vec4.Y), Z(vec4.Z) {}

// ============================================================================
// Static constants
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Zero = Vector3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::One = Vector3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::UnitX = Vector3<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::UnitY = Vector3<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::UnitZ = Vector3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Forward = Vector3<T>::UnitX;

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Right = Vector3<T>::UnitY;

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Up = Vector3<T>::UnitZ;

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Back = Vector3<T>(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Left = Vector3<T>(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const Vector3<T> Vector3<T>::Down = Vector3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1));

// ============================================================================
// Arithmetic operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator+(const Vector3<T>& other) const {
    return Vector3<T>(X + other.X, Y + other.Y, Z + other.Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator-(const Vector3<T>& other) const {
    return Vector3<T>(X - other.X, Y - other.Y, Z - other.Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator*(const Vector3<T>& other) const {
    return Vector3<T>(X * other.X, Y * other.Y, Z * other.Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator/(const Vector3<T>& other) const {
    return Vector3<T>(X / other.X, Y / other.Y, Z / other.Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator*(T scalar) const {
    return Vector3<T>(X * scalar, Y * scalar, Z * scalar);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator/(T scalar) const {
    return Vector3<T>(X / scalar, Y / scalar, Z / scalar);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::operator-() const {
    return Vector3<T>(-X, -Y, -Z);
}

// ============================================================================
// Compound assignment operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& other) {
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& other) {
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>& Vector3<T>::operator*=(const Vector3<T>& other) {
    X *= other.X;
    Y *= other.Y;
    Z *= other.Z;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>& Vector3<T>::operator/=(const Vector3<T>& other) {
    X /= other.X;
    Y /= other.Y;
    Z /= other.Z;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>& Vector3<T>::operator*=(T scalar) {
    X *= scalar;
    Y *= scalar;
    Z *= scalar;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T>& Vector3<T>::operator/=(T scalar) {
    X /= scalar;
    Y /= scalar;
    Z /= scalar;
    return *this;
}

// ============================================================================
// Comparison operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector3<T>::operator==(const Vector3<T>& other) const {
    return X == other.X && Y == other.Y && Z == other.Z;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector3<T>::operator!=(const Vector3<T>& other) const {
    return !(*this == other);
}

// ============================================================================
// Array access
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T& Vector3<T>::operator[](size_t index) {
    return XYZ[index];
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const T& Vector3<T>::operator[](size_t index) const {
    return XYZ[index];
}

// ============================================================================
// Vector operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::Dot(const Vector3<T>& other) const {
    return X * other.X + Y * other.Y + Z * other.Z;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& other) const {
    return Vector3<T>(Y * other.Z - Z * other.Y, Z * other.X - X * other.Z, X * other.Y - Y * other.X);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::Length() const {
    return Math::SquareRoot(static_cast<T>(LengthSquared()));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::LengthSquared() const {
    return X * X + Y * Y + Z * Z;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::Distance(const Vector3<T>& other) const {
    return (*this - other).Length();
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::DistanceSquared(const Vector3<T>& other) const {
    return (*this - other).LengthSquared();
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Normalized() const {
    T len = Length();
    if (len > static_cast<T>(0)) {
        return *this / len;
    }
    return Vector3<T>::Zero;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline void Vector3<T>::Normalize() {
    T len = Length();
    if (len > static_cast<T>(0)) {
        X /= len;
        Y /= len;
        Z /= len;
    }
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector3<T>::IsNormalized(T tolerance) const {
    return Math::Abs(LengthSquared() - static_cast<T>(1)) < tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector3<T>::IsZero(T tolerance) const {
    return Math::IsNearlyEqual(X, static_cast<T>(0), tolerance) &&
           Math::IsNearlyEqual(Y, static_cast<T>(0), tolerance) && Math::IsNearlyEqual(Z, static_cast<T>(0), tolerance);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector3<T>::IsNearlyEqual(const Vector3<T>& other, T tolerance) const {
    return Math::IsNearlyEqual(X, other.X, tolerance) && Math::IsNearlyEqual(Y, other.Y, tolerance) &&
           Math::IsNearlyEqual(Z, other.Z, tolerance);
}

// ============================================================================
// Interpolation
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Lerp(const Vector3<T>& target, T alpha) const {
    return *this + (target - *this) * alpha;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Slerp(const Vector3<T>& target, T alpha) const {
    T dot = Dot(target);
    dot = Math::Clamp(dot, static_cast<T>(-1), static_cast<T>(1));
    T theta = Math::ACos(dot) * alpha;
    Vector3<T> relativeVec = (target - *this * dot).Normalized();
    return (*this * Math::Cos(theta)) + (relativeVec * Math::Sin(theta));
}

// ============================================================================
// Projection and reflection
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::ProjectOnto(const Vector3<T>& other) const {
    T lenSq = other.LengthSquared();
    if (lenSq > static_cast<T>(0)) {
        return other * (Dot(other) / lenSq);
    }
    return Vector3<T>::Zero;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::ProjectOntoNormal(const Vector3<T>& normal) const {
    return normal * Dot(normal);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Reflect(const Vector3<T>& normal) const {
    return *this - normal * (static_cast<T>(2) * Dot(normal));
}

// ============================================================================
// Component-wise operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Abs() const {
    return Vector3<T>(Math::Abs(X), Math::Abs(Y), Math::Abs(Z));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Min(const Vector3<T>& other) const {
    return Vector3<T>(Math::Min(X, other.X), Math::Min(Y, other.Y), Math::Min(Z, other.Z));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Max(const Vector3<T>& other) const {
    return Vector3<T>(Math::Max(X, other.X), Math::Max(Y, other.Y), Math::Max(Z, other.Z));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Clamp(const Vector3<T>& min, const Vector3<T>& max) const {
    return Vector3<T>(Math::Clamp(X, min.X, max.X), Math::Clamp(Y, min.Y, max.Y), Math::Clamp(Z, min.Z, max.Z));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::GetMin() const {
    return Math::Min(X, Y, Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::GetMax() const {
    return Math::Max(X, Y, Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::GetAbsMin() const {
    return Math::Min(Math::Abs(X), Math::Abs(Y), Math::Abs(Z));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::GetAbsMax() const {
    return Math::Max(Math::Abs(X), Math::Abs(Y), Math::Abs(Z));
}

// ============================================================================
// Angle operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::AngleBetween(const Vector3<T>& other) const {
    T lenProduct = Length() * other.Length();
    if (lenProduct > static_cast<T>(0)) {
        T cosAngle = Math::Clamp(Dot(other) / lenProduct, static_cast<T>(-1), static_cast<T>(1));
        return Math::ACos(cosAngle);
    }
    return static_cast<T>(0);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::AngleBetweenNormalized(const Vector3<T>& other) const {
    T cosAngle = Math::Clamp(Dot(other), static_cast<T>(-1), static_cast<T>(1));
    return Math::ACos(cosAngle);
}

// ============================================================================
// Rotation operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::RotateAngleAxis(T angle, const Vector3<T>& axis) const {
    T sinAngle = Math::Sin(angle);
    T cosAngle = Math::Cos(angle);
    Vector3<T> normalAxis = axis.Normalized();

    return (*this * cosAngle) + (normalAxis.Cross(*this) * sinAngle) +
           (normalAxis * normalAxis.Dot(*this) * (static_cast<T>(1) - cosAngle));
}

// ============================================================================
// Conversion operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector3<T>::ToVector2() const {
    return Vector2<T>(X, Y);
}

// ============================================================================
// Utility functions
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline void Vector3<T>::Set(T x, T y, T z) {
    X = x;
    Y = y;
    Z = z;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::GetSignVector() const {
    return Vector3<T>(X >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1),
                      Y >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1),
                      Z >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Reciprocal() const {
    return Vector3<T>(X != static_cast<T>(0) ? static_cast<T>(1) / X : static_cast<T>(0),
                      Y != static_cast<T>(0) ? static_cast<T>(1) / Y : static_cast<T>(0),
                      Z != static_cast<T>(0) ? static_cast<T>(1) / Z : static_cast<T>(0));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector3<T>::AllComponentsEqual(T tolerance) const {
    return Math::Abs(X - Y) <= tolerance && Math::Abs(X - Z) <= tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::SnapToGrid(T gridSize) const {
    if (gridSize <= static_cast<T>(0)) {
        return *this;
    }
    return Vector3<T>(Math::Round(X / gridSize) * gridSize, Math::Round(Y / gridSize) * gridSize,
                      Math::Round(Z / gridSize) * gridSize);
}

// ============================================================================
// Static utility functions
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Min(const Vector3<T>& a, const Vector3<T>& b) {
    return a.Min(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Max(const Vector3<T>& a, const Vector3<T>& b) {
    return a.Max(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Clamp(const Vector3<T>& value, const Vector3<T>& min, const Vector3<T>& max) {
    return value.Clamp(min, max);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::Dot(const Vector3<T>& a, const Vector3<T>& b) {
    return a.Dot(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& a, const Vector3<T>& b) {
    return a.Cross(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::Distance(const Vector3<T>& a, const Vector3<T>& b) {
    return a.Distance(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector3<T>::DistanceSquared(const Vector3<T>& a, const Vector3<T>& b) {
    return a.DistanceSquared(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Lerp(const Vector3<T>& start, const Vector3<T>& end, T alpha) {
    return start.Lerp(end, alpha);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector3<T>::Slerp(const Vector3<T>& start, const Vector3<T>& end, T alpha) {
    return start.Slerp(end, alpha);
}

// ============================================================================
// Non-member operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> operator*(T scalar, const Vector3<T>& vec) {
    return vec * scalar;
}

// ============================================================================
// Vector4 Implementation
// ============================================================================

// ============================================================================
// Constructors
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>::Vector4() : X(static_cast<T>(0)), Y(static_cast<T>(0)), Z(static_cast<T>(0)), W(static_cast<T>(0)) {}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>::Vector4(T all) : X(all), Y(all), Z(all), W(all) {}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>::Vector4(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w) {}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>::Vector4(const Vector2<T>& vec2, T z, T w) : X(vec2.X), Y(vec2.Y), Z(z), W(w) {}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>::Vector4(const Vector3<T>& vec3, T w) : X(vec3.X), Y(vec3.Y), Z(vec3.Z), W(w) {}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>::Vector4(const SIMDType& simd) : Data(simd) {}

// ============================================================================
// Static constants
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Vector4<T> Vector4<T>::Zero =
    Vector4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Vector4<T> Vector4<T>::One =
    Vector4<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Vector4<T> Vector4<T>::UnitX =
    Vector4<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Vector4<T> Vector4<T>::UnitY =
    Vector4<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Vector4<T> Vector4<T>::UnitZ =
    Vector4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Vector4<T> Vector4<T>::UnitW =
    Vector4<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

// ============================================================================
// Arithmetic operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator+(const Vector4<T>& other) const {
    return Vector4<T>(Data + other.Data);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator-(const Vector4<T>& other) const {
    return Vector4<T>(Data - other.Data);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator*(const Vector4<T>& other) const {
    return Vector4<T>(Data * other.Data);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator/(const Vector4<T>& other) const {
    return Vector4<T>(Data / other.Data);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator*(T scalar) const {
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    return Vector4<T>(Data * scalarVec);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator/(T scalar) const {
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    return Vector4<T>(Data / scalarVec);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::operator-() const {
    SIMDType zero(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
    return Vector4<T>(zero - Data);
}

// ============================================================================
// Compound assignment operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& other) {
    Data = Data + other.Data;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& other) {
    Data = Data - other.Data;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>& Vector4<T>::operator*=(const Vector4<T>& other) {
    Data = Data * other.Data;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>& Vector4<T>::operator/=(const Vector4<T>& other) {
    Data = Data / other.Data;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>& Vector4<T>::operator*=(T scalar) {
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    Data = Data * scalarVec;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T>& Vector4<T>::operator/=(T scalar) {
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    Data = Data / scalarVec;
    return *this;
}

// ============================================================================
// Comparison operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline bool Vector4<T>::operator==(const Vector4<T>& other) const {
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline bool Vector4<T>::operator!=(const Vector4<T>& other) const {
    return !(*this == other);
}

// ============================================================================
// Array access
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T& Vector4<T>::operator[](size_t index) {
    return XYZW[index];
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline const T& Vector4<T>::operator[](size_t index) const {
    return XYZW[index];
}

// ============================================================================
// Vector operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::Dot(const Vector4<T>& other) const {
    SIMDType result = Data.Dot(other.Data);
    return result.template Get<0>();
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::Length() const {
    return SquareRoot(LengthSquared());
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::LengthSquared() const {
    return Dot(*this);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::Distance(const Vector4<T>& other) const {
    return (*this - other).Length();
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::DistanceSquared(const Vector4<T>& other) const {
    return (*this - other).LengthSquared();
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Normalized() const {
    T len = Length();
    if (len > static_cast<T>(0)) {
        return *this / len;
    }
    return Vector4<T>::Zero;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline void Vector4<T>::Normalize() {
    T len = Length();
    if (len > static_cast<T>(0)) {
        *this /= len;
    }
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline bool Vector4<T>::IsNormalized(T tolerance) const {
    return Abs(LengthSquared() - static_cast<T>(1)) < tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline bool Vector4<T>::IsZero(T tolerance) const {
    return Abs(X) <= tolerance && Abs(Y) <= tolerance && Abs(Z) <= tolerance && Abs(W) <= tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline bool Vector4<T>::IsNearlyEqual(const Vector4<T>& other, T tolerance) const {
    return Abs(X - other.X) <= tolerance && Abs(Y - other.Y) <= tolerance && Abs(Z - other.Z) <= tolerance &&
           Abs(W - other.W) <= tolerance;
}

// ============================================================================
// Interpolation
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Lerp(const Vector4<T>& target, T alpha) const {
    return *this + (target - *this) * alpha;
}

// ============================================================================
// Projection
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::ProjectOnto(const Vector4<T>& other) const {
    T lenSq = other.LengthSquared();
    if (lenSq > static_cast<T>(0)) {
        return other * (Dot(other) / lenSq);
    }
    return Vector4<T>::Zero;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::ProjectOntoNormal(const Vector4<T>& normal) const {
    return normal * Dot(normal);
}

// ============================================================================
// Component-wise operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Abs() const {
    return Vector4<T>(Math::Abs(X), Math::Abs(Y), Math::Abs(Z), Math::Abs(W));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Min(const Vector4<T>& other) const {
    return Vector4<T>(Data.Min(other.Data));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Max(const Vector4<T>& other) const {
    return Vector4<T>(Data.Max(other.Data));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Clamp(const Vector4<T>& min, const Vector4<T>& max) const {
    return Vector4<T>(Math::Clamp(X, min.X, max.X), Math::Clamp(Y, min.Y, max.Y), Math::Clamp(Z, min.Z, max.Z),
                      Math::Clamp(W, min.W, max.W));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::GetMin() const {
    return Math::Min(Math::Min(X, Y), Math::Min(Z, W));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::GetMax() const {
    return Math::Max(Math::Max(X, Y), Math::Max(Z, W));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::GetAbsMin() const {
    return Math::Min(Math::Min(Math::Abs(X), Math::Abs(Y)), Math::Min(Math::Abs(Z), Math::Abs(W)));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::GetAbsMax() const {
    return Math::Max(Math::Max(Math::Abs(X), Math::Abs(Y)), Math::Max(Math::Abs(Z), Math::Abs(W)));
}

// ============================================================================
// Conversion operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector2<T> Vector4<T>::ToVector2() const {
    return Vector2<T>(X, Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector3<T> Vector4<T>::ToVector3() const {
    return Vector3<T>(X, Y, Z);
}

// ============================================================================
// Utility functions
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline void Vector4<T>::Set(T x, T y, T z, T w) {
    X = x;
    Y = y;
    Z = z;
    W = w;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::GetSignVector() const {
    return Vector4<T>(X >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1),
                      Y >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1),
                      Z >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1),
                      W >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Reciprocal() const {
    return Vector4<T>(X != static_cast<T>(0) ? static_cast<T>(1) / X : static_cast<T>(0),
                      Y != static_cast<T>(0) ? static_cast<T>(1) / Y : static_cast<T>(0),
                      Z != static_cast<T>(0) ? static_cast<T>(1) / Z : static_cast<T>(0),
                      W != static_cast<T>(0) ? static_cast<T>(1) / W : static_cast<T>(0));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline bool Vector4<T>::AllComponentsEqual(T tolerance) const {
    return Abs(X - Y) <= tolerance && Abs(X - Z) <= tolerance && Abs(X - W) <= tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::SnapToGrid(T gridSize) const {
    if (gridSize <= static_cast<T>(0)) {
        return *this;
    }
    return Vector4<T>(Math::Round(X / gridSize) * gridSize, Math::Round(Y / gridSize) * gridSize,
                      Math::Round(Z / gridSize) * gridSize, Math::Round(W / gridSize) * gridSize);
}

// ============================================================================
// Static utility functions
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Min(const Vector4<T>& a, const Vector4<T>& b) {
    return a.Min(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Max(const Vector4<T>& a, const Vector4<T>& b) {
    return a.Max(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Clamp(const Vector4<T>& value, const Vector4<T>& min, const Vector4<T>& max) {
    return value.Clamp(min, max);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::Dot(const Vector4<T>& a, const Vector4<T>& b) {
    return a.Dot(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::Distance(const Vector4<T>& a, const Vector4<T>& b) {
    return a.Distance(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline T Vector4<T>::DistanceSquared(const Vector4<T>& a, const Vector4<T>& b) {
    return a.DistanceSquared(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> Vector4<T>::Lerp(const Vector4<T>& start, const Vector4<T>& end, T alpha) {
    return start.Lerp(end, alpha);
}

// ============================================================================
// Non-member operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
inline Vector4<T> operator*(T scalar, const Vector4<T>& vec) {
    return vec * scalar;
}

// ============================================================================
// Vector2 Implementation
// ============================================================================

// ============================================================================
// Constructors
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>::Vector2() : X(static_cast<T>(0)), Y(static_cast<T>(0)) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>::Vector2(T all) : X(all), Y(all) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>::Vector2(T x, T y) : X(x), Y(y) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>::Vector2(const Vector3<T>& vec3) : X(vec3.X), Y(vec3.Y) {}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>::Vector2(const Vector4<T>& vec4) : X(vec4.X), Y(vec4.Y) {}

// ============================================================================
// Static constants
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::Zero = Vector2<T>(static_cast<T>(0), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::One = Vector2<T>(static_cast<T>(1), static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::UnitX = Vector2<T>(static_cast<T>(1), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::UnitY = Vector2<T>(static_cast<T>(0), static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::Right = Vector2<T>::UnitX;

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::Up = Vector2<T>::UnitY;

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::Left = Vector2<T>(static_cast<T>(-1), static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Vector2<T> Vector2<T>::Down = Vector2<T>(static_cast<T>(0), static_cast<T>(-1));

// ============================================================================
// Arithmetic operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const {
    return Vector2<T>(X + other.X, Y + other.Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const {
    return Vector2<T>(X - other.X, Y - other.Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator*(const Vector2<T>& other) const {
    return Vector2<T>(X * other.X, Y * other.Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator/(const Vector2<T>& other) const {
    return Vector2<T>(X / other.X, Y / other.Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator*(T scalar) const {
    return Vector2<T>(X * scalar, Y * scalar);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator/(T scalar) const {
    return Vector2<T>(X / scalar, Y / scalar);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::operator-() const {
    return Vector2<T>(-X, -Y);
}

// ============================================================================
// Compound assignment operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& other) {
    X += other.X;
    Y += other.Y;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other) {
    X -= other.X;
    Y -= other.Y;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& other) {
    X *= other.X;
    Y *= other.Y;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>& Vector2<T>::operator/=(const Vector2<T>& other) {
    X /= other.X;
    Y /= other.Y;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>& Vector2<T>::operator*=(T scalar) {
    X *= scalar;
    Y *= scalar;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T>& Vector2<T>::operator/=(T scalar) {
    X /= scalar;
    Y /= scalar;
    return *this;
}

// ============================================================================
// Comparison operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector2<T>::operator==(const Vector2<T>& other) const {
    return X == other.X && Y == other.Y;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector2<T>::operator!=(const Vector2<T>& other) const {
    return !(*this == other);
}

// ============================================================================
// Array access
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T& Vector2<T>::operator[](size_t index) {
    return XY[index];
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline const T& Vector2<T>::operator[](size_t index) const {
    return XY[index];
}

// ============================================================================
// Vector operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Dot(const Vector2<T>& other) const {
    return X * other.X + Y * other.Y;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Cross(const Vector2<T>& other) const {
    return X * other.Y - Y * other.X;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Length() const {
    return SquareRoot(LengthSquared());
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::LengthSquared() const {
    return X * X + Y * Y;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Distance(const Vector2<T>& other) const {
    return (*this - other).Length();
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::DistanceSquared(const Vector2<T>& other) const {
    return (*this - other).LengthSquared();
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Normalized() const {
    T len = Length();
    if (len > static_cast<T>(0)) {
        return *this / len;
    }
    return Vector2<T>::Zero;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline void Vector2<T>::Normalize() {
    T len = Length();
    if (len > static_cast<T>(0)) {
        X /= len;
        Y /= len;
    }
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector2<T>::IsNormalized(T tolerance) const {
    return Abs(LengthSquared() - static_cast<T>(1)) < tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector2<T>::IsZero(T tolerance) const {
    return Abs(X) <= tolerance && Abs(Y) <= tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector2<T>::IsNearlyEqual(const Vector2<T>& other, T tolerance) const {
    return Abs(X - other.X) <= tolerance && Abs(Y - other.Y) <= tolerance;
}

// ============================================================================
// Interpolation
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Lerp(const Vector2<T>& target, T alpha) const {
    return *this + (target - *this) * alpha;
}

// ============================================================================
// Projection and reflection
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::ProjectOnto(const Vector2<T>& other) const {
    T lenSq = other.LengthSquared();
    if (lenSq > static_cast<T>(0)) {
        return other * (Dot(other) / lenSq);
    }
    return Vector2<T>::Zero;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::ProjectOntoNormal(const Vector2<T>& normal) const {
    return normal * Dot(normal);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Reflect(const Vector2<T>& normal) const {
    return *this - normal * (static_cast<T>(2) * Dot(normal));
}

// ============================================================================
// Component-wise operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Abs() const {
    return Vector2<T>(Math::Abs(X), Math::Abs(Y));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Min(const Vector2<T>& other) const {
    return Vector2<T>(Math::Min(X, other.X), Math::Min(Y, other.Y));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Max(const Vector2<T>& other) const {
    return Vector2<T>(Math::Max(X, other.X), Math::Max(Y, other.Y));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Clamp(const Vector2<T>& min, const Vector2<T>& max) const {
    return Vector2<T>(Math::Clamp(X, min.X, max.X), Math::Clamp(Y, min.Y, max.Y));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::GetMin() const {
    return Math::Min(X, Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::GetMax() const {
    return Math::Max(X, Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::GetAbsMin() const {
    return Math::Min(Math::Abs(X), Math::Abs(Y));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::GetAbsMax() const {
    return Math::Max(Math::Abs(X), Math::Abs(Y));
}

// ============================================================================
// Angle operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::AngleBetween(const Vector2<T>& other) const {
    T lenProduct = Length() * other.Length();
    if (lenProduct > static_cast<T>(0)) {
        T cosAngle = Math::Clamp(Dot(other) / lenProduct, static_cast<T>(-1), static_cast<T>(1));
        return ACos(cosAngle);
    }
    return static_cast<T>(0);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::AngleBetweenNormalized(const Vector2<T>& other) const {
    T cosAngle = Math::Clamp(Dot(other), static_cast<T>(-1), static_cast<T>(1));
    return ACos(cosAngle);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::GetAngle() const {
    return ATan2(Y, X);
}

// ============================================================================
// Rotation operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Rotate(T angleRadians) const {
    T cosAngle = Cos(angleRadians);
    T sinAngle = Sin(angleRadians);
    return Vector2<T>(X * cosAngle - Y * sinAngle, X * sinAngle + Y * cosAngle);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::RotateAround(const Vector2<T>& pivot, T angleRadians) const {
    Vector2<T> offset = *this - pivot;
    return pivot + offset.Rotate(angleRadians);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Perpendicular() const {
    return Vector2<T>(-Y, X);
}

// ============================================================================
// Conversion operations
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector3<T> Vector2<T>::ToVector3(T z) const {
    return Vector3<T>(X, Y, z);
}

// ============================================================================
// Utility functions
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline void Vector2<T>::Set(T x, T y) {
    X = x;
    Y = y;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::GetSignVector() const {
    return Vector2<T>(X >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1),
                      Y >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Reciprocal() const {
    return Vector2<T>(X != static_cast<T>(0) ? static_cast<T>(1) / X : static_cast<T>(0),
                      Y != static_cast<T>(0) ? static_cast<T>(1) / Y : static_cast<T>(0));
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline bool Vector2<T>::AllComponentsEqual(T tolerance) const {
    return Abs(X - Y) <= tolerance;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::SnapToGrid(T gridSize) const {
    if (gridSize <= static_cast<T>(0)) {
        return *this;
    }
    return Vector2<T>(Math::Round(X / gridSize) * gridSize, Math::Round(Y / gridSize) * gridSize);
}

// ============================================================================
// Static utility functions
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Min(const Vector2<T>& a, const Vector2<T>& b) {
    return a.Min(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Max(const Vector2<T>& a, const Vector2<T>& b) {
    return a.Max(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Clamp(const Vector2<T>& value, const Vector2<T>& min, const Vector2<T>& max) {
    return value.Clamp(min, max);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Dot(const Vector2<T>& a, const Vector2<T>& b) {
    return a.Dot(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Cross(const Vector2<T>& a, const Vector2<T>& b) {
    return a.Cross(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::Distance(const Vector2<T>& a, const Vector2<T>& b) {
    return a.Distance(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline T Vector2<T>::DistanceSquared(const Vector2<T>& a, const Vector2<T>& b) {
    return a.DistanceSquared(b);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> Vector2<T>::Lerp(const Vector2<T>& start, const Vector2<T>& end, T alpha) {
    return start.Lerp(end, alpha);
}

// ============================================================================
// Non-member operators
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
inline Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
    return vec * scalar;
}

} // namespace Edvar::Math