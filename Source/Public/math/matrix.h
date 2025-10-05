#pragma once
#include "math/simd_support.h"

namespace edvar::math {
struct vector3d;
struct quaternion;

struct matrix4x4d {
    matrix4x4d();
    matrix4x4d(const double all);
    matrix4x4d(const double& r0_c0, const double& r0_c1, const double& r0_c2, const double& r0_c3, const double& r1_c0,
               const double& r1_c1, const double& r1_c2, const double& r1_c3, const double& r2_c0, const double& r2_c1,
               const double& r2_c2, const double& r2_c3, const double& r3_c0, const double& r3_c1, const double& r3_c2,
               const double& r3_c3);
    matrix4x4d(const vector3d& row0, const vector3d& row1, const vector3d& row2, const vector3d& row3);
    matrix4x4d(const std::initializer_list<double>& list);
    matrix4x4d(double const* const* to_place_data);
    matrix4x4d(double const* to_place_data);

    matrix4x4d(const matrix4x4d& other);
    matrix4x4d(matrix4x4d&& other) noexcept;
    matrix4x4d& operator=(const matrix4x4d& rhs);
    matrix4x4d& operator=(matrix4x4d&& rhs) noexcept;

    ~matrix4x4d();

    static matrix4x4d identity();
    static matrix4x4d zero();

    matrix4x4d operator+(const matrix4x4d& rhs) const;
    matrix4x4d& operator+=(const matrix4x4d& rhs);
    matrix4x4d operator-(const matrix4x4d& rhs) const;
    matrix4x4d& operator-=(const matrix4x4d& rhs);
    matrix4x4d operator*(const matrix4x4d& rhs) const;
    matrix4x4d& operator*=(const matrix4x4d& rhs);
    matrix4x4d operator*(const double rhs) const;
    matrix4x4d& operator*=(const double rhs);
    matrix4x4d operator*(const vector3d& rhs) const;
    matrix4x4d& operator*=(const vector3d& rhs);
    matrix4x4d operator/(const double rhs) const;
    matrix4x4d& operator/=(const double rhs);

    inline double& operator()(uint32 row, uint32 col) { return this->data[row][col]; }
    inline const double& operator()(uint32 row, uint32 col) const { return this->data[row][col]; }

    bool operator==(const matrix4x4d& rhs) const;
    inline bool operator!=(const matrix4x4d& rhs) const { return !(*this == rhs); }

    matrix4x4d transpose() const;
    matrix4x4d& transpose_inline();

    double determinant() const;

    matrix4x4d inverse() const;
    matrix4x4d& inverse_inline();

    static matrix4x4d create_view_matrix(const vector3d& position, const vector3d& forward, const vector3d& up);
    static matrix4x4d create_perspective_projection_matrix(const double fov_y, const double aspect_ratio,
                                                           const double near_plane, const double far_plane);
    static matrix4x4d create_orthographic_projection_matrix(const double left, const double right, const double bottom,
                                                            const double top, const double near_plane,
                                                            const double far_plane);
    static matrix4x4d create_inverse_view_matrix(const vector3d& position, const vector3d& forward, const vector3d& up);

private:
    union {
        // row-major 4x4 matrix stored as 4 rows of 2 simd_128d each
        simd::simd_128d simd_data[8];
        // row-major access
        double data[4][4];
    };
};
} // namespace edvar::math