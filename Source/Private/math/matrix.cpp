#pragma once

#include "math/matrix.h"
#include "math/vector.h"

namespace edvar::math {
matrix4x4d::matrix4x4d() {
    static constexpr const uint32 row_count = 4;
    static constexpr const uint32 col_count = 4;

    for (uint32 r = 0; r < row_count; ++r)
        for (uint32 c = 0; c < col_count; ++c)
            this->data[r][c] = (r == c) ? 1.0 : 0.0;
}

matrix4x4d::matrix4x4d(const double all) {
    static constexpr const uint32 row_count = 4;
    static constexpr const uint32 col_count = 4;

    for (uint32 r = 0; r < row_count; ++r)
        for (uint32 c = 0; c < col_count; ++c)
            this->data[r][c] = all;
}

matrix4x4d::matrix4x4d(const double& r0_c0, const double& r0_c1, const double& r0_c2, const double& r0_c3,
                       const double& r1_c0, const double& r1_c1, const double& r1_c2, const double& r1_c3,
                       const double& r2_c0, const double& r2_c1, const double& r2_c2, const double& r2_c3,
                       const double& r3_c0, const double& r3_c1, const double& r3_c2, const double& r3_c3) {
    this->data[0][0] = r0_c0;
    this->data[0][1] = r0_c1;
    this->data[0][2] = r0_c2;
    this->data[0][3] = r0_c3;

    this->data[1][0] = r1_c0;
    this->data[1][1] = r1_c1;
    this->data[1][2] = r1_c2;
    this->data[1][3] = r1_c3;

    this->data[2][0] = r2_c0;
    this->data[2][1] = r2_c1;
    this->data[2][2] = r2_c2;
    this->data[2][3] = r2_c3;

    this->data[3][0] = r3_c0;
    this->data[3][1] = r3_c1;
    this->data[3][2] = r3_c2;
    this->data[3][3] = r3_c3;
}

matrix4x4d::matrix4x4d(const vector3d& row0, const vector3d& row1, const vector3d& row2, const vector3d& row3) {
    this->data[0][0] = row0.x();
    this->data[0][1] = row0.y();
    this->data[0][2] = row0.z();
    this->data[0][3] = 0.0;

    this->data[1][0] = row1.x();
    this->data[1][1] = row1.y();
    this->data[1][2] = row1.z();
    this->data[1][3] = 0.0;

    this->data[2][0] = row2.x();
    this->data[2][1] = row2.y();
    this->data[2][2] = row2.z();
    this->data[2][3] = 0.0;

    this->data[3][0] = row3.x();
    this->data[3][1] = row3.y();
    this->data[3][2] = row3.z();
    this->data[3][3] = 1.0;
}
matrix4x4d::matrix4x4d(const std::initializer_list<double>& list) {
    static constexpr const uint32 row_count = 4;
    static constexpr const uint32 col_count = 4;

    memory::zero_bytes(this->data, sizeof(this->data));

    uint32 i = 0;
    for (auto it = list.begin(); it != list.end() && i < row_count * col_count; ++it, ++i) {
        const uint32 r = i / col_count;
        const uint32 c = i % col_count;
        this->data[r][c] = *it;
    }
}
matrix4x4d::matrix4x4d(double const* const* to_place_data) {
    static constexpr const uint32 row_count = 4;
    static constexpr const uint32 col_count = 4;

    for (uint32 r = 0; r < row_count; ++r)
        for (uint32 c = 0; c < col_count; ++c)
            this->data[r][c] = to_place_data[r][c];
}
matrix4x4d::matrix4x4d(double const* to_place_data) {
    static constexpr const uint32 row_count = 4;
    static constexpr const uint32 col_count = 4;

    for (uint32 r = 0; r < row_count; ++r)
        for (uint32 c = 0; c < col_count; ++c)
            this->data[r][c] = to_place_data[r * col_count + c];
}

matrix4x4d::~matrix4x4d() { memory::zero_bytes(this->data, sizeof(this->data)); }

matrix4x4d matrix4x4d::identity() { return matrix4x4d(); }

matrix4x4d matrix4x4d::zero() { return matrix4x4d(0.0); }

matrix4x4d matrix4x4d::operator+(const matrix4x4d& rhs) const {
    matrix4x4d self_copy = *this;
    self_copy += rhs;
    return self_copy;
}

matrix4x4d& matrix4x4d::operator+=(const matrix4x4d& rhs) {
    static constexpr const uint32 loop_count = 4 * 2; // 4 rows of 2 simd_128d each
    for (uint32 i = 0; i < loop_count; ++i)
        this->simd_data[i] += rhs.simd_data[i];
    return *this;
}
matrix4x4d matrix4x4d::operator-(const matrix4x4d& rhs) const {
    matrix4x4d self_copy = *this;
    self_copy -= rhs;
    return self_copy;
}
matrix4x4d& matrix4x4d::operator-=(const matrix4x4d& rhs) {
    static constexpr const uint32 loop_count = 4 * 2; // 4 rows of 2 simd_128d each
    for (uint32 i = 0; i < loop_count; ++i)
        this->simd_data[i] -= rhs.simd_data[i];
    return *this;
}
matrix4x4d matrix4x4d::operator*(const matrix4x4d& rhs) const {
    matrix4x4d result(*this);
    result *= rhs;
    return result;
}
matrix4x4d& matrix4x4d::operator*=(const matrix4x4d& rhs) {
    // Unwound SIMD-optimized matrix multiplication
    simd::simd_128d result_simd[8];

    // Row 0
    {
        simd::simd_128d lhs_row0_0(this->data[0][0], this->data[0][1]);
        simd::simd_128d lhs_row0_1(this->data[0][2], this->data[0][3]);
        simd::simd_128d rhs_col0_0(rhs.data[0][0], rhs.data[1][0]);
        simd::simd_128d rhs_col0_1(rhs.data[2][0], rhs.data[3][0]);
        simd::simd_128d rhs_col1_0(rhs.data[0][1], rhs.data[1][1]);
        simd::simd_128d rhs_col1_1(rhs.data[2][1], rhs.data[3][1]);
        simd::simd_128d rhs_col2_0(rhs.data[0][2], rhs.data[1][2]);
        simd::simd_128d rhs_col2_1(rhs.data[2][2], rhs.data[3][2]);
        simd::simd_128d rhs_col3_0(rhs.data[0][3], rhs.data[1][3]);
        simd::simd_128d rhs_col3_1(rhs.data[2][3], rhs.data[3][3]);

        result_simd[0] = (lhs_row0_0 * rhs_col0_0).horizontal_add(lhs_row0_1 * rhs_col0_1);
        result_simd[1] = (lhs_row0_0 * rhs_col1_0).horizontal_add(lhs_row0_1 * rhs_col1_1);
        result_simd[2] = (lhs_row0_0 * rhs_col2_0).horizontal_add(lhs_row0_1 * rhs_col2_1);
        result_simd[3] = (lhs_row0_0 * rhs_col3_0).horizontal_add(lhs_row0_1 * rhs_col3_1);
    }
    // Row 1
    {
        simd::simd_128d lhs_row1_0(this->data[1][0], this->data[1][1]);
        simd::simd_128d lhs_row1_1(this->data[1][2], this->data[1][3]);
        simd::simd_128d rhs_col0_0(rhs.data[0][0], rhs.data[1][0]);
        simd::simd_128d rhs_col0_1(rhs.data[2][0], rhs.data[3][0]);
        simd::simd_128d rhs_col1_0(rhs.data[0][1], rhs.data[1][1]);
        simd::simd_128d rhs_col1_1(rhs.data[2][1], rhs.data[3][1]);
        simd::simd_128d rhs_col2_0(rhs.data[0][2], rhs.data[1][2]);
        simd::simd_128d rhs_col2_1(rhs.data[2][2], rhs.data[3][2]);
        simd::simd_128d rhs_col3_0(rhs.data[0][3], rhs.data[1][3]);
        simd::simd_128d rhs_col3_1(rhs.data[2][3], rhs.data[3][3]);

        result_simd[4] = (lhs_row1_0 * rhs_col0_0).horizontal_add(lhs_row1_1 * rhs_col0_1);
        result_simd[5] = (lhs_row1_0 * rhs_col1_0).horizontal_add(lhs_row1_1 * rhs_col1_1);
        result_simd[6] = (lhs_row1_0 * rhs_col2_0).horizontal_add(lhs_row1_1 * rhs_col2_1);
        result_simd[7] = (lhs_row1_0 * rhs_col3_0).horizontal_add(lhs_row1_1 * rhs_col3_1);
    }
    // Copy results back
    for (uint32 i = 0; i < 8; ++i) {
        this->simd_data[i] = result_simd[i];
    }
    return *this;
}

} // namespace edvar::math