#pragma once
#include "Math.hpp"
#include "Vector.hpp"

namespace Edvar::Math {

// Forward declarations
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Matrix4x4;

template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct Matrix3x3;

template <typename T>
    requires(std::is_arithmetic_v<T>)
struct Matrix2x2;
// End forward declarations

// ============================================================================
// Matrix4x4 - 4x4 Matrix with SIMD optimization
// ============================================================================

/**
 * @brief 4x4 matrix with SIMD optimization for float, double, and int32_t types.
 * Storage is row-major: M[row][col].
 * Vector multiplication follows industry standard: result = matrix * columnVector.
 * Coordinate system: Left-Handed Z-Up (Forward: +X, Right: +Y, Up: +Z).
 */
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct alignas(std::is_same_v<T, double> ? 32 : 16) Matrix4x4 {
    using SIMDType = SIMD::VectorDataType<T>;

    // ========================================================================
    // Data storage
    // ========================================================================
    union {
        SIMDType Rows[4];  // 4 SIMD vectors representing rows
        T M[4][4];         // Row-major: M[row][col]
        T Elements[16];    // Linear array for iteration
    };

    // ========================================================================
    // Constructors
    // ========================================================================
    
    /** Default constructor - uninitialized */
    Matrix4x4();

    /** Construct diagonal matrix with given value */
    explicit Matrix4x4(T diagonal);

    /** Construct from 16 elements in row-major order */
    explicit Matrix4x4(T m00, T m01, T m02, T m03,
                       T m10, T m11, T m12, T m13,
                       T m20, T m21, T m22, T m23,
                       T m30, T m31, T m32, T m33);

    /** Construct from array of 16 elements in row-major order */
    explicit Matrix4x4(const T elements[16]);

    /** Construct from 4 SIMD row vectors */
    explicit Matrix4x4(const SIMDType& row0, const SIMDType& row1, 
                       const SIMDType& row2, const SIMDType& row3);

    // ========================================================================
    // Static constants
    // ========================================================================
    
    /** Identity matrix */
    static const Matrix4x4<T> Identity;
    
    /** Zero matrix */
    static const Matrix4x4<T> Zero;

    // ========================================================================
    // Element access
    // ========================================================================
    
    /** Access element at (row, col) */
    T& operator()(int row, int col);
    
    /** Access element at (row, col) - const version */
    const T& operator()(int row, int col) const;

    /** Get row as Vector4 */
    Vector4<T> GetRow(int index) const;
    
    /** Get column as Vector4 */
    Vector4<T> GetColumn(int index) const;

    /** Set row from Vector4 */
    void SetRow(int index, const Vector4<T>& row);
    
    /** Set column from Vector4 */
    void SetColumn(int index, const Vector4<T>& column);

    // ========================================================================
    // Matrix operations
    // ========================================================================
    
    /** Matrix multiplication (handles transpose internally for SIMD) */
    Matrix4x4<T> operator*(const Matrix4x4<T>& other) const;
    
    /** Transform Vector4 (treats as column vector) */
    Vector4<T> operator*(const Vector4<T>& vec) const;

    /** Matrix addition */
    Matrix4x4<T> operator+(const Matrix4x4<T>& other) const;
    
    /** Matrix subtraction */
    Matrix4x4<T> operator-(const Matrix4x4<T>& other) const;

    /** Scalar multiplication */
    Matrix4x4<T> operator*(T scalar) const;
    
    /** Scalar division */
    Matrix4x4<T> operator/(T scalar) const;

    /** Compound assignment operators */
    Matrix4x4<T>& operator+=(const Matrix4x4<T>& other);
    Matrix4x4<T>& operator-=(const Matrix4x4<T>& other);
    Matrix4x4<T>& operator*=(const Matrix4x4<T>& other);
    Matrix4x4<T>& operator*=(T scalar);
    Matrix4x4<T>& operator/=(T scalar);

    /** Equality comparison (with epsilon tolerance) */
    bool operator==(const Matrix4x4<T>& other) const;
    
    /** Inequality comparison */
    bool operator!=(const Matrix4x4<T>& other) const;

    /** Transpose matrix */
    Matrix4x4<T> Transpose() const;
    
    /** Calculate determinant */
    T Determinant() const;
    
    /** Calculate inverse matrix */
    Matrix4x4<T> Inverse() const;
    
    /** Check if matrix is invertible */
    bool IsInvertible(T epsilon = static_cast<T>(SMALL_NUMBER)) const;

    // ========================================================================
    // Transformation factories (Left-Handed Z-Up)
    // ========================================================================
    
    /** Create translation matrix */
    static Matrix4x4<T> Translation(const Vector3<T>& translation);
    
    /** Create rotation matrix around X-axis (radians) */
    static Matrix4x4<T> RotationX(T angleRadians);
    
    /** Create rotation matrix around Y-axis (radians) */
    static Matrix4x4<T> RotationY(T angleRadians);
    
    /** Create rotation matrix around Z-axis (radians) */
    static Matrix4x4<T> RotationZ(T angleRadians);
    
    /** Create rotation matrix around arbitrary axis (radians) using Rodrigues' formula */
    static Matrix4x4<T> RotationAxis(const Vector3<T>& axis, T angleRadians);
    
    /** Create rotation matrix from Quaternion (stubbed for now) */
    static Matrix4x4<T> RotationQuaternion(const Quaternion& quat);
    
    /** Create non-uniform scale matrix */
    static Matrix4x4<T> Scale(const Vector3<T>& scale);
    
    /** Create uniform scale matrix */
    static Matrix4x4<T> Scale(T uniformScale);
    
    /** Create combined Translation-Rotation-Scale matrix */
    static Matrix4x4<T> TRS(const Vector3<T>& translation, const Quaternion& rotation, const Vector3<T>& scale);
    
    /** Create look-at view matrix (Left-Handed Z-Up) */
    static Matrix4x4<T> LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up);

    // ========================================================================
    // Projection factories (Left-Handed, [0,1] depth - D3D12 native)
    // ========================================================================
    
    /** Create perspective projection matrix with field of view */
    static Matrix4x4<T> PerspectiveFov(T fovYRadians, T aspectRatio, T nearZ, T farZ);
    
    /** Create perspective projection matrix with reverse Z for improved precision */
    static Matrix4x4<T> PerspectiveFov_ReverseZ(T fovYRadians, T aspectRatio, T nearZ, T farZ);
    
    /** Create orthographic projection matrix */
    static Matrix4x4<T> Orthographic(T width, T height, T nearZ, T farZ);
    
    /** Create orthographic projection matrix with off-center bounds */
    static Matrix4x4<T> OrthographicOffCenter(T left, T right, T bottom, T top, T nearZ, T farZ);

    // ========================================================================
    // Decomposition and utilities
    // ========================================================================
    
    /** Decompose matrix into translation, rotation, and scale (stubbed - requires Quaternion) */
    bool Decompose(Vector3<T>& outTranslation, Quaternion& outRotation, Vector3<T>& outScale) const;
    
    /** Extract translation component */
    Vector3<T> GetTranslation() const;
    
    /** Set translation component */
    void SetTranslation(const Vector3<T>& translation);
    
    /** Extract scale from basis vectors */
    Vector3<T> ExtractScale() const;
    
    /** Remove scale from matrix (normalize basis vectors) */
    Matrix4x4<T> RemoveScale() const;
    
    /** Get X-axis (right vector) */
    Vector3<T> GetAxisX() const;
    
    /** Get Y-axis (forward vector in LH Z-up) */
    Vector3<T> GetAxisY() const;
    
    /** Get Z-axis (up vector) */
    Vector3<T> GetAxisZ() const;

    /** Transform point (w = 1) */
    Vector3<T> TransformPoint(const Vector3<T>& point) const;
    
    /** Transform vector/direction (w = 0) */
    Vector3<T> TransformVector(const Vector3<T>& vector) const;
    
    /** Linear interpolation between two matrices */
    static Matrix4x4<T> Lerp(const Matrix4x4<T>& a, const Matrix4x4<T>& b, T alpha);
};

// Scalar multiplication from left side
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
Matrix4x4<T> operator*(T scalar, const Matrix4x4<T>& mat);

// Type aliases
using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4d = Matrix4x4<double>;
using Matrix4x4i = Matrix4x4<int32_t>;

// ============================================================================
// Matrix3x3 - 3x3 Matrix with SIMD optimization
// ============================================================================

/**
 * @brief 3x3 matrix with SIMD optimization (uses padding for alignment).
 * Primarily used for rotations and 3D transformations without translation.
 */
template <typename T>
    requires(std::is_arithmetic_v<T> && SIMD::IsSIMDTypeSupported<T>)
struct alignas(std::is_same_v<T, double> ? 32 : 16) Matrix3x3 {
    using SIMDType = SIMD::VectorDataType<T>;

    // Storage: 3 SIMD vectors with W component as padding
    union {
        SIMDType Rows[3];  // 3 SIMD vectors (W unused)
        T M[3][4];         // 3x4 for alignment (4th column unused)
    };

    // Constructors
    Matrix3x3();
    explicit Matrix3x3(T diagonal);
    explicit Matrix3x3(const SIMDType& row0, const SIMDType& row1, const SIMDType& row2);

    // Static constants
    static const Matrix3x3<T> Identity;
    static const Matrix3x3<T> Zero;

    // Element access
    T& operator()(int row, int col);
    const T& operator()(int row, int col) const;

    // Operations
    Matrix3x3<T> operator*(const Matrix3x3<T>& other) const;
    Vector3<T> operator*(const Vector3<T>& vec) const;
    Matrix3x3<T> Transpose() const;
    T Determinant() const;
    Matrix3x3<T> Inverse() const;

    // Conversion
    Matrix4x4<T> ToMatrix4x4() const;
    static Matrix3x3<T> FromMatrix4x4(const Matrix4x4<T>& mat);
};

// Type aliases
using Matrix3x3f = Matrix3x3<float>;
using Matrix3x3d = Matrix3x3<double>;
using Matrix3x3i = Matrix3x3<int32_t>;

// ============================================================================
// Matrix2x2 - 2x2 Matrix for 2D transformations
// ============================================================================

/**
 * @brief 2x2 matrix for 2D transformations (scalar implementation).
 */
template <typename T>
    requires(std::is_arithmetic_v<T>)
struct Matrix2x2 {
    T M[2][2];  // Row-major storage

    // Constructors
    Matrix2x2();
    explicit Matrix2x2(T diagonal);
    explicit Matrix2x2(T m00, T m01, T m10, T m11);

    // Static constants
    static const Matrix2x2<T> Identity;
    static const Matrix2x2<T> Zero;

    // Element access
    T& operator()(int row, int col);
    const T& operator()(int row, int col) const;

    // Operations
    Matrix2x2<T> operator*(const Matrix2x2<T>& other) const;
    Vector2<T> operator*(const Vector2<T>& vec) const;
    Matrix2x2<T> Transpose() const;
    T Determinant() const;
    Matrix2x2<T> Inverse() const;

    // 2D transformations
    static Matrix2x2<T> Rotation(T angleRadians);
    static Matrix2x2<T> Scale(T x, T y);
    static Matrix2x2<T> Scale(T uniform);
};

// Type aliases
using Matrix2x2f = Matrix2x2<float>;
using Matrix2x2d = Matrix2x2<double>;

} // namespace Edvar::Math
