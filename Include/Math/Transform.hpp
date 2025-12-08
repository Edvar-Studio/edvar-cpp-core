#pragma once
#include "Matrix.hpp"
#include "Vector.hpp"
#include "Quaternion.hpp"

namespace Edvar::Math {
struct Transform {
    // ========================================================================
    // Constructors
    // ========================================================================
    Transform();
    explicit Transform(const Matrix4x4d& matrix);
    explicit Transform(const Matrix4x4f& matrix);
    explicit Transform(const Vector3d& translation = Vector3d::Zero, const Quaternion& rotation = Quaternion::Identity,
                       const Vector3d& scale = Vector3d::One);
    explicit Transform(const Vector3f& translation, const Quaternion& rotation = Quaternion::Identity,
                       const Vector3f& scale = Vector3f::One);

    // Static factory methods
    static Transform FromMatrix(const Matrix4x4d& matrix);
    static Transform FromAxisAngle(const Vector3d& translation, const Vector3d& axis, double angle,
                                   const Vector3d& scale = Vector3d::One);
    static Transform FromEuler(const Vector3d& translation, const Vector3d& eulerAngles,
                               const Vector3d& scale = Vector3d::One);
    static Transform LookAt(const Vector3d& from, const Vector3d& to, const Vector3d& up = Vector3d::Up);
    static Transform TranslationOnly(const Vector3d& translation);
    static Transform RotationOnly(const Quaternion& rotation);
    static Transform ScaleOnly(const Vector3d& scale);

    // ========================================================================
    // Matrix conversion
    // ========================================================================
    void SetFromMatrix(const Matrix4x4d& matrix);
    Matrix4x4d ToMatrix() const;
    Matrix4x4f ToMatrix4x4f() const;
    Matrix4x4d ToMatrix4x4d() const;
    Matrix3x3d ToMatrix3x3d() const;
    void ToMatrix(Matrix4x4d& outMatrix) const;

    // ========================================================================
    // Component getters/setters
    // ========================================================================
    void SetTranslation(const Vector3d& translation);
    void SetRotation(const Quaternion& rotation);
    void SetScale(const Vector3d& scale);

    Vector3d GetTranslation() const;
    Quaternion GetRotation() const;
    Vector3d GetScale() const;

    // Immutable setters (return new transform)
    Transform WithTranslation(const Vector3d& newTranslation) const;
    Transform WithRotation(const Quaternion& newRotation) const;
    Transform WithScale(const Vector3d& newScale) const;
    Transform WithUniformScale(double uniformScale) const;

    // ========================================================================
    // In-place transformations
    // ========================================================================
    void TranslateInPlace(const Vector3d& delta);
    void RotateInPlace(const Quaternion& delta);
    void ScaleInPlace(const Vector3d& delta);
    void SetIdentity();
    void NormalizeRotation();

    // ========================================================================
    // Immutable transformations
    // ========================================================================
    Transform ApplyTranslation(const Vector3d& delta) const;
    Transform ApplyRotation(const Quaternion& delta) const;
    Transform ApplyScale(const Vector3d& delta) const;
    Transform Normalized() const;
    Transform RemoveScale() const;

    // ========================================================================
    // Operators
    // ========================================================================
    Transform& operator*=(const Transform& other);
    Transform operator*(const Transform& other) const;
    Transform Combine(const Transform& other) const;

    Transform& operator/=(const Transform& other);
    Transform operator/(const Transform& other) const;

    Transform& operator+=(const Transform& other);
    Transform operator+(const Transform& other) const;

    Transform& operator-=(const Transform& other);
    Transform operator-(const Transform& other) const;

    bool operator==(const Transform& other) const;
    bool operator!=(const Transform& other) const;

    // ========================================================================
    // Inverse and relative transforms
    // ========================================================================
    Transform Inverse() const;
    Transform GetRelativeTransform(const Transform& parent) const;
    Transform GetLocalTransform(const Transform& parent) const;
    Transform GetWorldTransform(const Transform& parent) const;
    static Transform ComposeTransforms(const Transform& parent, const Transform& local);

    // ========================================================================
    // Point/Vector/Rotation transformation
    // ========================================================================
    Vector3d TransformPoint(const Vector3d& point) const;
    Vector3d TransformDirection(const Vector3d& direction) const;
    Vector3d TransformScale(const Vector3d& scale) const;
    Quaternion TransformRotation(const Quaternion& rotation) const;

    Vector3d InverseTransformPoint(const Vector3d& point) const;
    Vector3d InverseTransformDirection(const Vector3d& direction) const;

    // Coordinate space transformations
    Vector3d TransformPointToWorld(const Vector3d& localPoint) const;
    Vector3d TransformPointToLocal(const Vector3d& worldPoint) const;
    Vector3d TransformVectorToWorld(const Vector3d& localVector) const;
    Vector3d TransformVectorToLocal(const Vector3d& worldVector) const;
    Quaternion TransformRotationToWorld(const Quaternion& localRotation) const;
    Quaternion TransformRotationToLocal(const Quaternion& worldRotation) const;

    // ========================================================================
    // Interpolation
    // ========================================================================
    static Transform Lerp(const Transform& a, const Transform& b, double alpha);
    static Transform Slerp(const Transform& a, const Transform& b, double alpha);
    Transform LerpTo(const Transform& target, double alpha) const;
    Transform SlerpTo(const Transform& target, double alpha) const;

    // ========================================================================
    // Validation and comparison
    // ========================================================================
    bool IsNearlyEqual(const Transform& other, double tolerance = 1e-4) const;
    bool IsIdentity(double tolerance = 1e-4) const;
    bool IsValid() const;
    bool HasUniformScale(double tolerance = 1e-4) const;
    bool HasNegativeScale() const;

    // ========================================================================
    // Direction vectors
    // ========================================================================
    Vector3d GetForward() const;
    Vector3d GetRight() const;
    Vector3d GetUp() const;
    Vector3d GetBack() const;
    Vector3d GetLeft() const;
    Vector3d GetDown() const;

    // ========================================================================
    // Advanced transformation utilities
    // ========================================================================
    Transform MirrorX() const;
    Transform MirrorY() const;
    Transform MirrorZ() const;
    Transform Mirror(const Vector3d& planeNormal) const;

    Transform RotateAround(const Vector3d& point, const Quaternion& rotation) const;
    Transform RotateAround(const Vector3d& point, const Vector3d& axis, double angle) const;
    Transform ScaleAround(const Vector3d& pivot, const Vector3d& scale) const;
    Transform LookAtDirection(const Vector3d& direction, const Vector3d& up = Vector3d::Up);

    Vector3d GetScaleSign() const;

    // ========================================================================
    // Debugging
    // ========================================================================
    Containers::String ToString() const;
    Containers::String ToStringCompact() const;

    // ========================================================================
    // Static constants
    // ========================================================================
    static const Transform Identity;

private:
    Vector3d Translation;
    Quaternion Rotation;
    Vector3d Scale;
};
} // namespace Edvar::Math
