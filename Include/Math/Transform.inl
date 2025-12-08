#pragma once
#include "Transform.hpp"
#include "Math/Math.hpp"

namespace Edvar::Math {

// ============================================================================
// Constructors
// ============================================================================

inline Transform::Transform() : Translation(Vector3d::Zero), Rotation(Quaternion::Identity), Scale(Vector3d::One) {}

inline Transform::Transform(const Matrix4x4d& matrix) { SetFromMatrix(matrix); }

inline Transform::Transform(const Matrix4x4f& matrix) {
    // Convert Matrix4x4f to Matrix4x4d
    Matrix4x4d matD;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matD(i, j) = static_cast<double>(matrix(i, j));
        }
    }
    SetFromMatrix(matD);
}

inline Transform::Transform(const Vector3d& translation, const Quaternion& rotation, const Vector3d& scale)
    : Translation(translation), Rotation(rotation), Scale(scale) {}

inline Transform::Transform(const Vector3f& translation, const Quaternion& rotation, const Vector3f& scale)
    : Translation(Vector3d(translation.X, translation.Y, translation.Z)), Rotation(rotation),
      Scale(Vector3d(scale.X, scale.Y, scale.Z)) {}

// ============================================================================
// Static factory methods
// ============================================================================

inline Transform Transform::FromMatrix(const Matrix4x4d& matrix) {
    Transform result = Transform(Vector3d::Zero, Quaternion::Identity, Vector3d::One);
    result.SetFromMatrix(matrix);
    return result;
}

inline Transform Transform::FromAxisAngle(const Vector3d& translation, const Vector3d& axis, double angle,
                                          const Vector3d& scale) {
    Vector3f axisF(static_cast<float>(axis.X), static_cast<float>(axis.Y), static_cast<float>(axis.Z));
    Quaternion rotation = Quaternion::FromAxisAngle(axisF, static_cast<float>(angle));
    return Transform(translation, rotation, scale);
}

inline Transform Transform::FromEuler(const Vector3d& translation, const Vector3d& eulerAngles, const Vector3d& scale) {
    Vector3f eulerF(static_cast<float>(eulerAngles.X), static_cast<float>(eulerAngles.Y),
                    static_cast<float>(eulerAngles.Z));
    Quaternion rotation = Quaternion::FromEuler(eulerF);
    return Transform(translation, rotation, scale);
}

inline Transform Transform::LookAt(const Vector3d& from, const Vector3d& to, const Vector3d& up) {
    Vector3f forwardF(static_cast<float>(to.X - from.X), static_cast<float>(to.Y - from.Y),
                      static_cast<float>(to.Z - from.Z));
    Vector3f upF(static_cast<float>(up.X), static_cast<float>(up.Y), static_cast<float>(up.Z));
    Quaternion rotation = Quaternion::LookRotation(forwardF, upF);
    return Transform(from, rotation, Vector3d::One);
}

inline Transform Transform::TranslationOnly(const Vector3d& translation) {
    return Transform(translation, Quaternion::Identity, Vector3d::One);
}

inline Transform Transform::RotationOnly(const Quaternion& rotation) {
    return Transform(Vector3d::Zero, rotation, Vector3d::One);
}

inline Transform Transform::ScaleOnly(const Vector3d& scale) {
    return Transform(Vector3d::Zero, Quaternion::Identity, scale);
}

// ============================================================================
// Static constant
// ============================================================================

const Transform Transform::Identity = Transform(Vector3d::Zero, Quaternion::Identity, Vector3d::One);

// ============================================================================
// Matrix conversion
// ============================================================================

inline void Transform::SetFromMatrix(const Matrix4x4d& matrix) {
    // Extract translation
    Translation = matrix.GetTranslation();

    // Extract scale
    Vector3d scaleX(matrix(0, 0), matrix(1, 0), matrix(2, 0));
    Vector3d scaleY(matrix(0, 1), matrix(1, 1), matrix(2, 1));
    Vector3d scaleZ(matrix(0, 2), matrix(1, 2), matrix(2, 2));

    Scale.X = scaleX.Length();
    Scale.Y = scaleY.Length();
    Scale.Z = scaleZ.Length();

    // Handle negative scale (determinant check)
    double det = matrix.Determinant();
    if (det < 0.0) {
        Scale.X = -Scale.X;
    }

    // Extract rotation by removing scale
    Matrix3x3d rotMat;
    if (Scale.X > VERY_SMALL_NUMBER) {
        rotMat(0, 0) = matrix(0, 0) / Scale.X;
        rotMat(1, 0) = matrix(1, 0) / Scale.X;
        rotMat(2, 0) = matrix(2, 0) / Scale.X;
    }
    if (Scale.Y > VERY_SMALL_NUMBER) {
        rotMat(0, 1) = matrix(0, 1) / Scale.Y;
        rotMat(1, 1) = matrix(1, 1) / Scale.Y;
        rotMat(2, 1) = matrix(2, 1) / Scale.Y;
    }
    if (Scale.Z > VERY_SMALL_NUMBER) {
        rotMat(0, 2) = matrix(0, 2) / Scale.Z;
        rotMat(1, 2) = matrix(1, 2) / Scale.Z;
        rotMat(2, 2) = matrix(2, 2) / Scale.Z;
    }

    // Convert to float matrix for quaternion extraction
    Matrix3x3f rotMatF;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rotMatF(i, j) = static_cast<float>(rotMat(i, j));
        }
    }

    Rotation = Quaternion::FromRotationMatrix(rotMatF);
}

inline Matrix4x4d Transform::ToMatrix() const {
    // T * R * S
    Matrix4x4d scaleMat = Matrix4x4d::Scale(Scale);
    Matrix4x4f rotMatF = Rotation.ToRotationMatrix4x4();
    Matrix4x4d rotMat;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rotMat(i, j) = static_cast<double>(rotMatF(i, j));
        }
    }
    Matrix4x4d transMat = Matrix4x4d::Translation(Translation);

    return transMat * rotMat * scaleMat;
}

inline Matrix4x4f Transform::ToMatrix4x4f() const {
    Matrix4x4d matD = ToMatrix();
    Matrix4x4f matF;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matF(i, j) = static_cast<float>(matD(i, j));
        }
    }
    return matF;
}

inline Matrix4x4d Transform::ToMatrix4x4d() const { return ToMatrix(); }

inline Matrix3x3d Transform::ToMatrix3x3d() const {
    // Rotation * Scale (no translation in 3x3)
    Matrix3x3f rotMat = Rotation.ToRotationMatrix3x3();
    Matrix3x3d rotMatD;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rotMatD(i, j) = static_cast<double>(rotMat(i, j));
        }
    }
    // Create scale matrix
    Matrix3x3d scaleMat;
    scaleMat(0, 0) = Scale.X;
    scaleMat(0, 1) = 0.0;
    scaleMat(0, 2) = 0.0;
    scaleMat(1, 0) = 0.0;
    scaleMat(1, 1) = Scale.Y;
    scaleMat(1, 2) = 0.0;
    scaleMat(2, 0) = 0.0;
    scaleMat(2, 1) = 0.0;
    scaleMat(2, 2) = Scale.Z;
    return rotMatD * scaleMat;
}

inline void Transform::ToMatrix(Matrix4x4d& outMatrix) const { outMatrix = ToMatrix(); }

// ============================================================================
// Component getters/setters
// ============================================================================

inline void Transform::SetTranslation(const Vector3d& translation) { Translation = translation; }

inline void Transform::SetRotation(const Quaternion& rotation) { Rotation = rotation; }

inline void Transform::SetScale(const Vector3d& scale) { Scale = scale; }

inline Vector3d Transform::GetTranslation() const { return Translation; }

inline Quaternion Transform::GetRotation() const { return Rotation; }

inline Vector3d Transform::GetScale() const { return Scale; }

inline Transform Transform::WithTranslation(const Vector3d& newTranslation) const {
    return Transform(newTranslation, Rotation, Scale);
}

inline Transform Transform::WithRotation(const Quaternion& newRotation) const {
    return Transform(Translation, newRotation, Scale);
}

inline Transform Transform::WithScale(const Vector3d& newScale) const {
    return Transform(Translation, Rotation, newScale);
}

inline Transform Transform::WithUniformScale(double uniformScale) const {
    return Transform(Translation, Rotation, Vector3d(uniformScale, uniformScale, uniformScale));
}

// ============================================================================
// In-place transformations
// ============================================================================

inline void Transform::TranslateInPlace(const Vector3d& delta) { Translation += delta; }

inline void Transform::RotateInPlace(const Quaternion& delta) {
    Rotation = delta * Rotation;
    Rotation.Normalize();
}

inline void Transform::ScaleInPlace(const Vector3d& delta) {
    Scale = Vector3d(Scale.X * delta.X, Scale.Y * delta.Y, Scale.Z * delta.Z);
}

inline void Transform::SetIdentity() {
    Translation = Vector3d::Zero;
    Rotation = Quaternion::Identity;
    Scale = Vector3d::One;
}

inline void Transform::NormalizeRotation() { Rotation.Normalize(); }

// ============================================================================
// Immutable transformations
// ============================================================================

inline Transform Transform::ApplyTranslation(const Vector3d& delta) const {
    Transform result = *this;
    result.TranslateInPlace(delta);
    return result;
}

inline Transform Transform::ApplyRotation(const Quaternion& delta) const {
    Transform result = *this;
    result.RotateInPlace(delta);
    return result;
}

inline Transform Transform::ApplyScale(const Vector3d& delta) const {
    Transform result = *this;
    result.ScaleInPlace(delta);
    return result;
}

inline Transform Transform::Normalized() const {
    Transform result = *this;
    result.NormalizeRotation();
    return result;
}

inline Transform Transform::RemoveScale() const { return Transform(Translation, Rotation, Vector3d::One); }

// ============================================================================
// Operators
// ============================================================================

inline Transform& Transform::operator*=(const Transform& other) {
    *this = *this * other;
    return *this;
}

inline Transform Transform::operator*(const Transform& other) const {
    // Combine transforms: this * other (other is applied first, then this)
    Vector3f scaledTransF(static_cast<float>(other.Translation.X * Scale.X),
                          static_cast<float>(other.Translation.Y * Scale.Y),
                          static_cast<float>(other.Translation.Z * Scale.Z));
    Vector3f rotatedF = Rotation.RotateVector(scaledTransF);
    Vector3d newTranslation = Translation + Vector3d(static_cast<double>(rotatedF.X), static_cast<double>(rotatedF.Y),
                                                     static_cast<double>(rotatedF.Z));

    Quaternion newRotation = Rotation * other.Rotation;
    Vector3d newScale = Vector3d(Scale.X * other.Scale.X, Scale.Y * other.Scale.Y, Scale.Z * other.Scale.Z);

    return Transform(newTranslation, newRotation, newScale);
}

inline Transform Transform::Combine(const Transform& other) const { return *this * other; }

inline Transform& Transform::operator/=(const Transform& other) {
    *this = *this / other;
    return *this;
}

inline Transform Transform::operator/(const Transform& other) const {
    // Divide transform by other (apply inverse of other)
    return *this * other.Inverse();
}

inline Transform& Transform::operator+=(const Transform& other) {
    Translation += other.Translation;
    Rotation = (Rotation + other.Rotation).Normalized();
    Scale += other.Scale;
    return *this;
}

inline Transform Transform::operator+(const Transform& other) const {
    Transform result = *this;
    result += other;
    return result;
}

inline Transform& Transform::operator-=(const Transform& other) {
    Translation -= other.Translation;
    Rotation = (Rotation - other.Rotation).Normalized();
    Scale -= other.Scale;
    return *this;
}

inline Transform Transform::operator-(const Transform& other) const {
    Transform result = *this;
    result -= other;
    return result;
}

inline bool Transform::operator==(const Transform& other) const {
    return Translation == other.Translation && Rotation == other.Rotation && Scale == other.Scale;
}

inline bool Transform::operator!=(const Transform& other) const { return !(*this == other); }

// ============================================================================
// Inverse and relative transforms
// ============================================================================

inline Transform Transform::Inverse() const {
    Quaternion invRotation = Rotation.Inverse();
    Vector3d invScale(Abs(Scale.X) > VERY_SMALL_NUMBER ? 1.0 / Scale.X : 0.0,
                      Abs(Scale.Y) > VERY_SMALL_NUMBER ? 1.0 / Scale.Y : 0.0,
                      Abs(Scale.Z) > VERY_SMALL_NUMBER ? 1.0 / Scale.Z : 0.0);

    // Inverse translation: -(R^-1 * (S^-1 * T))
    Vector3d scaledTranslation(Translation.X * invScale.X, Translation.Y * invScale.Y, Translation.Z * invScale.Z);
    Vector3f scaledTransF(static_cast<float>(scaledTranslation.X), static_cast<float>(scaledTranslation.Y),
                          static_cast<float>(scaledTranslation.Z));
    Vector3f invTransF = invRotation.RotateVector(scaledTransF);
    Vector3d invTranslation(-static_cast<double>(invTransF.X), -static_cast<double>(invTransF.Y),
                            -static_cast<double>(invTransF.Z));

    return Transform(invTranslation, invRotation, invScale);
}

inline Transform Transform::GetRelativeTransform(const Transform& parent) const {
    // Get transform relative to parent: parent^-1 * this
    return parent.Inverse() * (*this);
}

inline Transform Transform::GetLocalTransform(const Transform& parent) const { return GetRelativeTransform(parent); }

inline Transform Transform::GetWorldTransform(const Transform& parent) const {
    // Get world transform from local: parent * this
    return parent * (*this);
}

inline Transform Transform::ComposeTransforms(const Transform& parent, const Transform& local) {
    return parent * local;
}

// ============================================================================
// Point/Vector/Rotation transformation
// ============================================================================

inline Vector3d Transform::TransformPoint(const Vector3d& point) const {
    // Apply scale, rotation, then translation
    Vector3d scaled(point.X * Scale.X, point.Y * Scale.Y, point.Z * Scale.Z);
    Vector3f scaledF(static_cast<float>(scaled.X), static_cast<float>(scaled.Y), static_cast<float>(scaled.Z));
    Vector3f rotatedF = Rotation.RotateVector(scaledF);
    Vector3d rotated(static_cast<double>(rotatedF.X), static_cast<double>(rotatedF.Y), static_cast<double>(rotatedF.Z));
    return Translation + rotated;
}

inline Vector3d Transform::TransformDirection(const Vector3d& direction) const {
    // Apply rotation only (no translation or scale)
    Vector3f dirF(static_cast<float>(direction.X), static_cast<float>(direction.Y), static_cast<float>(direction.Z));
    Vector3f rotatedF = Rotation.RotateVector(dirF);
    return Vector3d(static_cast<double>(rotatedF.X), static_cast<double>(rotatedF.Y), static_cast<double>(rotatedF.Z));
}

inline Vector3d Transform::TransformScale(const Vector3d& scale) const {
    // Apply scale component
    return Vector3d(scale.X * Scale.X, scale.Y * Scale.Y, scale.Z * Scale.Z);
}

inline Quaternion Transform::TransformRotation(const Quaternion& rotation) const {
    // Apply rotation component
    return Rotation * rotation;
}

inline Vector3d Transform::InverseTransformPoint(const Vector3d& point) const {
    // Reverse: remove translation, then rotation, then scale
    Vector3d translated = point - Translation;
    Vector3f translatedF(static_cast<float>(translated.X), static_cast<float>(translated.Y),
                         static_cast<float>(translated.Z));
    Vector3f rotatedF = Rotation.UnrotateVector(translatedF);
    Vector3d rotated(static_cast<double>(rotatedF.X), static_cast<double>(rotatedF.Y), static_cast<double>(rotatedF.Z));

    return Vector3d(Abs(Scale.X) > VERY_SMALL_NUMBER ? rotated.X / Scale.X : rotated.X,
                    Abs(Scale.Y) > VERY_SMALL_NUMBER ? rotated.Y / Scale.Y : rotated.Y,
                    Abs(Scale.Z) > VERY_SMALL_NUMBER ? rotated.Z / Scale.Z : rotated.Z);
}

inline Vector3d Transform::InverseTransformDirection(const Vector3d& direction) const {
    // Reverse rotation only
    Vector3f dirF(static_cast<float>(direction.X), static_cast<float>(direction.Y), static_cast<float>(direction.Z));
    Vector3f rotatedF = Rotation.UnrotateVector(dirF);
    return Vector3d(static_cast<double>(rotatedF.X), static_cast<double>(rotatedF.Y), static_cast<double>(rotatedF.Z));
}

// ============================================================================
// Coordinate space transformations
// ============================================================================

inline Vector3d Transform::TransformPointToWorld(const Vector3d& localPoint) const {
    return TransformPoint(localPoint);
}

inline Vector3d Transform::TransformPointToLocal(const Vector3d& worldPoint) const {
    return InverseTransformPoint(worldPoint);
}

inline Vector3d Transform::TransformVectorToWorld(const Vector3d& localVector) const {
    return TransformDirection(localVector);
}

inline Vector3d Transform::TransformVectorToLocal(const Vector3d& worldVector) const {
    return InverseTransformDirection(worldVector);
}

inline Quaternion Transform::TransformRotationToWorld(const Quaternion& localRotation) const {
    return TransformRotation(localRotation);
}

inline Quaternion Transform::TransformRotationToLocal(const Quaternion& worldRotation) const {
    return Rotation.Inverse() * worldRotation;
}

// ============================================================================
// Interpolation
// ============================================================================

inline Transform Transform::Lerp(const Transform& a, const Transform& b, double alpha) {
    Vector3d translation = a.Translation + (b.Translation - a.Translation) * alpha;
    Quaternion rotation = Quaternion::Nlerp(a.Rotation, b.Rotation, static_cast<float>(alpha));
    Vector3d scale = a.Scale + (b.Scale - a.Scale) * alpha;
    return Transform(translation, rotation, scale);
}

inline Transform Transform::Slerp(const Transform& a, const Transform& b, double alpha) {
    Vector3d translation = a.Translation + (b.Translation - a.Translation) * alpha;
    Quaternion rotation = Quaternion::Slerp(a.Rotation, b.Rotation, static_cast<float>(alpha));
    Vector3d scale = a.Scale + (b.Scale - a.Scale) * alpha;
    return Transform(translation, rotation, scale);
}

inline Transform Transform::LerpTo(const Transform& target, double alpha) const { return Lerp(*this, target, alpha); }

inline Transform Transform::SlerpTo(const Transform& target, double alpha) const { return Slerp(*this, target, alpha); }

// ============================================================================
// Validation and comparison
// ============================================================================

inline bool Transform::IsNearlyEqual(const Transform& other, double tolerance) const {
    return Translation.IsNearlyEqual(other.Translation, tolerance) &&
           Rotation.AngleTo(other.Rotation) <= static_cast<float>(tolerance) &&
           Scale.IsNearlyEqual(other.Scale, tolerance);
}

inline bool Transform::IsIdentity(double tolerance) const {
    return Translation.IsNearlyEqual(Vector3d::Zero, tolerance) && Rotation.IsIdentity(static_cast<float>(tolerance)) &&
           Scale.IsNearlyEqual(Vector3d::One, tolerance);
}

inline bool Transform::IsValid() const {
    return !IsInfinite(Translation.X) && !IsInfinite(Translation.Y) && !IsInfinite(Translation.Z) &&
           !IsNaN(Translation.X) && !IsNaN(Translation.Y) && !IsNaN(Translation.Z) && Rotation.IsFinite() &&
           !IsInfinite(Scale.X) && !IsInfinite(Scale.Y) && !IsInfinite(Scale.Z) && !IsNaN(Scale.X) && !IsNaN(Scale.Y) &&
           !IsNaN(Scale.Z);
}

inline bool Transform::HasUniformScale(double tolerance) const {
    return Abs(Scale.X - Scale.Y) <= tolerance && Abs(Scale.Y - Scale.Z) <= tolerance;
}

inline bool Transform::HasNegativeScale() const { return Scale.X < 0.0 || Scale.Y < 0.0 || Scale.Z < 0.0; }

// ============================================================================
// Direction vectors
// ============================================================================

inline Vector3d Transform::GetForward() const {
    // Forward is +X in LH Z-up
    Vector3f forwardF = Rotation.GetForward();
    return Vector3d(static_cast<double>(forwardF.X), static_cast<double>(forwardF.Y), static_cast<double>(forwardF.Z));
}

inline Vector3d Transform::GetRight() const {
    // Right is +Y in LH Z-up
    Vector3f rightF = Rotation.GetRight();
    return Vector3d(static_cast<double>(rightF.X), static_cast<double>(rightF.Y), static_cast<double>(rightF.Z));
}

inline Vector3d Transform::GetUp() const {
    // Up is +Z in LH Z-up
    Vector3f upF = Rotation.GetUp();
    return Vector3d(static_cast<double>(upF.X), static_cast<double>(upF.Y), static_cast<double>(upF.Z));
}

inline Vector3d Transform::GetBack() const { return -GetForward(); }

inline Vector3d Transform::GetLeft() const { return -GetRight(); }

inline Vector3d Transform::GetDown() const { return -GetUp(); }

// ============================================================================
// Advanced transformation utilities
// ============================================================================

inline Transform Transform::MirrorX() const {
    Vector3d newScale(-Scale.X, Scale.Y, Scale.Z);
    return Transform(Translation, Rotation, newScale);
}

inline Transform Transform::MirrorY() const {
    Vector3d newScale(Scale.X, -Scale.Y, Scale.Z);
    return Transform(Translation, Rotation, newScale);
}

inline Transform Transform::MirrorZ() const {
    Vector3d newScale(Scale.X, Scale.Y, -Scale.Z);
    return Transform(Translation, Rotation, newScale);
}

inline Transform Transform::Mirror(const Vector3d& planeNormal) const {
    // Mirror across a plane defined by its normal
    Vector3d normal = planeNormal.Normalized();

    // Mirror translation
    double dot = Translation.Dot(normal);
    Vector3d mirroredTranslation = Translation - normal * (2.0 * dot);

    // Mirror rotation (reflect across plane)
    Vector3f normalF(static_cast<float>(normal.X), static_cast<float>(normal.Y), static_cast<float>(normal.Z));
    Vector3f forward = Rotation.GetForward();
    Vector3f mirroredForward = forward - normalF * (2.0f * forward.Dot(normalF));

    Vector3f up = Rotation.GetUp();
    Vector3f mirroredUp = up - normalF * (2.0f * up.Dot(normalF));

    Quaternion mirroredRotation = Quaternion::LookRotation(mirroredForward, mirroredUp);

    return Transform(mirroredTranslation, mirroredRotation, Scale);
}

inline Transform Transform::RotateAround(const Vector3d& point, const Quaternion& rotation) const {
    // Rotate transform around a point
    Vector3d offset = Translation - point;
    Vector3f offsetF(static_cast<float>(offset.X), static_cast<float>(offset.Y), static_cast<float>(offset.Z));
    Vector3f rotatedOffsetF = rotation.RotateVector(offsetF);
    Vector3d rotatedOffset(static_cast<double>(rotatedOffsetF.X), static_cast<double>(rotatedOffsetF.Y),
                           static_cast<double>(rotatedOffsetF.Z));
    Vector3d newTranslation = point + rotatedOffset;
    Quaternion newRotation = rotation * Rotation;

    return Transform(newTranslation, newRotation, Scale);
}

inline Transform Transform::RotateAround(const Vector3d& point, const Vector3d& axis, double angle) const {
    Vector3f axisF(static_cast<float>(axis.X), static_cast<float>(axis.Y), static_cast<float>(axis.Z));
    Quaternion rotation = Quaternion::FromAxisAngle(axisF, static_cast<float>(angle));
    return RotateAround(point, rotation);
}

inline Transform Transform::ScaleAround(const Vector3d& pivot, const Vector3d& scale) const {
    // Scale transform around a pivot point
    Vector3d offset = Translation - pivot;
    Vector3d scaledOffset(offset.X * scale.X, offset.Y * scale.Y, offset.Z * scale.Z);
    Vector3d newTranslation = pivot + scaledOffset;
    Vector3d newScale(Scale.X * scale.X, Scale.Y * scale.Y, Scale.Z * scale.Z);

    return Transform(newTranslation, Rotation, newScale);
}

inline Transform Transform::LookAtDirection(const Vector3d& direction, const Vector3d& up) {
    Vector3f dirF(static_cast<float>(direction.X), static_cast<float>(direction.Y), static_cast<float>(direction.Z));
    Vector3f upF(static_cast<float>(up.X), static_cast<float>(up.Y), static_cast<float>(up.Z));
    Rotation = Quaternion::LookRotation(dirF, upF);
    return *this;
}

inline Vector3d Transform::GetScaleSign() const {
    return Vector3d(Scale.X >= 0.0 ? 1.0 : -1.0, Scale.Y >= 0.0 ? 1.0 : -1.0, Scale.Z >= 0.0 ? 1.0 : -1.0);
}

// ============================================================================
// Debugging
// ============================================================================

inline Containers::String Transform::ToString() const {
    return Containers::String::PrintF(
        u"Transform(T: (%.3f, %.3f, %.3f), R: (%.3f, %.3f, %.3f, %.3f), S: (%.3f, %.3f, %.3f))", Translation.X,
        Translation.Y, Translation.Z, Rotation.X, Rotation.Y, Rotation.Z, Rotation.W, Scale.X, Scale.Y, Scale.Z);
}

inline Containers::String Transform::ToStringCompact() const {
    return Containers::String::PrintF(u"T[%.2f,%.2f,%.2f] R[%.2f,%.2f,%.2f,%.2f] S[%.2f,%.2f,%.2f]", Translation.X,
                                      Translation.Y, Translation.Z, Rotation.X, Rotation.Y, Rotation.Z, Rotation.W,
                                      Scale.X, Scale.Y, Scale.Z);
}

} // namespace Edvar::Math
