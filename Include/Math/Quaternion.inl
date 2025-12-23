#pragma once
#include "Math/Math.hpp"
#include "Quaternion.hpp"

namespace Edvar::Math {

// ============================================================================
// Constructors
// ============================================================================

inline Quaternion::Quaternion() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {
    // Identity quaternion
}

inline Quaternion::Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

inline Quaternion::Quaternion(const SIMD::Float_4& simd) { SIMDData = simd; }

inline Quaternion Quaternion::FromAxisAngle(const Vector3f& axis, float angleRadians) {
    Vector3f normalizedAxis = axis.Normalized();
    float halfAngle = angleRadians * 0.5f;
    float sinHalf = static_cast<float>(Sin(static_cast<double>(halfAngle)));
    float cosHalf = static_cast<float>(Cos(static_cast<double>(halfAngle)));

    return Quaternion(normalizedAxis.X * sinHalf, normalizedAxis.Y * sinHalf, normalizedAxis.Z * sinHalf, cosHalf);
}

inline Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll) {
    // Euler angles to quaternion (ZYX order - roll, pitch, yaw)
    // pitch = rotation around X, yaw = rotation around Y, roll = rotation around Z
    float halfPitch = pitch * 0.5f;
    float halfYaw = yaw * 0.5f;
    float halfRoll = roll * 0.5f;

    float cp = static_cast<float>(Cos(static_cast<double>(halfPitch)));
    float sp = static_cast<float>(Sin(static_cast<double>(halfPitch)));
    float cy = static_cast<float>(Cos(static_cast<double>(halfYaw)));
    float sy = static_cast<float>(Sin(static_cast<double>(halfYaw)));
    float cr = static_cast<float>(Cos(static_cast<double>(halfRoll)));
    float sr = static_cast<float>(Sin(static_cast<double>(halfRoll)));

    return Quaternion(sr * cp * cy - cr * sp * sy, // X
                      cr * sp * cy + sr * cp * sy, // Y
                      cr * cp * sy - sr * sp * cy, // Z
                      cr * cp * cy + sr * sp * sy  // W
    );
}

inline Quaternion Quaternion::FromEuler(const Vector3f& eulerAngles) {
    return FromEuler(eulerAngles.X, eulerAngles.Y, eulerAngles.Z);
}

inline Quaternion Quaternion::FromRotationMatrix(const Matrix3x3<float>& rotationMatrix) {
    // Convert 3x3 rotation matrix to quaternion
    float trace = rotationMatrix(0, 0) + rotationMatrix(1, 1) + rotationMatrix(2, 2);

    Quaternion result;

    if (trace > 0.0f) {
        float s = SquareRoot(trace + 1.0f) * 2.0f; // s = 4 * w
        result.W = 0.25f * s;
        result.X = (rotationMatrix(2, 1) - rotationMatrix(1, 2)) / s;
        result.Y = (rotationMatrix(0, 2) - rotationMatrix(2, 0)) / s;
        result.Z = (rotationMatrix(1, 0) - rotationMatrix(0, 1)) / s;
    } else if (rotationMatrix(0, 0) > rotationMatrix(1, 1) && rotationMatrix(0, 0) > rotationMatrix(2, 2)) {
        float s = SquareRoot(1.0f + rotationMatrix(0, 0) - rotationMatrix(1, 1) - rotationMatrix(2, 2)) * 2.0f;
        result.W = (rotationMatrix(2, 1) - rotationMatrix(1, 2)) / s;
        result.X = 0.25f * s;
        result.Y = (rotationMatrix(0, 1) + rotationMatrix(1, 0)) / s;
        result.Z = (rotationMatrix(0, 2) + rotationMatrix(2, 0)) / s;
    } else if (rotationMatrix(1, 1) > rotationMatrix(2, 2)) {
        float s = SquareRoot(1.0f + rotationMatrix(1, 1) - rotationMatrix(0, 0) - rotationMatrix(2, 2)) * 2.0f;
        result.W = (rotationMatrix(0, 2) - rotationMatrix(2, 0)) / s;
        result.X = (rotationMatrix(0, 1) + rotationMatrix(1, 0)) / s;
        result.Y = 0.25f * s;
        result.Z = (rotationMatrix(1, 2) + rotationMatrix(2, 1)) / s;
    } else {
        float s = SquareRoot(1.0f + rotationMatrix(2, 2) - rotationMatrix(0, 0) - rotationMatrix(1, 1)) * 2.0f;
        result.W = (rotationMatrix(1, 0) - rotationMatrix(0, 1)) / s;
        result.X = (rotationMatrix(0, 2) + rotationMatrix(2, 0)) / s;
        result.Y = (rotationMatrix(1, 2) + rotationMatrix(2, 1)) / s;
        result.Z = 0.25f * s;
    }

    return result;
}

inline Quaternion Quaternion::FromRotationMatrix(const Matrix4x4<float>& transformMatrix) {
    // Extract upper-left 3x3 rotation part
    Matrix3x3<float> rotMat = Matrix3x3<float>::FromMatrix4x4(transformMatrix);
    return FromRotationMatrix(rotMat);
}

inline Quaternion Quaternion::FromToRotation(const Vector3f& from, const Vector3f& to) {
    Vector3f f = from.Normalized();
    Vector3f t = to.Normalized();

    float dot = f.Dot(t);

    // Check if vectors are nearly parallel
    if (dot >= 0.999999f) {
        return Quaternion::Identity;
    }

    // Check if vectors are nearly opposite
    if (dot <= -0.999999f) {
        // Find an axis perpendicular to from
        Vector3f axis = Vector3f::UnitX.Cross(f);
        if (axis.LengthSquared() < 0.000001f) {
            axis = Vector3f::UnitY.Cross(f);
        }
        axis = axis.Normalized();
        return FromAxisAngle(axis, static_cast<float>(PI));
    }

    // Standard case
    Vector3f axis = f.Cross(t);
    float s = SquareRoot((1.0f + dot) * 2.0f);
    float invS = 1.0f / s;

    return Quaternion(axis.X * invS, axis.Y * invS, axis.Z * invS, s * 0.5f);
}

inline Quaternion Quaternion::LookRotation(const Vector3f& forward, const Vector3f& up) {
    // Create a rotation that looks in the forward direction with the given up vector
    Vector3f f = forward.Normalized();
    Vector3f r = up.Cross(f).Normalized(); // Right vector
    Vector3f u = f.Cross(r);               // Recalculate up

    // Build rotation matrix from basis vectors
    // In LH Z-up: Forward = +X, Right = +Y, Up = +Z
    Matrix3x3<float> rotMat;
    rotMat(0, 0) = f.X;
    rotMat(0, 1) = r.X;
    rotMat(0, 2) = u.X;
    rotMat(1, 0) = f.Y;
    rotMat(1, 1) = r.Y;
    rotMat(1, 2) = u.Y;
    rotMat(2, 0) = f.Z;
    rotMat(2, 1) = r.Z;
    rotMat(2, 2) = u.Z;

    return FromRotationMatrix(rotMat);
}

// ============================================================================
// Static constant
// ============================================================================

inline const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

// ============================================================================
// Basic operations
// ============================================================================

inline Quaternion Quaternion::operator*(const Quaternion& other) const {
    // Quaternion multiplication: this * other
    return Quaternion(
        W * other.X + X * other.W + Y * other.Z - Z * other.Y, W * other.Y - X * other.Z + Y * other.W + Z * other.X,
        W * other.Z + X * other.Y - Y * other.X + Z * other.W, W * other.W - X * other.X - Y * other.Y - Z * other.Z);
}

inline Quaternion& Quaternion::operator*=(const Quaternion& other) {
    *this = *this * other;
    return *this;
}

inline Quaternion Quaternion::operator*(float scalar) const {
    return Quaternion(X * scalar, Y * scalar, Z * scalar, W * scalar);
}

inline Quaternion& Quaternion::operator*=(float scalar) {
    X *= scalar;
    Y *= scalar;
    Z *= scalar;
    W *= scalar;
    return *this;
}

inline Quaternion Quaternion::operator+(const Quaternion& other) const {
    return Quaternion(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
}

inline Quaternion& Quaternion::operator+=(const Quaternion& other) {
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    W += other.W;
    return *this;
}

inline Quaternion Quaternion::operator-(const Quaternion& other) const {
    return Quaternion(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
}

inline Quaternion& Quaternion::operator-=(const Quaternion& other) {
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    W -= other.W;
    return *this;
}

inline Quaternion Quaternion::operator-() const { return Quaternion(-X, -Y, -Z, -W); }

inline bool Quaternion::operator==(const Quaternion& other) const {
    return IsNearlyEqual(X, other.X) && IsNearlyEqual(Y, other.Y) && IsNearlyEqual(Z, other.Z) &&
           IsNearlyEqual(W, other.W);
}

inline bool Quaternion::operator!=(const Quaternion& other) const { return !(*this == other); }

// ============================================================================
// Vector rotation
// ============================================================================

inline Vector3f Quaternion::RotateVector(const Vector3f& vec) const {
    // Optimized version: v' = q * v * q^-1
    // Using formula: v' = v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v)
    Vector3f qvec(X, Y, Z);
    Vector3f cross1 = qvec.Cross(vec);
    Vector3f cross2 = qvec.Cross(cross1 + vec * W);
    return vec + cross2 * 2.0f;
}

inline Vector3f Quaternion::UnrotateVector(const Vector3f& vec) const {
    // Apply inverse rotation
    return Conjugate().RotateVector(vec);
}

// ============================================================================
// Quaternion properties
// ============================================================================

inline float Quaternion::Length() const { return SquareRoot(X * X + Y * Y + Z * Z + W * W); }

inline float Quaternion::LengthSquared() const { return X * X + Y * Y + Z * Z + W * W; }

inline void Quaternion::Normalize() {
    float len = Length();
    if (len > static_cast<float>(VERY_SMALL_NUMBER)) {
        float invLen = 1.0f / len;
        X *= invLen;
        Y *= invLen;
        Z *= invLen;
        W *= invLen;
    } else {
        *this = Identity;
    }
}

inline Quaternion Quaternion::Normalized() const {
    Quaternion result = *this;
    result.Normalize();
    return result;
}

inline bool Quaternion::IsNormalized(float tolerance) const { return Abs(LengthSquared() - 1.0f) <= tolerance; }

inline bool Quaternion::IsIdentity(float tolerance) const {
    return Abs(X) <= tolerance && Abs(Y) <= tolerance && Abs(Z) <= tolerance && Abs(W - 1.0f) <= tolerance;
}

// ============================================================================
// Quaternion operations
// ============================================================================

inline Quaternion Quaternion::Conjugate() const { return Quaternion(-X, -Y, -Z, W); }

inline Quaternion Quaternion::Inverse() const {
    float lenSq = LengthSquared();
    if (lenSq > static_cast<float>(VERY_SMALL_NUMBER)) {
        float invLenSq = 1.0f / lenSq;
        return Quaternion(-X * invLenSq, -Y * invLenSq, -Z * invLenSq, W * invLenSq);
    }
    return Identity;
}

inline float Quaternion::Dot(const Quaternion& other) const {
    return X * other.X + Y * other.Y + Z * other.Z + W * other.W;
}

inline float Quaternion::Dot(const Quaternion& a, const Quaternion& b) { return a.Dot(b); }

inline float Quaternion::AngleTo(const Quaternion& other) const {
    float dot = Abs(Dot(other));
    dot = Clamp(dot, 0.0f, 1.0f);
    return 2.0f * static_cast<float>(ACos(static_cast<double>(dot)));
}

inline float Quaternion::Angle(const Quaternion& a, const Quaternion& b) { return a.AngleTo(b); }

// ============================================================================
// Interpolation
// ============================================================================

inline Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float alpha) {
    // Linear interpolation (not normalized)
    float oneMinusAlpha = 1.0f - alpha;
    return Quaternion(a.X * oneMinusAlpha + b.X * alpha, a.Y * oneMinusAlpha + b.Y * alpha,
                      a.Z * oneMinusAlpha + b.Z * alpha, a.W * oneMinusAlpha + b.W * alpha);
}

inline Quaternion Quaternion::Nlerp(const Quaternion& a, const Quaternion& b, float alpha) {
    // Normalized linear interpolation
    Quaternion result = Lerp(a, b, alpha);
    result.Normalize();
    return result;
}

inline Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float alpha) {
    // Spherical linear interpolation
    Quaternion qb = b;

    float dot = a.Dot(b);

    // Ensure shortest path
    if (dot < 0.0f) {
        qb = -qb;
        dot = -dot;
    }

    // If quaternions are very close, use lerp to avoid division by zero
    if (dot > 0.9995f) {
        return Nlerp(a, qb, alpha);
    }

    // Clamp dot product
    dot = Clamp(dot, -1.0f, 1.0f);

    float theta = static_cast<float>(ACos(static_cast<double>(dot)));
    float sinTheta = static_cast<float>(Sin(static_cast<double>(theta)));

    float wa = static_cast<float>(Sin(static_cast<double>((1.0f - alpha) * theta))) / sinTheta;
    float wb = static_cast<float>(Sin(static_cast<double>(alpha * theta))) / sinTheta;

    return Quaternion(a.X * wa + qb.X * wb, a.Y * wa + qb.Y * wb, a.Z * wa + qb.Z * wb, a.W * wa + qb.W * wb);
}

inline Quaternion Quaternion::SlerpWithExtraSpins(const Quaternion& a, const Quaternion& b, float alpha,
                                                  int extraSpins) {
    // Slerp with additional full rotations
    float dot = a.Dot(b);
    float theta = static_cast<float>(ACos(static_cast<double>(Clamp(Abs(dot), -1.0f, 1.0f))));

    if (dot < 0.0f) {
        theta = -theta;
    }

    float totalTheta = theta + static_cast<float>(extraSpins) * static_cast<float>(TWO_PI);
    float sinTheta = static_cast<float>(Sin(static_cast<double>(totalTheta)));

    if (Abs(sinTheta) < static_cast<float>(VERY_SMALL_NUMBER)) {
        return Nlerp(a, b, alpha);
    }

    float wa = static_cast<float>(Sin(static_cast<double>((1.0f - alpha) * totalTheta))) / sinTheta;
    float wb = static_cast<float>(Sin(static_cast<double>(alpha * totalTheta))) / sinTheta;

    return Quaternion(a.X * wa + b.X * wb, a.Y * wa + b.Y * wb, a.Z * wa + b.Z * wb, a.W * wa + b.W * wb);
}

inline Quaternion Quaternion::Squad(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2,
                                    const Quaternion& q3, float alpha) {
    // Spherical quadrangle interpolation
    Quaternion c0 = Slerp(q0, q3, alpha);
    Quaternion c1 = Slerp(q1, q2, alpha);
    return Slerp(c0, c1, 2.0f * alpha * (1.0f - alpha));
}

// ============================================================================
// Conversion to other formats
// ============================================================================

inline void Quaternion::ToAxisAngle(Vector3f& outAxis, float& outAngle) const {
    Quaternion normalized = Normalized();
    outAngle = 2.0f * static_cast<float>(ACos(static_cast<double>(Clamp(normalized.W, -1.0f, 1.0f))));

    float sinHalfAngle = SquareRoot(1.0f - normalized.W * normalized.W);

    if (sinHalfAngle > static_cast<float>(VERY_SMALL_NUMBER)) {
        outAxis.X = normalized.X / sinHalfAngle;
        outAxis.Y = normalized.Y / sinHalfAngle;
        outAxis.Z = normalized.Z / sinHalfAngle;
    } else {
        // Angle is very small, axis is arbitrary
        outAxis = Vector3f::UnitX;
    }
}

inline Vector3f Quaternion::ToEuler() const {
    // Convert quaternion to Euler angles (radians)
    Vector3f euler;

    // Roll (X-axis rotation)
    float sinr_cosp = 2.0f * (W * X + Y * Z);
    float cosr_cosp = 1.0f - 2.0f * (X * X + Y * Y);
    euler.X = static_cast<float>(ATan2(static_cast<double>(sinr_cosp), static_cast<double>(cosr_cosp)));

    // Pitch (Y-axis rotation)
    float sinp = 2.0f * (W * Y - Z * X);
    if (Abs(sinp) >= 1.0f) {
        euler.Y = Math::CopySign(static_cast<float>(HALF_PI), sinp); // Use 90 degrees if out of range
    } else {
        euler.Y = static_cast<float>(ASin(static_cast<double>(sinp)));
    }

    // Yaw (Z-axis rotation)
    float siny_cosp = 2.0f * (W * Z + X * Y);
    float cosy_cosp = 1.0f - 2.0f * (Y * Y + Z * Z);
    euler.Z = static_cast<float>(ATan2(static_cast<double>(siny_cosp), static_cast<double>(cosy_cosp)));

    return euler;
}

inline Matrix3x3<float> Quaternion::ToRotationMatrix3x3() const {
    Matrix3x3<float> mat;

    float xx = X * X;
    float yy = Y * Y;
    float zz = Z * Z;
    float xy = X * Y;
    float xz = X * Z;
    float yz = Y * Z;
    float wx = W * X;
    float wy = W * Y;
    float wz = W * Z;

    mat(0, 0) = 1.0f - 2.0f * (yy + zz);
    mat(0, 1) = 2.0f * (xy - wz);
    mat(0, 2) = 2.0f * (xz + wy);

    mat(1, 0) = 2.0f * (xy + wz);
    mat(1, 1) = 1.0f - 2.0f * (xx + zz);
    mat(1, 2) = 2.0f * (yz - wx);

    mat(2, 0) = 2.0f * (xz - wy);
    mat(2, 1) = 2.0f * (yz + wx);
    mat(2, 2) = 1.0f - 2.0f * (xx + yy);

    return mat;
}

inline Matrix4x4<float> Quaternion::ToRotationMatrix4x4() const {
    Matrix3x3<float> mat3 = ToRotationMatrix3x3();
    return mat3.ToMatrix4x4();
}

inline Vector3f Quaternion::GetRotationAxis() const {
    float sinHalfAngle = SquareRoot(1.0f - W * W);

    if (sinHalfAngle > static_cast<float>(VERY_SMALL_NUMBER)) {
        return Vector3f(X / sinHalfAngle, Y / sinHalfAngle, Z / sinHalfAngle);
    }

    return Vector3f::UnitX; // Arbitrary axis for identity rotation
}

inline float Quaternion::GetRotationAngle() const {
    return 2.0f * static_cast<float>(ACos(static_cast<double>(Clamp(W, -1.0f, 1.0f))));
}

// ============================================================================
// Direction extraction
// ============================================================================

inline Vector3f Quaternion::GetForward() const {
    // Forward is +X in LH Z-up
    return RotateVector(Vector3f::UnitX);
}

inline Vector3f Quaternion::GetRight() const {
    // Right is +Y in LH Z-up
    return RotateVector(Vector3f::UnitY);
}

inline Vector3f Quaternion::GetUp() const {
    // Up is +Z in LH Z-up
    return RotateVector(Vector3f::UnitZ);
}

inline Vector3f Quaternion::GetBack() const { return -GetForward(); }

inline Vector3f Quaternion::GetLeft() const { return -GetRight(); }

inline Vector3f Quaternion::GetDown() const { return -GetUp(); }

// ============================================================================
// Utility functions
// ============================================================================

inline bool Quaternion::IsNaN() const { return Math::IsNaN(X) || Math::IsNaN(Y) || Math::IsNaN(Z) || Math::IsNaN(W); }

inline bool Quaternion::IsFinite() const {
    return !IsInfinite(X) && !IsInfinite(Y) && !IsInfinite(Z) && !IsInfinite(W) && !IsNaN();
}

inline Quaternion Quaternion::EnsureShortestPath(const Quaternion& quat, const Quaternion& reference) {
    // Ensure quaternion is in the same hemisphere as reference (shortest path for interpolation)
    if (quat.Dot(reference) < 0.0f) {
        return -quat;
    }
    return quat;
}

inline void Quaternion::ToSwingTwist(const Vector3f& axis, Quaternion& outSwing, Quaternion& outTwist) const {
    // Decompose rotation into swing (rotation perpendicular to axis) and twist (rotation around axis)
    Vector3f normalizedAxis = axis.Normalized();

    // Project quaternion onto the axis
    Vector3f projection(X, Y, Z);
    float dot = projection.Dot(normalizedAxis);
    projection = normalizedAxis * dot;

    // Twist quaternion
    outTwist = Quaternion(projection.X, projection.Y, projection.Z, W);
    outTwist.Normalize();

    // Swing quaternion
    outSwing = *this * outTwist.Conjugate();
}

inline Quaternion Quaternion::FromSwingTwist(const Quaternion& swing, const Quaternion& twist) { return swing * twist; }

// ============================================================================
// Debugging
// ============================================================================

inline Containers::String Quaternion::ToString() const {
    return Containers::String::Format(u"Quaternion(X: {;p=6}, Y: {;p=6}, Z: {;p=6}, W: {;p=6})", X, Y, Z, W);
}

inline Containers::String Quaternion::ToStringCompact() const {
    return Containers::String::Format(u"Q({;p=3}, {;p=3}, {;p=3}, {;p=3})", X, Y, Z, W);
}

// ============================================================================
// Free functions
// ============================================================================

inline Quaternion operator*(float scalar, const Quaternion& quat) { return quat * scalar; }

} // namespace Edvar::Math
