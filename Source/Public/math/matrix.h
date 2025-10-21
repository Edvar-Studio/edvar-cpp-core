#pragma once
#include "math/simd_support.h"

namespace edvar::math {

template <typename simd_type, uint32 rows, uint32 cols> struct matrix {
    using value_type = typename simd::meta::simd_traits<simd_type>::element_type;
    static constexpr uint32 simd_width = simd::meta::simd_traits<simd_type>::element_count;
    static_assert(simd::meta::simd_traits<simd_type>::is_simd);

    static_assert(simd_width >= rows || simd_width >= cols,
                  "At least one dimension must fit in a single SIMD register");
    static constexpr bool is_row_major = (simd_width >= cols);
    static constexpr size_t simd_rows = is_row_major ? (rows + simd_width - 1) / simd_width : rows;
    static constexpr size_t simd_cols = is_row_major ? cols : (cols + simd_width - 1) / simd_width;

    union {
        simd_type simd_data[simd_rows][simd_cols];
        value_type data[rows][cols];
        value_type flat_data[rows * cols];
    };

    matrix() = default;
    explicit matrix(value_type diagonal) {
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                data[r][c] = (r == c) ? diagonal : value_type(0);
            }
        }
    }
    explicit matrix(const value_type* values) {
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                data[r][c] = values[r * cols + c];
            }
        }
    }
    matrix(const matrix& other) : simd_data(other.simd_data) {}
    matrix(matrix&& other) noexcept : simd_data(edvar::move(other.simd_data)) {}
    matrix(const simd_type& other) : simd_data(other) {}

    matrix& operator=(const matrix& other) {
        if (this != &other) {
            simd_data = other.simd_data;
        }
        return *this;
    }
    matrix& operator=(matrix&& other) noexcept {
        if (this != &other) {
            simd_data = edvar::move(other.simd_data);
        }
        return *this;
    }

    value_type& operator()(const uint32 row, const uint32 col) { return data[row][col]; }
    const value_type& operator()(const uint32 row, const uint32 col) const { return data[row][col]; }

    template <typename new_type> matrix<new_type, rows, cols> convert_to() const {
        matrix<new_type, rows, cols> result;
        for (uint32 i_simd_rows = 0; i_simd_rows < simd_rows; ++i_simd_rows) {
            for (uint32 i_simd_cols = 0; i_simd_cols < simd_cols; ++i_simd_cols) {
                result.simd_data[i_simd_rows][i_simd_cols] =
                    simd_data[i_simd_rows][i_simd_cols].template convert_to<new_type>();
            }
        }
        return result;
    }
    // matrix to matrix operations
    template <typename other_type, uint32 other_rows, uint32 other_cols>
    matrix<simd_type, rows, other_cols>& operator*=(const matrix<other_type, other_rows, other_cols>& other) {
        static_assert(cols == other_rows, "Incompatible matrix dimensions for multiplication");
        if constexpr (std::is_same_v<simd_type, other_type>) {
            matrix<other_type, other_rows, other_cols> transposed_other = other.transposed();
            simd_type::matrix_multiply<rows, cols, other_rows, other_cols>(simd_data, transposed_other.simd_data,
                                                                           simd_data);
        } else {
            matrix<simd_type, other_rows, other_cols> converted_other = other.template convert_to<simd_type>();
            matrix<simd_type, other_rows, other_cols> transposed_other = converted_other.transposed();
            simd_type::matrix_multiply<rows, cols, other_rows, other_cols>(simd_data, transposed_other.simd_data,
                                                                           simd_data);
        }
        return *this;
    }

    template <typename other_type, uint32 other_rows, uint32 other_cols>
    matrix<simd_type, rows, other_cols> operator*(const matrix<other_type, other_rows, other_cols>& other) const {
        return matrix<simd_type, rows, other_cols>(*this) *= other;
    }
    template <typename other_type> matrix& operator+=(const matrix<other_type, rows, cols>& other) {
        if constexpr (std::is_same_v<simd_type, other_type>) {
            for (uint32 r = 0; r < simd_rows; ++r) {
                for (uint32 c = 0; c < simd_cols; ++c) {
                    simd_data[r][c] += other.simd_data[r][c];
                }
            }
        } else {
            matrix<simd_type, rows, cols> converted_other = other.template convert_to<simd_type>();
            for (uint32 r = 0; r < simd_rows; ++r) {
                for (uint32 c = 0; c < simd_cols; ++c) {
                    simd_data[r][c] += converted_other.simd_data[r][c];
                }
            }
        }
        return *this;
    }
    template <typename other_type> matrix operator+(const matrix<other_type, rows, cols>& other) const {
        return matrix(*this) += other;
    }

    template <typename other_type> matrix operator-=(const matrix<other_type, rows, cols>& other) {
        if constexpr (std::is_same_v<simd_type, other_type>) {
            for (uint32 r = 0; r < simd_rows; ++r) {
                for (uint32 c = 0; c < simd_cols; ++c) {
                    simd_data[r][c] -= other.simd_data[r][c];
                }
            }
        } else {
            matrix<simd_type, rows, cols> converted_other = other.template convert_to<simd_type>();
            for (uint32 r = 0; r < simd_rows; ++r) {
                for (uint32 c = 0; c < simd_cols; ++c) {
                    simd_data[r][c] -= converted_other.simd_data[r][c];
                }
            }
        }
        return *this;
    }
    template <typename other_type> matrix operator-(const matrix<other_type, rows, cols>& other) const {
        return matrix(*this) -= other;
    }

    // scalar operations
    matrix& operator+=(const value_type scalar) {
        simd_type simd_scalar(scalar);
        for (uint32 r = 0; r < simd_rows; ++r) {
            for (uint32 c = 0; c < simd_cols; ++c) {
                simd_data[r][c] += simd_scalar;
            }
        }
        return *this;
    }
    matrix operator+(const value_type scalar) const { return matrix(*this) += scalar; }
    matrix& operator-=(const value_type scalar) {
        simd_type simd_scalar(scalar);
        for (uint32 r = 0; r < simd_rows; ++r) {
            for (uint32 c = 0; c < simd_cols; ++c) {
                simd_data[r][c] -= simd_scalar;
            }
        }
        return *this;
    }
    matrix operator-(const value_type scalar) const { return matrix(*this) -= scalar; }
    matrix& operator*=(const value_type scalar) {
        simd_type simd_scalar(scalar);
        for (uint32 r = 0; r < simd_rows; ++r) {
            for (uint32 c = 0; c < simd_cols; ++c) {
                simd_data[r][c] *= simd_scalar;
            }
        }
        return *this;
    }
    matrix operator*(const value_type scalar) const { return matrix(*this) *= scalar; }
    matrix& operator/=(const value_type scalar) {
        simd_type simd_scalar(scalar);
        for (uint32 r = 0; r < simd_rows; ++r) {
            for (uint32 c = 0; c < simd_cols; ++c) {
                simd_data[r][c] /= simd_scalar;
            }
        }
        return *this;
    }
    matrix operator/(const value_type scalar) const { return matrix(*this) /= scalar; }

    matrix transposed() const {
        matrix result = *this;
        result.transpose_inline();
        return result;
    }

    matrix& transpose_inline() {
        for (uint32 r = 0; r < rows; ++r) {
            for (uint32 c = r + 1; c < cols; ++c) {
                edvar::swap(data[r][c], data[c][r]);
            }
        }
        return *this;
    }

    matrix& invert_inline() {}
};

using matrix4f = matrix<simd::simd_128f, 4, 4>;
using matrix4d = matrix<simd::simd_256d, 4, 4>;

} // namespace edvar::math