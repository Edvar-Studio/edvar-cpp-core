#pragma once
#include "Math/Math.hpp"
#include "Matrix.hpp"

namespace Edvar::Math {

// ============================================================================
// Matrix4x4 Implementation
// ============================================================================

// ========================================================================
// Constructors
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>::Matrix4x4() {
    // Uninitialized for performance
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>::Matrix4x4(T diagonal) {
    M[0][0] = diagonal;
    M[0][1] = 0;
    M[0][2] = 0;
    M[0][3] = 0;
    M[1][0] = 0;
    M[1][1] = diagonal;
    M[1][2] = 0;
    M[1][3] = 0;
    M[2][0] = 0;
    M[2][1] = 0;
    M[2][2] = diagonal;
    M[2][3] = 0;
    M[3][0] = 0;
    M[3][1] = 0;
    M[3][2] = 0;
    M[3][3] = diagonal;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>::Matrix4x4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30,
                        T m31, T m32, T m33) {
    M[0][0] = m00;
    M[0][1] = m01;
    M[0][2] = m02;
    M[0][3] = m03;
    M[1][0] = m10;
    M[1][1] = m11;
    M[1][2] = m12;
    M[1][3] = m13;
    M[2][0] = m20;
    M[2][1] = m21;
    M[2][2] = m22;
    M[2][3] = m23;
    M[3][0] = m30;
    M[3][1] = m31;
    M[3][2] = m32;
    M[3][3] = m33;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>::Matrix4x4(const T elements[16]) {
    for (int i = 0; i < 16; ++i) {
        Elements[i] = elements[i];
    }
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>::Matrix4x4(const SIMDType& row0, const SIMDType& row1, const SIMDType& row2, const SIMDType& row3) {
    Rows[0] = row0;
    Rows[1] = row1;
    Rows[2] = row2;
    Rows[3] = row3;
}

// ========================================================================
// Static constants
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Matrix4x4<T> Matrix4x4<T>::Identity = Matrix4x4<T>(static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Matrix4x4<T> Matrix4x4<T>::Zero = Matrix4x4<T>(static_cast<T>(0));

// ========================================================================
// Element access
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
T& Matrix4x4<T>::operator()(int row, int col) {
    return M[row][col];
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const T& Matrix4x4<T>::operator()(int row, int col) const {
    return M[row][col];
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector4<T> Matrix4x4<T>::GetRow(int index) const {
    return Vector4<T>(M[index][0], M[index][1], M[index][2], M[index][3]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector4<T> Matrix4x4<T>::GetColumn(int index) const {
    return Vector4<T>(M[0][index], M[1][index], M[2][index], M[3][index]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
void Matrix4x4<T>::SetRow(int index, const Vector4<T>& row) {
    M[index][0] = row.X;
    M[index][1] = row.Y;
    M[index][2] = row.Z;
    M[index][3] = row.W;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
void Matrix4x4<T>::SetColumn(int index, const Vector4<T>& column) {
    M[0][index] = column.X;
    M[1][index] = column.Y;
    M[2][index] = column.Z;
    M[3][index] = column.W;
}

// ========================================================================
// Matrix operations
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& other) const {
    Matrix4x4<T> result;

    // SIMD::MatrixMultiply4x4 expects the second matrix to be transposed
    // We handle the transpose internally for a cleaner API
    Matrix4x4<T> otherTransposed = other.Transpose();

    SIMDType::MatrixMultiply4x4(Rows, otherTransposed.Rows, result.Rows);

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector4<T> Matrix4x4<T>::operator*(const Vector4<T>& vec) const {
    // Transform vector as column vector: result = matrix * vec
    Vector4<T> result;
    result.X = M[0][0] * vec.X + M[0][1] * vec.Y + M[0][2] * vec.Z + M[0][3] * vec.W;
    result.Y = M[1][0] * vec.X + M[1][1] * vec.Y + M[1][2] * vec.Z + M[1][3] * vec.W;
    result.Z = M[2][0] * vec.X + M[2][1] * vec.Y + M[2][2] * vec.Z + M[2][3] * vec.W;
    result.W = M[3][0] * vec.X + M[3][1] * vec.Y + M[3][2] * vec.Z + M[3][3] * vec.W;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& other) const {
    Matrix4x4<T> result;
    result.Rows[0] = Rows[0] + other.Rows[0];
    result.Rows[1] = Rows[1] + other.Rows[1];
    result.Rows[2] = Rows[2] + other.Rows[2];
    result.Rows[3] = Rows[3] + other.Rows[3];
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& other) const {
    Matrix4x4<T> result;
    result.Rows[0] = Rows[0] - other.Rows[0];
    result.Rows[1] = Rows[1] - other.Rows[1];
    result.Rows[2] = Rows[2] - other.Rows[2];
    result.Rows[3] = Rows[3] - other.Rows[3];
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::operator*(T scalar) const {
    Matrix4x4<T> result;
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    result.Rows[0] = Rows[0] * scalarVec;
    result.Rows[1] = Rows[1] * scalarVec;
    result.Rows[2] = Rows[2] * scalarVec;
    result.Rows[3] = Rows[3] * scalarVec;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::operator/(T scalar) const {
    Matrix4x4<T> result;
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    result.Rows[0] = Rows[0] / scalarVec;
    result.Rows[1] = Rows[1] / scalarVec;
    result.Rows[2] = Rows[2] / scalarVec;
    result.Rows[3] = Rows[3] / scalarVec;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& other) {
    Rows[0] = Rows[0] + other.Rows[0];
    Rows[1] = Rows[1] + other.Rows[1];
    Rows[2] = Rows[2] + other.Rows[2];
    Rows[3] = Rows[3] + other.Rows[3];
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& other) {
    Rows[0] = Rows[0] - other.Rows[0];
    Rows[1] = Rows[1] - other.Rows[1];
    Rows[2] = Rows[2] - other.Rows[2];
    Rows[3] = Rows[3] - other.Rows[3];
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& other) {
    *this = *this * other;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>& Matrix4x4<T>::operator*=(T scalar) {
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    Rows[0] = Rows[0] * scalarVec;
    Rows[1] = Rows[1] * scalarVec;
    Rows[2] = Rows[2] * scalarVec;
    Rows[3] = Rows[3] * scalarVec;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T>& Matrix4x4<T>::operator/=(T scalar) {
    SIMDType scalarVec(scalar, scalar, scalar, scalar);
    Rows[0] = Rows[0] / scalarVec;
    Rows[1] = Rows[1] / scalarVec;
    Rows[2] = Rows[2] / scalarVec;
    Rows[3] = Rows[3] / scalarVec;
    return *this;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
bool Matrix4x4<T>::operator==(const Matrix4x4<T>& other) const {
    for (int i = 0; i < 16; ++i) {
        if (!IsNearlyEqual(Elements[i], other.Elements[i])) {
            return false;
        }
    }
    return true;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& other) const {
    return !(*this == other);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Transpose() const {
    Matrix4x4<T> result;

    // Transpose using SIMD shuffles for better performance
    // For now, use scalar path (can be optimized later with SIMD shuffles)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.M[i][j] = M[j][i];
        }
    }

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
T Matrix4x4<T>::Determinant() const {
    // Calculate 4x4 determinant using cofactor expansion
    T a00 = M[0][0], a01 = M[0][1], a02 = M[0][2], a03 = M[0][3];
    T a10 = M[1][0], a11 = M[1][1], a12 = M[1][2], a13 = M[1][3];
    T a20 = M[2][0], a21 = M[2][1], a22 = M[2][2], a23 = M[2][3];
    T a30 = M[3][0], a31 = M[3][1], a32 = M[3][2], a33 = M[3][3];

    // Calculate 2x2 sub-determinants
    T det0 = a22 * a33 - a23 * a32;
    T det1 = a21 * a33 - a23 * a31;
    T det2 = a21 * a32 - a22 * a31;
    T det3 = a20 * a33 - a23 * a30;
    T det4 = a20 * a32 - a22 * a30;
    T det5 = a20 * a31 - a21 * a30;

    // Calculate 3x3 cofactors
    T cof0 = a11 * det0 - a12 * det1 + a13 * det2;
    T cof1 = a10 * det0 - a12 * det3 + a13 * det4;
    T cof2 = a10 * det1 - a11 * det3 + a13 * det5;
    T cof3 = a10 * det2 - a11 * det4 + a12 * det5;

    // Calculate 4x4 determinant
    return a00 * cof0 - a01 * cof1 + a02 * cof2 - a03 * cof3;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Inverse() const {
    T det = Determinant();

    if (Abs(det) < static_cast<T>(VERY_SMALL_NUMBER)) {
        // Matrix is singular, return identity
        return Identity;
    }

    // Calculate inverse using adjugate method
    Matrix4x4<T> result;

    T a00 = M[0][0], a01 = M[0][1], a02 = M[0][2], a03 = M[0][3];
    T a10 = M[1][0], a11 = M[1][1], a12 = M[1][2], a13 = M[1][3];
    T a20 = M[2][0], a21 = M[2][1], a22 = M[2][2], a23 = M[2][3];
    T a30 = M[3][0], a31 = M[3][1], a32 = M[3][2], a33 = M[3][3];

    // Calculate cofactor matrix (transposed)
    result.M[0][0] = (a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31));
    result.M[0][1] = -(a01 * (a22 * a33 - a23 * a32) - a02 * (a21 * a33 - a23 * a31) + a03 * (a21 * a32 - a22 * a31));
    result.M[0][2] = (a01 * (a12 * a33 - a13 * a32) - a02 * (a11 * a33 - a13 * a31) + a03 * (a11 * a32 - a12 * a31));
    result.M[0][3] = -(a01 * (a12 * a23 - a13 * a22) - a02 * (a11 * a23 - a13 * a21) + a03 * (a11 * a22 - a12 * a21));

    result.M[1][0] = -(a10 * (a22 * a33 - a23 * a32) - a12 * (a20 * a33 - a23 * a30) + a13 * (a20 * a32 - a22 * a30));
    result.M[1][1] = (a00 * (a22 * a33 - a23 * a32) - a02 * (a20 * a33 - a23 * a30) + a03 * (a20 * a32 - a22 * a30));
    result.M[1][2] = -(a00 * (a12 * a33 - a13 * a32) - a02 * (a10 * a33 - a13 * a30) + a03 * (a10 * a32 - a12 * a30));
    result.M[1][3] = (a00 * (a12 * a23 - a13 * a22) - a02 * (a10 * a23 - a13 * a20) + a03 * (a10 * a22 - a12 * a20));

    result.M[2][0] = (a10 * (a21 * a33 - a23 * a31) - a11 * (a20 * a33 - a23 * a30) + a13 * (a20 * a31 - a21 * a30));
    result.M[2][1] = -(a00 * (a21 * a33 - a23 * a31) - a01 * (a20 * a33 - a23 * a30) + a03 * (a20 * a31 - a21 * a30));
    result.M[2][2] = (a00 * (a11 * a33 - a13 * a31) - a01 * (a10 * a33 - a13 * a30) + a03 * (a10 * a31 - a11 * a30));
    result.M[2][3] = -(a00 * (a11 * a23 - a13 * a21) - a01 * (a10 * a23 - a13 * a20) + a03 * (a10 * a21 - a11 * a20));

    result.M[3][0] = -(a10 * (a21 * a32 - a22 * a31) - a11 * (a20 * a32 - a22 * a30) + a12 * (a20 * a31 - a21 * a30));
    result.M[3][1] = (a00 * (a21 * a32 - a22 * a31) - a01 * (a20 * a32 - a22 * a30) + a02 * (a20 * a31 - a21 * a30));
    result.M[3][2] = -(a00 * (a11 * a32 - a12 * a31) - a01 * (a10 * a32 - a12 * a30) + a02 * (a10 * a31 - a11 * a30));
    result.M[3][3] = (a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20) + a02 * (a10 * a21 - a11 * a20));

    // Divide by determinant
    T invDet = static_cast<T>(1) / det;
    result *= invDet;

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
bool Matrix4x4<T>::IsInvertible(T epsilon) const {
    return Abs(Determinant()) > epsilon;
}

// ========================================================================
// Transformation factories
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Translation(const Vector3<T>& translation) {
    Matrix4x4<T> result = Identity;
    result.M[0][3] = translation.X;
    result.M[1][3] = translation.Y;
    result.M[2][3] = translation.Z;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::RotationX(T angleRadians) {
    T c = static_cast<T>(Cos(static_cast<double>(angleRadians)));
    T s = static_cast<T>(Sin(static_cast<double>(angleRadians)));

    Matrix4x4<T> result = Identity;
    result.M[1][1] = c;
    result.M[1][2] = -s;
    result.M[2][1] = s;
    result.M[2][2] = c;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::RotationY(T angleRadians) {
    T c = static_cast<T>(Cos(static_cast<double>(angleRadians)));
    T s = static_cast<T>(Sin(static_cast<double>(angleRadians)));

    Matrix4x4<T> result = Identity;
    result.M[0][0] = c;
    result.M[0][2] = s;
    result.M[2][0] = -s;
    result.M[2][2] = c;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::RotationZ(T angleRadians) {
    T c = static_cast<T>(Cos(static_cast<double>(angleRadians)));
    T s = static_cast<T>(Sin(static_cast<double>(angleRadians)));

    Matrix4x4<T> result = Identity;
    result.M[0][0] = c;
    result.M[0][1] = -s;
    result.M[1][0] = s;
    result.M[1][1] = c;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::RotationAxis(const Vector3<T>& axis, T angleRadians) {
    // Rodrigues' rotation formula
    Vector3<T> normAxis = axis.Normalized();
    T c = static_cast<T>(Cos(static_cast<double>(angleRadians)));
    T s = static_cast<T>(Sin(static_cast<double>(angleRadians)));
    T t = static_cast<T>(1) - c;

    T x = normAxis.X;
    T y = normAxis.Y;
    T z = normAxis.Z;

    Matrix4x4<T> result;
    result.M[0][0] = t * x * x + c;
    result.M[0][1] = t * x * y - s * z;
    result.M[0][2] = t * x * z + s * y;
    result.M[0][3] = 0;

    result.M[1][0] = t * x * y + s * z;
    result.M[1][1] = t * y * y + c;
    result.M[1][2] = t * y * z - s * x;
    result.M[1][3] = 0;

    result.M[2][0] = t * x * z - s * y;
    result.M[2][1] = t * y * z + s * x;
    result.M[2][2] = t * z * z + c;
    result.M[2][3] = 0;

    result.M[3][0] = 0;
    result.M[3][1] = 0;
    result.M[3][2] = 0;
    result.M[3][3] = 1;

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::RotationQuaternion(const Quaternion& quat) {
    // Convert quaternion to 4x4 rotation matrix
    Matrix4x4<float> rotMatF = quat.ToRotationMatrix4x4();
    
    // Convert to target type if needed
    if constexpr (std::is_same_v<T, float>) {
        return rotMatF;
    } else {
        Matrix4x4<T> result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.M[i][j] = static_cast<T>(rotMatF.M[i][j]);
            }
        }
        return result;
    }
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Scale(const Vector3<T>& scale) {
    Matrix4x4<T> result = Identity;
    result.M[0][0] = scale.X;
    result.M[1][1] = scale.Y;
    result.M[2][2] = scale.Z;
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Scale(T uniformScale) {
    return Scale(Vector3<T>(uniformScale, uniformScale, uniformScale));
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::TRS(const Vector3<T>& translation, const Quaternion& rotation, const Vector3<T>& scale) {
    // Compose transformation matrix: T * R * S
    Matrix4x4<T> scaleMat = Scale(scale);
    Matrix4x4<T> rotMat = RotationQuaternion(rotation);
    Matrix4x4<T> transMat = Translation(translation);
    
    return transMat * rotMat * scaleMat;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up) {
    // Left-Handed Z-Up coordinate system
    // Forward: +X, Right: +Y, Up: +Z

    Vector3<T> forward = (target - eye).Normalized();  // +X direction
    Vector3<T> right = forward.Cross(up).Normalized(); // +Y direction
    Vector3<T> newUp = right.Cross(forward);           // +Z direction (recalculated)

    Matrix4x4<T> result;

    // Rotation part (basis vectors in rows for view matrix)
    result.M[0][0] = right.X;
    result.M[0][1] = right.Y;
    result.M[0][2] = right.Z;
    result.M[0][3] = -right.Dot(eye);

    result.M[1][0] = forward.X;
    result.M[1][1] = forward.Y;
    result.M[1][2] = forward.Z;
    result.M[1][3] = -forward.Dot(eye);

    result.M[2][0] = newUp.X;
    result.M[2][1] = newUp.Y;
    result.M[2][2] = newUp.Z;
    result.M[2][3] = -newUp.Dot(eye);

    result.M[3][0] = 0;
    result.M[3][1] = 0;
    result.M[3][2] = 0;
    result.M[3][3] = 1;

    return result;
}

// ========================================================================
// Projection factories
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::PerspectiveFov(T fovYRadians, T aspectRatio, T nearZ, T farZ) {
    // Left-handed perspective projection with [0,1] depth range (D3D12 native)
    T tanHalfFovy = static_cast<T>(Math::Tan(static_cast<double>(fovYRadians) / 2.0));

    Matrix4x4<T> result(static_cast<T>(0));

    result.M[0][0] = static_cast<T>(1) / (aspectRatio * tanHalfFovy);
    result.M[1][1] = static_cast<T>(1) / tanHalfFovy;
    result.M[2][2] = farZ / (farZ - nearZ);
    result.M[2][3] = -(farZ * nearZ) / (farZ - nearZ);
    result.M[3][2] = static_cast<T>(1);
    result.M[3][3] = static_cast<T>(0);

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::PerspectiveFov_ReverseZ(T fovYRadians, T aspectRatio, T nearZ, T farZ) {
    // Reverse-Z for improved depth precision
    T tanHalfFovy = static_cast<T>(Math::Tan(static_cast<double>(fovYRadians) / 2.0));

    Matrix4x4<T> result(static_cast<T>(0));

    result.M[0][0] = static_cast<T>(1) / (aspectRatio * tanHalfFovy);
    result.M[1][1] = static_cast<T>(1) / tanHalfFovy;
    result.M[2][2] = nearZ / (nearZ - farZ);
    result.M[2][3] = (farZ * nearZ) / (nearZ - farZ);
    result.M[3][2] = static_cast<T>(1);
    result.M[3][3] = static_cast<T>(0);

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Orthographic(T width, T height, T nearZ, T farZ) {
    // Left-handed orthographic projection with [0,1] depth range
    Matrix4x4<T> result(static_cast<T>(0));

    result.M[0][0] = static_cast<T>(2) / width;
    result.M[1][1] = static_cast<T>(2) / height;
    result.M[2][2] = static_cast<T>(1) / (farZ - nearZ);
    result.M[2][3] = -nearZ / (farZ - nearZ);
    result.M[3][3] = static_cast<T>(1);

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::OrthographicOffCenter(T left, T right, T bottom, T top, T nearZ, T farZ) {
    // Left-handed orthographic projection with off-center bounds
    Matrix4x4<T> result(static_cast<T>(0));

    result.M[0][0] = static_cast<T>(2) / (right - left);
    result.M[0][3] = -(right + left) / (right - left);

    result.M[1][1] = static_cast<T>(2) / (top - bottom);
    result.M[1][3] = -(top + bottom) / (top - bottom);

    result.M[2][2] = static_cast<T>(1) / (farZ - nearZ);
    result.M[2][3] = -nearZ / (farZ - nearZ);

    result.M[3][3] = static_cast<T>(1);

    return result;
}

// ========================================================================
// Decomposition and utilities
// ========================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
bool Matrix4x4<T>::Decompose(Vector3<T>& outTranslation, Quaternion& outRotation, Vector3<T>& outScale) const {
    // Extract translation
    outTranslation = GetTranslation();
    
    // Extract scale from basis vectors
    Vector3<T> scaleX(M[0][0], M[1][0], M[2][0]);
    Vector3<T> scaleY(M[0][1], M[1][1], M[2][1]);
    Vector3<T> scaleZ(M[0][2], M[1][2], M[2][2]);
    
    outScale.X = scaleX.Length();
    outScale.Y = scaleY.Length();
    outScale.Z = scaleZ.Length();
    
    // Check for invalid scale
    if (outScale.X < static_cast<T>(VERY_SMALL_NUMBER) ||
        outScale.Y < static_cast<T>(VERY_SMALL_NUMBER) ||
        outScale.Z < static_cast<T>(VERY_SMALL_NUMBER)) {
        return false;
    }
    
    // Handle negative scale (determinant check)
    T det = Determinant();
    if (det < static_cast<T>(0)) {
        outScale.X = -outScale.X;
    }
    
    // Extract rotation by removing scale
    Matrix3x3<float> rotMat;
    rotMat(0, 0) = static_cast<float>(M[0][0] / outScale.X);
    rotMat(1, 0) = static_cast<float>(M[1][0] / outScale.X);
    rotMat(2, 0) = static_cast<float>(M[2][0] / outScale.X);
    
    rotMat(0, 1) = static_cast<float>(M[0][1] / outScale.Y);
    rotMat(1, 1) = static_cast<float>(M[1][1] / outScale.Y);
    rotMat(2, 1) = static_cast<float>(M[2][1] / outScale.Y);
    
    rotMat(0, 2) = static_cast<float>(M[0][2] / outScale.Z);
    rotMat(1, 2) = static_cast<float>(M[1][2] / outScale.Z);
    rotMat(2, 2) = static_cast<float>(M[2][2] / outScale.Z);
    
    // Convert rotation matrix to quaternion
    outRotation = Quaternion::FromRotationMatrix(rotMat);
    
    return true;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::GetTranslation() const {
    return Vector3<T>(M[0][3], M[1][3], M[2][3]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
void Matrix4x4<T>::SetTranslation(const Vector3<T>& translation) {
    M[0][3] = translation.X;
    M[1][3] = translation.Y;
    M[2][3] = translation.Z;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::ExtractScale() const {
    Vector3<T> scaleX(M[0][0], M[1][0], M[2][0]);
    Vector3<T> scaleY(M[0][1], M[1][1], M[2][1]);
    Vector3<T> scaleZ(M[0][2], M[1][2], M[2][2]);

    return Vector3<T>(scaleX.Length(), scaleY.Length(), scaleZ.Length());
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::RemoveScale() const {
    Vector3<T> scale = ExtractScale();

    if (scale.X == 0 || scale.Y == 0 || scale.Z == 0) {
        return *this;
    }

    Matrix4x4<T> result = *this;

    // Normalize basis vectors
    result.M[0][0] /= scale.X;
    result.M[1][0] /= scale.X;
    result.M[2][0] /= scale.X;
    result.M[0][1] /= scale.Y;
    result.M[1][1] /= scale.Y;
    result.M[2][1] /= scale.Y;
    result.M[0][2] /= scale.Z;
    result.M[1][2] /= scale.Z;
    result.M[2][2] /= scale.Z;

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::GetAxisX() const {
    return Vector3<T>(M[0][0], M[1][0], M[2][0]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::GetAxisY() const {
    return Vector3<T>(M[0][1], M[1][1], M[2][1]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::GetAxisZ() const {
    return Vector3<T>(M[0][2], M[1][2], M[2][2]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::TransformPoint(const Vector3<T>& point) const {
    // Transform as point (w = 1)
    T x = M[0][0] * point.X + M[0][1] * point.Y + M[0][2] * point.Z + M[0][3];
    T y = M[1][0] * point.X + M[1][1] * point.Y + M[1][2] * point.Z + M[1][3];
    T z = M[2][0] * point.X + M[2][1] * point.Y + M[2][2] * point.Z + M[2][3];
    return Vector3<T>(x, y, z);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix4x4<T>::TransformVector(const Vector3<T>& vector) const {
    // Transform as direction (w = 0, no translation)
    T x = M[0][0] * vector.X + M[0][1] * vector.Y + M[0][2] * vector.Z;
    T y = M[1][0] * vector.X + M[1][1] * vector.Y + M[1][2] * vector.Z;
    T z = M[2][0] * vector.X + M[2][1] * vector.Y + M[2][2] * vector.Z;
    return Vector3<T>(x, y, z);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix4x4<T>::Lerp(const Matrix4x4<T>& a, const Matrix4x4<T>& b, T alpha) {
    Matrix4x4<T> result;
    for (int i = 0; i < 16; ++i) {
        result.Elements[i] = a.Elements[i] + (b.Elements[i] - a.Elements[i]) * alpha;
    }
    return result;
}

// Scalar multiplication from left
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> operator*(T scalar, const Matrix4x4<T>& mat) {
    return mat * scalar;
}

// ============================================================================
// Matrix3x3 Implementation
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T>::Matrix3x3() {
    // Uninitialized
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T>::Matrix3x3(T diagonal) {
    M[0][0] = diagonal;
    M[0][1] = 0;
    M[0][2] = 0;
    M[0][3] = 0;
    M[1][0] = 0;
    M[1][1] = diagonal;
    M[1][2] = 0;
    M[1][3] = 0;
    M[2][0] = 0;
    M[2][1] = 0;
    M[2][2] = diagonal;
    M[2][3] = 0;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T>::Matrix3x3(const SIMDType& row0, const SIMDType& row1, const SIMDType& row2) {
    Rows[0] = row0;
    Rows[1] = row1;
    Rows[2] = row2;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Matrix3x3<T> Matrix3x3<T>::Identity = Matrix3x3<T>(static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const Matrix3x3<T> Matrix3x3<T>::Zero = Matrix3x3<T>(static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
T& Matrix3x3<T>::operator()(int row, int col) {
    return M[row][col];
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
const T& Matrix3x3<T>::operator()(int row, int col) const {
    return M[row][col];
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& other) const {
    Matrix3x3<T> result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.M[i][j] = M[i][0] * other.M[0][j] + M[i][1] * other.M[1][j] + M[i][2] * other.M[2][j];
        }
    }
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Vector3<T> Matrix3x3<T>::operator*(const Vector3<T>& vec) const {
    return Vector3<T>(M[0][0] * vec.X + M[0][1] * vec.Y + M[0][2] * vec.Z,
                      M[1][0] * vec.X + M[1][1] * vec.Y + M[1][2] * vec.Z,
                      M[2][0] * vec.X + M[2][1] * vec.Y + M[2][2] * vec.Z);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T> Matrix3x3<T>::Transpose() const {
    Matrix3x3<T> result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.M[i][j] = M[j][i];
        }
    }
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
T Matrix3x3<T>::Determinant() const {
    return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]) +
           M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T> Matrix3x3<T>::Inverse() const {
    T det = Determinant();
    if (Abs(det) < static_cast<T>(VERY_SMALL_NUMBER)) {
        return Identity;
    }

    T invDet = static_cast<T>(1) / det;
    Matrix3x3<T> result;

    result.M[0][0] = (M[1][1] * M[2][2] - M[1][2] * M[2][1]) * invDet;
    result.M[0][1] = -(M[0][1] * M[2][2] - M[0][2] * M[2][1]) * invDet;
    result.M[0][2] = (M[0][1] * M[1][2] - M[0][2] * M[1][1]) * invDet;

    result.M[1][0] = -(M[1][0] * M[2][2] - M[1][2] * M[2][0]) * invDet;
    result.M[1][1] = (M[0][0] * M[2][2] - M[0][2] * M[2][0]) * invDet;
    result.M[1][2] = -(M[0][0] * M[1][2] - M[0][2] * M[1][0]) * invDet;

    result.M[2][0] = (M[1][0] * M[2][1] - M[1][1] * M[2][0]) * invDet;
    result.M[2][1] = -(M[0][0] * M[2][1] - M[0][1] * M[2][0]) * invDet;
    result.M[2][2] = (M[0][0] * M[1][1] - M[0][1] * M[1][0]) * invDet;

    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> Matrix3x3<T>::ToMatrix4x4() const {
    Matrix4x4<T> result = Matrix4x4<T>::Identity;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.M[i][j] = M[i][j];
        }
    }
    return result;
}

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix3x3<T> Matrix3x3<T>::FromMatrix4x4(const Matrix4x4<T>& mat) {
    Matrix3x3<T> result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.M[i][j] = mat.M[i][j];
        }
        result.M[i][3] = 0; // Clear padding
    }
    return result;
}

// ============================================================================
// Matrix2x2 Implementation
// ============================================================================

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T>::Matrix2x2() {
    // Uninitialized
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T>::Matrix2x2(T diagonal) {
    M[0][0] = diagonal;
    M[0][1] = 0;
    M[1][0] = 0;
    M[1][1] = diagonal;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T>::Matrix2x2(T m00, T m01, T m10, T m11) {
    M[0][0] = m00;
    M[0][1] = m01;
    M[1][0] = m10;
    M[1][1] = m11;
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Matrix2x2<T> Matrix2x2<T>::Identity = Matrix2x2<T>(static_cast<T>(1));

template <typename T>
    requires(std::is_arithmetic_v<T>)
const Matrix2x2<T> Matrix2x2<T>::Zero = Matrix2x2<T>(static_cast<T>(0));

template <typename T>
    requires(std::is_arithmetic_v<T>)
T& Matrix2x2<T>::operator()(int row, int col) {
    return M[row][col];
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
const T& Matrix2x2<T>::operator()(int row, int col) const {
    return M[row][col];
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T> Matrix2x2<T>::operator*(const Matrix2x2<T>& other) const {
    return Matrix2x2<T>(
        M[0][0] * other.M[0][0] + M[0][1] * other.M[1][0], M[0][0] * other.M[0][1] + M[0][1] * other.M[1][1],
        M[1][0] * other.M[0][0] + M[1][1] * other.M[1][0], M[1][0] * other.M[0][1] + M[1][1] * other.M[1][1]);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Vector2<T> Matrix2x2<T>::operator*(const Vector2<T>& vec) const {
    return Vector2<T>(M[0][0] * vec.X + M[0][1] * vec.Y, M[1][0] * vec.X + M[1][1] * vec.Y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T> Matrix2x2<T>::Transpose() const {
    return Matrix2x2<T>(M[0][0], M[1][0], M[0][1], M[1][1]);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
T Matrix2x2<T>::Determinant() const {
    return M[0][0] * M[1][1] - M[0][1] * M[1][0];
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T> Matrix2x2<T>::Inverse() const {
    T det = Determinant();
    if (Abs(det) < static_cast<T>(VERY_SMALL_NUMBER)) {
        return Identity;
    }

    T invDet = static_cast<T>(1) / det;
    return Matrix2x2<T>(M[1][1] * invDet, -M[0][1] * invDet, -M[1][0] * invDet, M[0][0] * invDet);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T> Matrix2x2<T>::Rotation(T angleRadians) {
    T c = static_cast<T>(Cos(static_cast<double>(angleRadians)));
    T s = static_cast<T>(Sin(static_cast<double>(angleRadians)));
    return Matrix2x2<T>(c, -s, s, c);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T> Matrix2x2<T>::Scale(T x, T y) {
    return Matrix2x2<T>(x, 0, 0, y);
}

template <typename T>
    requires(std::is_arithmetic_v<T>)
Matrix2x2<T> Matrix2x2<T>::Scale(T uniform) {
    return Scale(uniform, uniform);
}

} // namespace Edvar::Math
