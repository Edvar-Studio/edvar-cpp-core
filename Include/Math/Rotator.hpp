#pragma once
#include "Math/Vector.hpp"

namespace Edvar::Math {

// Forward declarations
struct Quaternion;
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Matrix3x3;
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Matrix4x4;
// End forward declarations

/**
 * @brief Rotator for representing rotations using Euler angles.
 * 
 * Stores rotation as Pitch (X-axis), Yaw (Y-axis), and Roll (Z-axis) in degrees.
 * This is a more intuitive representation than quaternions for many use cases,
 * but suffers from gimbal lock. Use Quaternion for interpolation and composition.
 * 
 * Coordinate System: Left-Handed Z-Up
 * - Pitch: Rotation around X-axis (up/down)
 * - Yaw: Rotation around Y-axis (left/right)
 * - Roll: Rotation around Z-axis (tilt/banking)
 * 
 * Angles are stored in degrees for better readability.
 */
struct Rotator {
    // ========================================================================
    // Data storage
    // ========================================================================
    
    double Pitch;  // Rotation around X-axis (degrees)
    double Yaw;    // Rotation around Y-axis (degrees)
    double Roll;   // Rotation around Z-axis (degrees)

    // ========================================================================
    // Constructors
    // ========================================================================
    
    /** Default constructor - creates zero rotation */
    Rotator();
    
    /** Construct from pitch, yaw, roll (in degrees) */
    Rotator(double pitch, double yaw, double roll);
    
    /** Construct from Vector3d (pitch, yaw, roll in degrees) */
    explicit Rotator(const Vector3d& eulerDegrees);

    // Static factory methods
    
    /** Create rotator from quaternion */
    static Rotator FromQuaternion(const Quaternion& quat);
    
    /** Create rotator from rotation matrix */
    static Rotator FromRotationMatrix(const Matrix3x3<float>& matrix);
    static Rotator FromRotationMatrix(const Matrix4x4<float>& matrix);
    
    /** Create rotator that looks from one point to another */
    static Rotator LookAt(const Vector3d& from, const Vector3d& to, const Vector3d& up = Vector3d::Up);
    
    /** Create rotator from direction vector */
    static Rotator FromDirection(const Vector3d& direction, const Vector3d& up = Vector3d::Up);

    // ========================================================================
    // Static constants
    // ========================================================================
    
    /** Zero rotation (0, 0, 0) */
    static const Rotator Zero;

    // ========================================================================
    // Conversion
    // ========================================================================
    
    /** Convert to quaternion */
    Quaternion ToQuaternion() const;
    
    /** Convert to rotation matrix (3x3) */
    Matrix3x3<float> ToRotationMatrix3x3() const;
    
    /** Convert to rotation matrix (4x4) */
    Matrix4x4<float> ToRotationMatrix4x4() const;
    
    /** Convert to radians (returns Vector3d with pitch, yaw, roll in radians) */
    Vector3d ToRadians() const;
    
    /** Convert to vector (returns Vector3d with pitch, yaw, roll in degrees) */
    Vector3d ToVector() const;

    // ========================================================================
    // Normalization
    // ========================================================================
    
    /** Normalize angles to [-180, 180] range */
    void Normalize();
    
    /** Return normalized copy with angles in [-180, 180] range */
    Rotator Normalized() const;
    
    /** Clamp angles to specified range */
    void Clamp(double minPitch, double maxPitch, double minYaw, double maxYaw, double minRoll, double maxRoll);
    
    /** Return clamped copy */
    Rotator Clamped(double minPitch, double maxPitch, double minYaw, double maxYaw, double minRoll, double maxRoll) const;

    // ========================================================================
    // Operators
    // ========================================================================
    
    /** Add rotations (component-wise, not proper rotation composition) */
    Rotator operator+(const Rotator& other) const;
    Rotator& operator+=(const Rotator& other);
    
    /** Subtract rotations (component-wise) */
    Rotator operator-(const Rotator& other) const;
    Rotator& operator-=(const Rotator& other);
    
    /** Scale rotation */
    Rotator operator*(double scale) const;
    Rotator& operator*=(double scale);
    
    /** Negate rotation */
    Rotator operator-() const;
    
    /** Equality comparison */
    bool operator==(const Rotator& other) const;
    bool operator!=(const Rotator& other) const;

    // ========================================================================
    // Rotation operations
    // ========================================================================
    
    /** Rotate a vector by this rotation */
    Vector3d RotateVector(const Vector3d& vec) const;
    
    /** Unrotate a vector (apply inverse rotation) */
    Vector3d UnrotateVector(const Vector3d& vec) const;
    
    /** Get inverse rotation */
    Rotator Inverse() const;
    
    /** Compose rotations (converts to quaternions for proper composition) */
    Rotator Compose(const Rotator& other) const;

    // ========================================================================
    // Direction vectors
    // ========================================================================
    
    /** Get forward direction vector (+X in LH Z-up) */
    Vector3d GetForward() const;
    
    /** Get right direction vector (+Y in LH Z-up) */
    Vector3d GetRight() const;
    
    /** Get up direction vector (+Z in LH Z-up) */
    Vector3d GetUp() const;
    
    /** Get back direction vector (-X) */
    Vector3d GetBack() const;
    
    /** Get left direction vector (-Y) */
    Vector3d GetLeft() const;
    
    /** Get down direction vector (-Z) */
    Vector3d GetDown() const;
    
    /** Get all basis vectors at once */
    void GetAxes(Vector3d& outForward, Vector3d& outRight, Vector3d& outUp) const;

    // ========================================================================
    // Interpolation
    // ========================================================================
    
    /** Linear interpolation (component-wise, may have issues with angle wrapping) */
    static Rotator Lerp(const Rotator& a, const Rotator& b, double alpha);
    
    /** Spherical linear interpolation (converts to quaternions) */
    static Rotator Slerp(const Rotator& a, const Rotator& b, double alpha);
    
    /** Linear interpolation to target */
    Rotator LerpTo(const Rotator& target, double alpha) const;
    
    /** Spherical linear interpolation to target */
    Rotator SlerpTo(const Rotator& target, double alpha) const;

    // ========================================================================
    // Comparison and validation
    // ========================================================================
    
    /** Check if nearly equal with tolerance */
    bool IsNearlyEqual(const Rotator& other, double tolerance = 1e-4) const;
    
    /** Check if this is zero rotation */
    bool IsZero(double tolerance = 1e-6) const;
    
    /** Check if values are finite (not NaN or infinite) */
    bool IsFinite() const;
    
    /** Get shortest angular distance to another rotator */
    double AngularDistance(const Rotator& other) const;

    // ========================================================================
    // Utility functions
    // ========================================================================
    
    /** Get the axis with the largest rotation */
    int GetDominantAxis() const;
    
    /** Convert from radians to degrees */
    static Rotator FromRadians(double pitch, double yaw, double roll);
    static Rotator FromRadians(const Vector3d& radiansVec);
    
    /** Normalize angle to [-180, 180] range */
    static double NormalizeAngle(double angle);
    
    /** Get shortest angular difference between two angles */
    static double AngularDifference(double angle1, double angle2);

    // ========================================================================
    // Debugging
    // ========================================================================
    
    /** Convert to string representation */
    Containers::String ToString() const;
    
    /** Convert to compact string representation */
    Containers::String ToStringCompact() const;
};

// Free function for scalar multiplication from left
Rotator operator*(double scale, const Rotator& rotator);

} // namespace Edvar::Math
