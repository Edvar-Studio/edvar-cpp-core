#pragma once

#include "Math/Vector.hpp"

namespace Edvar::Math {

// Forward declarations
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Matrix4x4;

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Matrix3x3;
// End forward declarations

/**
 * @brief Quaternion for representing rotations in 3D space.
 * Uses XYZW format where (X, Y, Z) is the imaginary part and W is the real
 * part. Quaternion = W + Xi + Yj + Zk
 */
struct alignas(16) Quaternion {
    // ========================================================================
    // Data storage
    // ========================================================================
    union {
        struct {
            float X, Y, Z, W; // Imaginary (X, Y, Z) + Real (W)
        };
        float XYZW[4];
        SIMD::Float_4 SIMDData;
    };

    // ========================================================================
    // Constructors
    // ========================================================================

    /** Default constructor - creates identity quaternion */
    Quaternion();

    /** Construct from components (imaginary X, Y, Z and real W) */
    Quaternion(float x, float y, float z, float w);

    /** Construct from SIMD vector */
    explicit Quaternion(const SIMD::Float_4& simd);

    /** Construct from axis and angle (radians) */
    static Quaternion FromAxisAngle(const Vector3f& axis, float angleRadians);

    /** Construct from Euler angles (radians) - pitch (X), yaw (Y), roll (Z) */
    static Quaternion FromEuler(float pitch, float yaw, float roll);
    static Quaternion FromEuler(const Vector3f& eulerAngles);

    /** Construct from rotation matrix */
    static Quaternion FromRotationMatrix(const Matrix3x3<float>& rotationMatrix);
    static Quaternion FromRotationMatrix(const Matrix4x4<float>& transformMatrix);

    /** Construct quaternion that rotates from one direction to another */
    static Quaternion FromToRotation(const Vector3f& from, const Vector3f& to);

    /** Construct quaternion from forward and up vectors */
    static Quaternion LookRotation(const Vector3f& forward, const Vector3f& up = Vector3f::Up);

    // ========================================================================
    // Static constants
    // ========================================================================

    static const Quaternion Identity;

    // ========================================================================
    // Basic operations
    // ========================================================================

    /** Quaternion multiplication (composition of rotations) */
    Quaternion operator*(const Quaternion& other) const;

    /** Compound multiplication */
    Quaternion& operator*=(const Quaternion& other);

    /** Scalar multiplication */
    Quaternion operator*(float scalar) const;
    Quaternion& operator*=(float scalar);

    /** Quaternion addition (not rotation composition) */
    Quaternion operator+(const Quaternion& other) const;
    Quaternion& operator+=(const Quaternion& other);

    /** Quaternion subtraction */
    Quaternion operator-(const Quaternion& other) const;
    Quaternion& operator-=(const Quaternion& other);

    /** Unary negation */
    Quaternion operator-() const;

    /** Equality comparison */
    bool operator==(const Quaternion& other) const;
    bool operator!=(const Quaternion& other) const;

    // ========================================================================
    // Vector rotation
    // ========================================================================

    /** Rotate a vector by this quaternion */
    Vector3f RotateVector(const Vector3f& vec) const;

    /** Unrotate a vector (apply inverse rotation) */
    Vector3f UnrotateVector(const Vector3f& vec) const;

    // ========================================================================
    // Quaternion properties
    // ========================================================================

    /** Get the length (magnitude) of the quaternion */
    float Length() const;

    /** Get the squared length of the quaternion */
    float LengthSquared() const;

    /** Normalize the quaternion in-place */
    void Normalize();

    /** Get normalized copy of the quaternion */
    Quaternion Normalized() const;

    /** Check if quaternion is normalized */
    bool IsNormalized(float tolerance = 1e-4f) const;

    /** Check if quaternion is identity */
    bool IsIdentity(float tolerance = 1e-4f) const;

    // ========================================================================
    // Quaternion operations
    // ========================================================================

    /** Get the conjugate of the quaternion (inverse for unit quaternions) */
    Quaternion Conjugate() const;

    /** Get the inverse of the quaternion */
    Quaternion Inverse() const;

    /** Dot product of two quaternions */
    float Dot(const Quaternion& other) const;
    static float Dot(const Quaternion& a, const Quaternion& b);

    /** Get angle (in radians) between two quaternions */
    float AngleTo(const Quaternion& other) const;
    static float Angle(const Quaternion& a, const Quaternion& b);

    // ========================================================================
    // Interpolation
    // ========================================================================

    /** Linear interpolation (not recommended for rotations, use Slerp instead) */
    static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float alpha);

    /** Normalized linear interpolation (faster than Slerp, good approximation) */
    static Quaternion Nlerp(const Quaternion& a, const Quaternion& b, float alpha);

    /** Spherical linear interpolation (smooth rotation interpolation) */
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float alpha);

    /** Spherical linear interpolation with extra spins */
    static Quaternion SlerpWithExtraSpins(const Quaternion& a, const Quaternion& b, float alpha, int extraSpins);

    /** Squad interpolation (smooth interpolation through multiple rotations) */
    static Quaternion Squad(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2, const Quaternion& q3,
                            float alpha);

    // ========================================================================
    // Conversion to other formats
    // ========================================================================

    /** Convert to axis-angle representation */
    void ToAxisAngle(Vector3f& outAxis, float& outAngle) const;

    /** Convert to Euler angles (radians) - pitch (X), yaw (Y), roll (Z) */
    Vector3f ToEuler() const;

    /** Convert to rotation matrix */
    Matrix3x3<float> ToRotationMatrix3x3() const;
    Matrix4x4<float> ToRotationMatrix4x4() const;

    /** Get the rotation axis (for non-identity quaternions) */
    Vector3f GetRotationAxis() const;

    /** Get the rotation angle in radians */
    float GetRotationAngle() const;

    // ========================================================================
    // Direction extraction
    // ========================================================================

    /** Get forward direction (rotated +X axis) */
    Vector3f GetForward() const;

    /** Get right direction (rotated +Y axis) */
    Vector3f GetRight() const;

    /** Get up direction (rotated +Z axis) */
    Vector3f GetUp() const;

    /** Get back direction (rotated -X axis) */
    Vector3f GetBack() const;

    /** Get left direction (rotated -Y axis) */
    Vector3f GetLeft() const;

    /** Get down direction (rotated -Z axis) */
    Vector3f GetDown() const;

    // ========================================================================
    // Utility functions
    // ========================================================================

    /** Check if quaternion contains any NaN values */
    bool IsNaN() const;

    /** Check if quaternion is finite (no inf/NaN) */
    bool IsFinite() const;

    /** Ensure quaternion is in the same hemisphere as reference (for
     * interpolation) */
    static Quaternion EnsureShortestPath(const Quaternion& quat, const Quaternion& reference);

    /** Get swing and twist decomposition around an axis */
    void ToSwingTwist(const Vector3f& axis, Quaternion& outSwing, Quaternion& outTwist) const;

    /** Create quaternion from swing and twist components */
    static Quaternion FromSwingTwist(const Quaternion& swing, const Quaternion& twist);

    // ========================================================================
    // Debugging
    // ========================================================================

    /** Convert to string representation */
    Containers::String ToString() const;
    Containers::String ToStringCompact() const;
};

// Scalar multiplication from left side
Quaternion operator*(float scalar, const Quaternion& quat);

} // namespace Edvar::Math
