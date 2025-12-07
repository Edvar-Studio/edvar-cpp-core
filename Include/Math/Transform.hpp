#pragma once
#include "Matrix.hpp"
#include "Vector.hpp"
#include "Quaternion.hpp"

namespace Edvar::Math {
struct Transform {
    Transform();
    explicit Transform(const Matrix4x4d& matrix);
    explicit Transform(const Matrix4x4f& matrix);
    explicit Transform(const Vector3d& translation = Vector3d::Zero, const Quaternion& rotation = Quaternion::Identity,
                       const Vector3d& scale = Vector3d::One);

    void SetFromMatrix(const Matrix4x4d& matrix);

    Matrix4x4d ToMatrix() const;

    void SetTranslation(const Vector3d& translation);
    void SetRotation(const Quaternion& rotation);
    void SetScale(const Vector3d& scale);

    Vector3d GetTranslation() const;
    Quaternion GetRotation() const;
    Vector3d GetScale() const;

    void TranslateInPlace(const Vector3d& delta);
    void RotateInPlace(const Quaternion& delta);
    void ScaleInPlace(const Vector3d& delta);

    Transform ApplyTranslation(const Vector3d& delta) const;
    Transform ApplyRotation(const Quaternion& delta) const;
    Transform ApplyScale(const Vector3d& delta) const;

    Transform& operator*=(const Transform& other);
    Transform operator*(const Transform& other) const;
    Transform Combine(const Transform& other) const;

    Transform& operator/=(const Transform& other);
    Transform operator/(const Transform& other) const;

    Transform& operator+=(const Transform& other);
    Transform operator+(const Transform& other) const;

    Transform& operator-=(const Transform& other);
    Transform operator-(const Transform& other) const;

    Transform Inverse() const;

    Transform GetRelativeTransform(const Transform& parent) const;

    Vector3d TransformPoint(const Vector3d& point) const;
    Vector3d TransformScale(const Vector3d& scale) const;
    Quaternion TransformRotation(const Quaternion& rotation) const;

private:
    Vector3d Translation;
    Quaternion Rotation;
    Vector3d Scale;
};
} // namespace Edvar::Math