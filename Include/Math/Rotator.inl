#pragma once
#include "Rotator.hpp"
#include "Quaternion.hpp"
#include "Matrix.hpp"
#include "Math/Math.hpp"
#include <cmath>

namespace Edvar::Math {

// ============================================================================
// Constructors
// ============================================================================

inline Rotator::Rotator() : Pitch(0.0), Yaw(0.0), Roll(0.0) {}

inline Rotator::Rotator(double pitch, double yaw, double roll) 
    : Pitch(pitch), Yaw(yaw), Roll(roll) {}

inline Rotator::Rotator(const Vector3d& eulerDegrees) 
    : Pitch(eulerDegrees.X), Yaw(eulerDegrees.Y), Roll(eulerDegrees.Z) {}

// ============================================================================
// Static factory methods
// ============================================================================

inline Rotator Rotator::FromQuaternion(const Quaternion& quat) {
    // Convert quaternion to Euler angles (radians), then to degrees
    Vector3f eulerRad = quat.ToEuler();
    return Rotator(
        RadToDeg(static_cast<double>(eulerRad.X)),
        RadToDeg(static_cast<double>(eulerRad.Y)),
        RadToDeg(static_cast<double>(eulerRad.Z))
    );
}

inline Rotator Rotator::FromRotationMatrix(const Matrix3x3<float>& matrix) {
    Quaternion quat = Quaternion::FromRotationMatrix(matrix);
    return FromQuaternion(quat);
}

inline Rotator Rotator::FromRotationMatrix(const Matrix4x4<float>& matrix) {
    Quaternion quat = Quaternion::FromRotationMatrix(matrix);
    return FromQuaternion(quat);
}

inline Rotator Rotator::LookAt(const Vector3d& from, const Vector3d& to, const Vector3d& up) {
    Vector3f forwardF(
        static_cast<float>(to.X - from.X),
        static_cast<float>(to.Y - from.Y),
        static_cast<float>(to.Z - from.Z)
    );
    Vector3f upF(static_cast<float>(up.X), static_cast<float>(up.Y), static_cast<float>(up.Z));
    
    Quaternion quat = Quaternion::LookRotation(forwardF, upF);
    return FromQuaternion(quat);
}

inline Rotator Rotator::FromDirection(const Vector3d& direction, const Vector3d& up) {
    Vector3f dirF(static_cast<float>(direction.X), static_cast<float>(direction.Y), static_cast<float>(direction.Z));
    Vector3f upF(static_cast<float>(up.X), static_cast<float>(up.Y), static_cast<float>(up.Z));
    
    Quaternion quat = Quaternion::LookRotation(dirF, upF);
    return FromQuaternion(quat);
}

// ============================================================================
// Static constant
// ============================================================================

const Rotator Rotator::Zero = Rotator(0.0, 0.0, 0.0);

// ============================================================================
// Conversion
// ============================================================================

inline Quaternion Rotator::ToQuaternion() const {
    // Convert degrees to radians
    float pitchRad = static_cast<float>(DegToRad(Pitch));
    float yawRad = static_cast<float>(DegToRad(Yaw));
    float rollRad = static_cast<float>(DegToRad(Roll));
    
    return Quaternion::FromEuler(pitchRad, yawRad, rollRad);
}

inline Matrix3x3<float> Rotator::ToRotationMatrix3x3() const {
    return ToQuaternion().ToRotationMatrix3x3();
}

inline Matrix4x4<float> Rotator::ToRotationMatrix4x4() const {
    return ToQuaternion().ToRotationMatrix4x4();
}

inline Vector3d Rotator::ToRadians() const {
    return Vector3d(
        DegToRad(Pitch),
        DegToRad(Yaw),
        DegToRad(Roll)
    );
}

inline Vector3d Rotator::ToVector() const {
    return Vector3d(Pitch, Yaw, Roll);
}

// ============================================================================
// Normalization
// ============================================================================

inline void Rotator::Normalize() {
    Pitch = NormalizeAngle(Pitch);
    Yaw = NormalizeAngle(Yaw);
    Roll = NormalizeAngle(Roll);
}

inline Rotator Rotator::Normalized() const {
    return Rotator(
        NormalizeAngle(Pitch),
        NormalizeAngle(Yaw),
        NormalizeAngle(Roll)
    );
}

inline void Rotator::Clamp(double minPitch, double maxPitch, double minYaw, double maxYaw, double minRoll, double maxRoll) {
    Pitch = Math::Clamp(Pitch, minPitch, maxPitch);
    Yaw = Math::Clamp(Yaw, minYaw, maxYaw);
    Roll = Math::Clamp(Roll, minRoll, maxRoll);
}

inline Rotator Rotator::Clamped(double minPitch, double maxPitch, double minYaw, double maxYaw, double minRoll, double maxRoll) const {
    return Rotator(
        Math::Clamp(Pitch, minPitch, maxPitch),
        Math::Clamp(Yaw, minYaw, maxYaw),
        Math::Clamp(Roll, minRoll, maxRoll)
    );
}

// ============================================================================
// Operators
// ============================================================================

inline Rotator Rotator::operator+(const Rotator& other) const {
    return Rotator(Pitch + other.Pitch, Yaw + other.Yaw, Roll + other.Roll);
}

inline Rotator& Rotator::operator+=(const Rotator& other) {
    Pitch += other.Pitch;
    Yaw += other.Yaw;
    Roll += other.Roll;
    return *this;
}

inline Rotator Rotator::operator-(const Rotator& other) const {
    return Rotator(Pitch - other.Pitch, Yaw - other.Yaw, Roll - other.Roll);
}

inline Rotator& Rotator::operator-=(const Rotator& other) {
    Pitch -= other.Pitch;
    Yaw -= other.Yaw;
    Roll -= other.Roll;
    return *this;
}

inline Rotator Rotator::operator*(double scale) const {
    return Rotator(Pitch * scale, Yaw * scale, Roll * scale);
}

inline Rotator& Rotator::operator*=(double scale) {
    Pitch *= scale;
    Yaw *= scale;
    Roll *= scale;
    return *this;
}

inline Rotator Rotator::operator-() const {
    return Rotator(-Pitch, -Yaw, -Roll);
}

inline bool Rotator::operator==(const Rotator& other) const {
    return Math::IsNearlyEqual(Pitch, other.Pitch) && 
           Math::IsNearlyEqual(Yaw, other.Yaw) && 
           Math::IsNearlyEqual(Roll, other.Roll);
}

inline bool Rotator::operator!=(const Rotator& other) const {
    return !(*this == other);
}

// ============================================================================
// Rotation operations
// ============================================================================

inline Vector3d Rotator::RotateVector(const Vector3d& vec) const {
    Quaternion quat = ToQuaternion();
    Vector3f vecF(static_cast<float>(vec.X), static_cast<float>(vec.Y), static_cast<float>(vec.Z));
    Vector3f rotatedF = quat.RotateVector(vecF);
    return Vector3d(
        static_cast<double>(rotatedF.X),
        static_cast<double>(rotatedF.Y),
        static_cast<double>(rotatedF.Z)
    );
}

inline Vector3d Rotator::UnrotateVector(const Vector3d& vec) const {
    Quaternion quat = ToQuaternion();
    Vector3f vecF(static_cast<float>(vec.X), static_cast<float>(vec.Y), static_cast<float>(vec.Z));
    Vector3f unrotatedF = quat.UnrotateVector(vecF);
    return Vector3d(
        static_cast<double>(unrotatedF.X),
        static_cast<double>(unrotatedF.Y),
        static_cast<double>(unrotatedF.Z)
    );
}

inline Rotator Rotator::Inverse() const {
    // For Euler angles, inverse is simply negation
    return Rotator(-Pitch, -Yaw, -Roll);
}

inline Rotator Rotator::Compose(const Rotator& other) const {
    // Convert to quaternions for proper composition
    Quaternion q1 = ToQuaternion();
    Quaternion q2 = other.ToQuaternion();
    Quaternion result = q1 * q2;
    return FromQuaternion(result);
}

// ============================================================================
// Direction vectors
// ============================================================================

inline Vector3d Rotator::GetForward() const {
    Quaternion quat = ToQuaternion();
    Vector3f forwardF = quat.GetForward();
    return Vector3d(
        static_cast<double>(forwardF.X),
        static_cast<double>(forwardF.Y),
        static_cast<double>(forwardF.Z)
    );
}

inline Vector3d Rotator::GetRight() const {
    Quaternion quat = ToQuaternion();
    Vector3f rightF = quat.GetRight();
    return Vector3d(
        static_cast<double>(rightF.X),
        static_cast<double>(rightF.Y),
        static_cast<double>(rightF.Z)
    );
}

inline Vector3d Rotator::GetUp() const {
    Quaternion quat = ToQuaternion();
    Vector3f upF = quat.GetUp();
    return Vector3d(
        static_cast<double>(upF.X),
        static_cast<double>(upF.Y),
        static_cast<double>(upF.Z)
    );
}

inline Vector3d Rotator::GetBack() const {
    return -GetForward();
}

inline Vector3d Rotator::GetLeft() const {
    return -GetRight();
}

inline Vector3d Rotator::GetDown() const {
    return -GetUp();
}

inline void Rotator::GetAxes(Vector3d& outForward, Vector3d& outRight, Vector3d& outUp) const {
    Quaternion quat = ToQuaternion();
    
    Vector3f forwardF = quat.GetForward();
    Vector3f rightF = quat.GetRight();
    Vector3f upF = quat.GetUp();
    
    outForward = Vector3d(static_cast<double>(forwardF.X), static_cast<double>(forwardF.Y), static_cast<double>(forwardF.Z));
    outRight = Vector3d(static_cast<double>(rightF.X), static_cast<double>(rightF.Y), static_cast<double>(rightF.Z));
    outUp = Vector3d(static_cast<double>(upF.X), static_cast<double>(upF.Y), static_cast<double>(upF.Z));
}

// ============================================================================
// Interpolation
// ============================================================================

inline Rotator Rotator::Lerp(const Rotator& a, const Rotator& b, double alpha) {
    // Component-wise linear interpolation with angle wrapping
    double pitch = a.Pitch + AngularDifference(a.Pitch, b.Pitch) * alpha;
    double yaw = a.Yaw + AngularDifference(a.Yaw, b.Yaw) * alpha;
    double roll = a.Roll + AngularDifference(a.Roll, b.Roll) * alpha;
    
    return Rotator(pitch, yaw, roll);
}

inline Rotator Rotator::Slerp(const Rotator& a, const Rotator& b, double alpha) {
    // Convert to quaternions for proper spherical interpolation
    Quaternion q1 = a.ToQuaternion();
    Quaternion q2 = b.ToQuaternion();
    Quaternion result = Quaternion::Slerp(q1, q2, static_cast<float>(alpha));
    return FromQuaternion(result);
}

inline Rotator Rotator::LerpTo(const Rotator& target, double alpha) const {
    return Lerp(*this, target, alpha);
}

inline Rotator Rotator::SlerpTo(const Rotator& target, double alpha) const {
    return Slerp(*this, target, alpha);
}

// ============================================================================
// Comparison and validation
// ============================================================================

inline bool Rotator::IsNearlyEqual(const Rotator& other, double tolerance) const {
    return Abs(AngularDifference(Pitch, other.Pitch)) <= tolerance &&
           Abs(AngularDifference(Yaw, other.Yaw)) <= tolerance &&
           Abs(AngularDifference(Roll, other.Roll)) <= tolerance;
}

inline bool Rotator::IsZero(double tolerance) const {
    return Abs(Pitch) <= tolerance && Abs(Yaw) <= tolerance && Abs(Roll) <= tolerance;
}

inline bool Rotator::IsFinite() const {
    return !IsInfinite(Pitch) && !IsInfinite(Yaw) && !IsInfinite(Roll) &&
           !IsNaN(Pitch) && !IsNaN(Yaw) && !IsNaN(Roll);
}

inline double Rotator::AngularDistance(const Rotator& other) const {
    // Use quaternion angle for proper distance
    Quaternion q1 = ToQuaternion();
    Quaternion q2 = other.ToQuaternion();
    return static_cast<double>(q1.AngleTo(q2));
}

// ============================================================================
// Utility functions
// ============================================================================

inline int Rotator::GetDominantAxis() const {
    double absPitch = Abs(Pitch);
    double absYaw = Abs(Yaw);
    double absRoll = Abs(Roll);
    
    if (absPitch >= absYaw && absPitch >= absRoll) return 0; // Pitch
    if (absYaw >= absRoll) return 1; // Yaw
    return 2; // Roll
}

inline Rotator Rotator::FromRadians(double pitch, double yaw, double roll) {
    return Rotator(
        RadToDeg(pitch),
        RadToDeg(yaw),
        RadToDeg(roll)
    );
}

inline Rotator Rotator::FromRadians(const Vector3d& radiansVec) {
    return FromRadians(radiansVec.X, radiansVec.Y, radiansVec.Z);
}

inline double Rotator::NormalizeAngle(double angle) {
    // Normalize to [-180, 180] range
    angle = std::fmod(angle, 360.0);
    if (angle > 180.0) {
        angle -= 360.0;
    } else if (angle < -180.0) {
        angle += 360.0;
    }
    return angle;
}

inline double Rotator::AngularDifference(double angle1, double angle2) {
    // Get shortest angular difference
    double diff = angle2 - angle1;
    diff = std::fmod(diff + 180.0, 360.0);
    if (diff < 0.0) {
        diff += 360.0;
    }
    return diff - 180.0;
}

// ============================================================================
// Debugging
// ============================================================================

inline Containers::String Rotator::ToString() const {
    return Containers::String::PrintF(
        u"Rotator(Pitch: %.3f°, Yaw: %.3f°, Roll: %.3f°)",
        Pitch, Yaw, Roll
    );
}

inline Containers::String Rotator::ToStringCompact() const {
    return Containers::String::PrintF(u"R(%.1f°, %.1f°, %.1f°)", Pitch, Yaw, Roll);
}

// ============================================================================
// Free functions
// ============================================================================

inline Rotator operator*(double scale, const Rotator& rotator) {
    return rotator * scale;
}

} // namespace Edvar::Math
