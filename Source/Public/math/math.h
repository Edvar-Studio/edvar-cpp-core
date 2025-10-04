#pragma once
#include "diagnostics/assertion.h"
#include "meta/meta.h"
#ifndef EDVAR_CPP_CORE_MATH_USE_INTRINSICS
#    define EDVAR_CPP_CORE_MATH_USE_INTRINSICS 1
#endif
#if EDVAR_CPP_CORE_MATH_USE_INTRINSICS
#    include <xmmintrin.h>
#endif
#ifndef EDVAR_CPP_CORE_CPU_SSE4_2
#    define EDVAR_CPP_CORE_CPU_SSE4_2 0
#endif
#ifndef EDVAR_CPP_CORE_CPU_SSE2
#    define EDVAR_CPP_CORE_CPU_SSE2 EDVAR_CPP_CORE_CPU_SSE4_2 // SSE4 implies SSE2
#endif
#ifndef EDVAR_CPP_CORE_CPU_AVX10_1
#    define EDVAR_CPP_CORE_CPU_AVX10_1 0
#endif
#ifndef EDVAR_CPP_CORE_CPU_AVX512
#    define EDVAR_CPP_CORE_CPU_AVX512 EDVAR_CPP_CORE_CPU_AVX10_1 // AVX10.1 implies AVX512
#endif
#ifndef EDVAR_CPP_CORE_CPU_AVX2
#    define EDVAR_CPP_CORE_CPU_AVX2 EDVAR_CPP_CORE_CPU_AVX512 // AVX512 implies AVX2
#endif
#ifndef EDVAR_CPP_CORE_CPU_AVX
#    define EDVAR_CPP_CORE_CPU_AVX EDVAR_CPP_CORE_CPU_AVX2 // AVX2 implies AVX
#endif

namespace edvar::math {

static constexpr double PI = 3.14159265358979323846;
static constexpr double TWO_PI = 2.0 * PI;
static constexpr double HALF_PI = PI / 2.0;
static constexpr double E = 2.71828182845904523536;
static constexpr double E_SQUARE = E * E;
static constexpr double GOLDEN_RATIO = 1.618033988749895;

static constexpr double SMALL_NUMBER = 1e-10;
static constexpr double VERY_SMALL_NUMBER = 1e-20;
static constexpr double LARGE_NUMBER = 1e+10;
static constexpr double VERY_LARGE_NUMBER = 1e+20;

inline auto abs(const auto& value) -> decltype(value) { return value < 0 ? -value : value; }
inline auto min(const auto& a, const auto& b) -> decltype(a) { return a < b ? a : b; }
inline auto max(const auto& a, const auto& b) -> decltype(a) { return a > b ? a : b; }
inline auto clamp(const auto& value, const auto& min_value, const auto& max_value) -> decltype(value) {
    return min(max(value, min_value), max_value);
}
inline auto square(const auto& value) -> decltype(value * value) { return value * value; }
inline auto cube(const auto& value) -> decltype(value * value * value) { return value * value * value; }
inline auto floor(const auto& value) -> decltype(value) {
    using return_type = decltype(value);
    if constexpr (meta::is_unsigned<meta::remove_cv<meta::remove_reference<return_type>>>) {
        return static_cast<return_type>(static_cast<uint64>(value));
    }
    return static_cast<decltype(value)>(static_cast<int64>(value));
}
inline auto ceil(const auto& value) -> decltype(value) {
    using return_type = decltype(value);
    if constexpr (meta::is_unsigned<meta::remove_cv<meta::remove_reference<return_type>>>) {
        return static_cast<return_type>(
            static_cast<uint64>(value + (value == static_cast<return_type>(static_cast<uint64>(value)) ? 0 : 1)));
    }
    return static_cast<decltype(value)>(
        static_cast<int64>(value + (value == static_cast<return_type>(static_cast<int64>(value)) ? 0 : 1)));
}

double ln(const double& value);
float ln(const float& value);
inline int ln(const int& value) { return static_cast<int>(math::ln(static_cast<double>(value))); }
inline int64 ln(const int64& value) { return static_cast<int64>(math::ln(static_cast<double>(value))); }

double log(const double& value, const double& base);
inline float log(const float& value, const float& base) {
    return static_cast<float>(math::log(static_cast<double>(value), static_cast<double>(base)));
}
int log(const int& value, const int& base);
int64 log(const int64& value, const int64& base);

double exp(const double& value, const double& tolerance = 1e-10);
inline float exp(const float& value) { return static_cast<float>(math::exp(static_cast<double>(value))); }

int power(const int& value, const int& exponent);
inline double power(const double& value, const double& exponent) { return math::exp(math::ln(value) * exponent); }
inline float power(const float& value, const float& exponent) {
    return static_cast<float>(math::power(static_cast<double>(value), static_cast<double>(exponent)));
}
inline double power(const double& value, const int& exponent) {
    if (exponent == 0) [[unlikely]] {
        return 1.0;
    }
    double result = value;
    for (int i = 1; i < exponent; ++i) {
        result *= value;
    }
    return result;
}

double square_root(const int& value, const double& tolerance = 1e-6);
double square_root(const double& value, const double& tolerance = 1e-6);
double square_root(const float& value, const double& tolerance = 1e-6f);

#if EDVAR_CPP_CORE_MATH_USE_INTRINSICS
template <size_t N, typename T> struct simd_vector_cluster {};
template <size_t N> struct simd_vector_cluster<N, double> {
    static constexpr size_t N2 = (N + 1) & ~size_t(1); // Ceil to nearest multiple of 2
    static_assert(N2 > 0, "N must be positive.");
    __m128d data[N2 / 2];

    simd_vector_cluster() {
        for (size_t i = 0; i < N2 / 2; ++i)
            data[i] = _mm_setzero_pd();
    }

    simd_vector_cluster(const double* values) {
        for (size_t i = 0; i < N2 / 2; ++i)
            data[i] = _mm_loadu_pd(&values[i * 2]);
    }

    double get(size_t idx) const {
        size_t vec_idx = idx / 2;
        size_t elem_idx = idx % 2;
        alignas(16) double vals[2];
        _mm_store_pd(vals, data[vec_idx]);
        return vals[elem_idx];
    }

    void set(size_t idx, double value) {
        size_t vec_idx = idx / 2;
        size_t elem_idx = idx % 2;
        alignas(16) double vals[2];
        _mm_store_pd(vals, data[vec_idx]);
        vals[elem_idx] = value;
        data[vec_idx] = _mm_load_pd(vals);
    }

    simd_vector_cluster operator+(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N2 / 2; ++i)
            result.data[i] = _mm_add_pd(data[i], other.data[i]);
        return result;
    }

    simd_vector_cluster operator-(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N2 / 2; ++i)
            result.data[i] = _mm_sub_pd(data[i], other.data[i]);
        return result;
    }

    simd_vector_cluster operator*(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N2 / 2; ++i)
            result.data[i] = _mm_mul_pd(data[i], other.data[i]);
        return result;
    }
    simd_vector_cluster& operator*=(const simd_vector_cluster& other) {
        for (size_t i = 0; i < N2 / 2; ++i)
            data[i] = _mm_mul_pd(data[i], other.data[i]);
        return *this;
    }

    simd_vector_cluster operator/(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N2 / 2; ++i)
            result.data[i] = _mm_div_pd(data[i], other.data[i]);
        return result;
    }
    simd_vector_cluster operator/=(const simd_vector_cluster& other) {
        for (size_t i = 0; i < N2 / 2; ++i)
            data[i] = _mm_div_pd(data[i], other.data[i]);
        return *this;
    }
};
template <size_t requested_size> struct simd_vector_cluster<requested_size, float> {
    static constexpr size_t actual_size = (requested_size + 1) & ~size_t(1); // Ceil to nearest multiple of 2
    static_assert(actual_size > 0, "N must be positive.");
    __m128 data[actual_size / 2];

    simd_vector_cluster() {
        for (size_t i = 0; i < actual_size / 2; ++i)
            data[i] = _mm_setzero_ps();
    }

    simd_vector_cluster(const float* values) {
        for (size_t i = 0; i < actual_size / 2; ++i)
            data[i] = _mm_loadu_ps(&values[i * 4]);
    }

    float get(size_t idx) const {
        size_t vec_idx = idx / 2;
        size_t elem_idx = idx % 2;
        alignas(16) float vals[4];
        _mm_store_ps(vals, data[vec_idx]);
        return vals[elem_idx];
    }

    void set(size_t idx, float value) {
        size_t vec_idx = idx / 2;
        size_t elem_idx = idx % 2;
        alignas(16) float vals[2];
        _mm_store_ps(vals, data[vec_idx]);
        vals[elem_idx] = value;
        data[vec_idx] = _mm_load_ps(vals);
    }

    simd_vector_cluster operator+(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < actual_size / 2; ++i)
            result.data[i] = _mm_add_ps(data[i], other.data[i]);
        return result;
    }

    simd_vector_cluster operator-(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < actual_size / 2; ++i)
            result.data[i] = _mm_sub_ps(data[i], other.data[i]);
        return result;
    }

    simd_vector_cluster operator*(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < actual_size / 2; ++i)
            result.data[i] = _mm_mul_ps(data[i], other.data[i]);
        return result;
    }
    simd_vector_cluster& operator*=(const simd_vector_cluster& other) {
        for (size_t i = 0; i < actual_size / 2; ++i)
            data[i] = _mm_mul_ps(data[i], other.data[i]);
        return *this;
    }

    simd_vector_cluster operator/(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < actual_size / 2; ++i)
            result.data[i] = _mm_div_ps(data[i], other.data[i]);
        return result;
    }
    simd_vector_cluster& operator/=(const simd_vector_cluster& other) {
        for (size_t i = 0; i < actual_size / 2; ++i)
            data[i] = _mm_div_ps(data[i], other.data[i]);
        return *this;
    }
};
#else
template <size_t N, typename T> struct simd_vector_cluster {
    T data[N];

    simd_vector_cluster() {
        for (size_t i = 0; i < N; ++i)
            data[i] = T();
    }
    T get(size_t idx) const { return data[idx]; }
    void set(size_t idx, T value) { data[idx] = value; }

    simd_vector_cluster operator+(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N; ++i)
            result.data[i] = data[i] + other.data[i];
        return result;
    }
    simd_vector_cluster operator-(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N; ++i)
            result.data[i] = data[i] - other.data[i];
        return result;
    }
    simd_vector_cluster operator*(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N; ++i)
            result.data[i] = data[i] * other.data[i];
        return result;
    }
    simd_vector_cluster operator*=(const simd_vector_cluster& other) {
        for (size_t i = 0; i < N; ++i)
            data[i] *= other.data[i];
        return *this;
    }
    simd_vector_cluster operator/(const simd_vector_cluster& other) const {
        simd_vector_cluster result;
        for (size_t i = 0; i < N; ++i)
            result.data[i] = data[i] / other.data[i];
        return result;
    }
    simd_vector_cluster operator/=(const simd_vector_cluster& other) {
        for (size_t i = 0; i < N; ++i)
            data[i] /= other.data[i];
        return *this;
    }
};
#endif

template <int rows, int cols, typename T> struct matrix {
    static constexpr int num_rows = rows;
    static constexpr int num_cols = cols;
    static_assert(rows > 0 && cols > 0, "Matrix dimensions must be positive.");

    simd_vector_cluster<rows * cols, T> data;

    matrix() {
        // default to identity matrix if square
        if constexpr (rows == cols) {
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    set(r, c, r == c ? T(1) : T(0));
                }
            }
        } else {
            for (int i = 0; i < rows * cols; ++i) {
                data.set(i, T(0));
            }
        }
    }

    T get(int row, int col) const { return data.get(row * cols + col); }

    void set(int row, int col, T value) { data.set(row * cols + col, value); }

    const matrix& identity() {
        static matrix identity_matrix;
        static bool initialized = false;
        if (!initialized) {
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    identity_matrix.set(r, c, r == c ? T(1) : T(0));
                }
            }
            initialized = true;
        }
        return identity_matrix;
    }

    matrix transpose() const {
        matrix result;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                result.set(c, r, get(r, c));
            }
        }
        return result;
    }

    void transpose_in_place() {
        for (int r = 0; r < rows; ++r) {
            for (int c = r + 1; c < cols; ++c) {
                T temp = get(r, c);
                set(r, c, get(c, r));
                set(c, r, temp);
            }
        }
    }

    void inverse_in_place() {
        static_assert(rows == cols, "Inverse is only defined for square matrices.");
        matrix result = matrix::identity();
        matrix copy = *this;

        for (int r = 0; r < rows; ++r) {
            // Find pivot
            T pivot = copy.get(r, r);
            if (pivot == T(0)) {
                // Find a row below the current row to swap with
                for (int rr = r + 1; rr < rows; ++rr) {
                    if (copy.get(rr, r) != T(0)) {
                        // Swap rows in both matrices
                        for (int c = 0; c < cols; ++c) {
                            T temp = copy.get(r, c);
                            copy.set(r, c, copy.get(rr, c));
                            copy.set(rr, c, temp);

                            temp = result.get(r, c);
                            result.set(r, c, result.get(rr, c));
                            result.set(rr, c, temp);
                        }
                        pivot = copy.get(r, r);
                        break;
                    }
                }
            }
            edvar::diagnostics::check_printf(pivot == T(0), L"Matrix is singular and cannot be inverted.");

            // Normalize pivot row
            for (int c = 0; c < cols; ++c) {
                copy.set(r, c, copy.get(r, c) / pivot);
                result.set(r, c, result.get(r, c) / pivot);
            }

            // Eliminate other rows
            for (int rr = 0; rr < rows; ++rr) {
                if (rr != r) {
                    T factor = copy.get(rr, r);
                    for (int c = 0; c < cols; ++c) {
                        copy.set(rr, c, copy.get(rr, c) - factor * copy.get(r, c));
                        result.set(rr, c, result.get(rr, c) - factor * result.get(r, c));
                    }
                }
            }
        }

        *this = result;
    }

    int64 dot(const matrix& other) const {
        static_assert(rows == 1 || cols == 1, "Dot product is only defined for vectors.");
        static_assert(other.num_rows == 1 || other.num_cols == 1, "Dot product is only defined for vectors.");
        edvar::diagnostics::check_printf(rows * cols != other.num_rows * other.num_cols,
                                         L"Dot product requires vectors of the same size.");
        int64 result = 0;
        for (int i = 0; i < rows * cols; ++i) {
            result += static_cast<int64>(get(i / cols, i % cols)) *
                      static_cast<int64>(other.get(i / other.num_cols, i % other.num_cols));
        }
        return result;
    }

    T* flatten() { return reinterpret_cast<T*>(data.data); }

    matrix inverse() const {
        matrix result = *this;
        result.inverse_in_place();
        return result;
    }

    double determinant() const {
        static_assert(rows == cols, "Determinant is only defined for square matrices.");
        if constexpr (rows == 1) {
            return get(0, 0);
        } else if constexpr (rows == 2) {
            return get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0);
        } else if constexpr (rows == 3) {
            return get(0, 0) * (get(1, 1) * get(2, 2) - get(1, 2) * get(2, 1)) -
                   get(0, 1) * (get(1, 0) * get(2, 2) - get(1, 2) * get(2, 0)) +
                   get(0, 2) * (get(1, 0) * get(2, 1) - get(1, 1) * get(2, 0));
        } else {
            double det = 0;
            for (int c = 0; c < cols; ++c) {
                matrix<rows - 1, cols - 1, T> sub = submatrix<rows - 1, cols - 1>(0, c);
                det += ((c % 2 == 0 ? 1 : -1) * get(0, c) * sub.determinant());
            }
            return det;
        }
    }

    template <int new_rows, int new_cols>
    matrix<new_rows, new_cols, T> submatrix(int row_offset, int col_offset) const {
        static_assert(new_rows <= rows && new_cols <= cols,
                      "Submatrix size must be less than or equal to original matrix size.");
        matrix<new_rows, new_cols, T> result;
        for (int r = 0; r < new_rows; ++r) {
            for (int c = 0; c < new_cols; ++c) {
                result.set(r, c, get(r + row_offset, c + col_offset));
            }
        }
        return result;
    }

    matrix operator+(const matrix& other) const {
        matrix result;
        result.data = data + other.data;
        return result;
    }

    matrix operator-(const matrix& other) const {
        matrix result;
        result.data = data - other.data;
        return result;
    }

    matrix operator*(const matrix& other) const {
        matrix result;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                T sum = T();
                for (int k = 0; k < cols; ++k) {
                    sum += get(r, k) * other.get(k, c);
                }
                result.set(r, c, sum);
            }
        }
        return result;
    }

    matrix operator*(T scalar) const {
        matrix result;
        for (int i = 0; i < rows * cols; ++i) {
            result.data.set(i, data.get(i) * scalar);
        }
        return result;
    }
};

using matrix_3x3d = matrix<3, 3, double>;
using matrix_4x4d = matrix<4, 4, double>;
using matrix_3x3f = matrix<3, 3, float>;
using matrix_4x4f = matrix<4, 4, float>;

struct vector3 {

    vector3() {
        data.set(0, 0.0);
        data.set(1, 0.0);
        data.set(2, 0.0);
    }
    vector3(double x, double y, double z) {
        data.set(0, x);
        data.set(1, y);
        data.set(2, z);
    }
    vector3(const double* values) {
        data.set(0, values[0]);
        data.set(1, values[1]);
        data.set(2, values[2]);
    }
    vector3(const array<double>& values) {
        edvar::diagnostics::check_printf(values.length() < 3, L"Array must have at least 3 elements.");
        data.set(0, values[0]);
        data.set(1, values[1]);
        data.set(2, values[2]);
    }
    vector3(const container::array_view<double>& values) {
        edvar::diagnostics::check_printf(values.length() < 3, L"ArrayView must have at least 3 elements.");
        data.set(0, values[0]);
        data.set(1, values[1]);
        data.set(2, values[2]);
    }
    vector3(const std::initializer_list<double>& values) {
        edvar::diagnostics::check_printf(values.size() < 3, L"Initializer list must have at least 3 elements.");
        auto it = values.begin();
        data.set(0, *it++);
        data.set(1, *it++);
        data.set(2, *it++);
    }

    double x() const { return data.get(0); }
    double y() const { return data.get(1); }
    double z() const { return data.get(2); }

    void set_x(double value) { data.set(0, value); }
    void set_y(double value) { data.set(1, value); }
    void set_z(double value) { data.set(2, value); }

    double length() const { return math::square_root(x() * x() + y() * y() + z() * z()); }
    double squared_length() const { return x() * x() + y() * y() + z() * z(); }

    void invert_in_place() {
        data.set(0, -data.get(0));
        data.set(1, -data.get(1));
        data.set(2, -data.get(2));
    }
    vector3 invert() const {
        vector3 copy = *this;
        copy.invert_in_place();
        return copy;
    }
    void normalize_in_place() {
        double len = length();
        if (len == 0.0) {
            data.set(0, 0.0);
            data.set(1, 0.0);
            data.set(2, 0.0);
            return;
        }
        data.set(0, data.get(0) / len);
        data.set(1, data.get(1) / len);
        data.set(2, data.get(2) / len);
    }
    vector3 normalize() const {
        vector3 copy = *this;
        copy.normalize_in_place();
        return copy;
    }
    void mirror_in_place(const vector3& normal) {
        vector3 n = normal.normalize();
        double dot_product = this->dot(n);
        data.set(0, data.get(0) - 2 * dot_product * n.x());
        data.set(1, data.get(1) - 2 * dot_product * n.y());
        data.set(2, data.get(2) - 2 * dot_product * n.z());
    }
    vector3 mirror(const vector3& normal) const {
        vector3 copy = *this;
        copy.mirror_in_place(normal);
        return copy;
    }

    vector3 operator+(const vector3& other) const { return vector3(x() + other.x(), y() + other.y(), z() + other.z()); }

    vector3 operator-(const vector3& other) const { return vector3(x() - other.x(), y() - other.y(), z() - other.z()); }

    vector3 operator*(double scalar) const { return vector3(x() * scalar, y() * scalar, z() * scalar); }

    vector3 operator/(double scalar) const {
        edvar::diagnostics::check_printf(scalar == 0.0, L"Division by zero in vector3d.");
        return vector3(x() / scalar, y() / scalar, z() / scalar);
    }
    vector3 operator*(const vector3& other) const { return vector3(x() * other.x(), y() * other.y(), z() * other.z()); }
    vector3 operator/(const vector3& other) const {
        edvar::diagnostics::check_printf(other.x() == 0.0 || other.y() == 0.0 || other.z() == 0.0,
                                         L"Division by zero in vector3d.");
        return vector3(x() / other.x(), y() / other.y(), z() / other.z());
    }
    vector3& operator+=(const vector3& other) {
        set_x(x() + other.x());
        set_y(y() + other.y());
        set_z(z() + other.z());
        return *this;
    }
    vector3& operator-=(const vector3& other) {
        set_x(x() - other.x());
        set_y(y() - other.y());
        set_z(z() - other.z());
        return *this;
    }
    vector3& operator*=(double scalar) {
        set_x(x() * scalar);
        set_y(y() * scalar);
        set_z(z() * scalar);
        return *this;
    }
    vector3& operator/=(double scalar) {
        edvar::diagnostics::check_printf(scalar == 0.0, L"Division by zero in vector3d.");
        set_x(x() / scalar);
        set_y(y() / scalar);
        set_z(z() / scalar);
        return *this;
    }
    vector3& operator*=(const vector3& other) {
        data *= other.data;
        return *this;
    }

    double dot(const vector3& other) const { return x() * other.x() + y() * other.y() + z() * other.z(); }

    vector3 cross(const vector3& other) const {
        return vector3(y() * other.z() - z() * other.y(), z() * other.x() - x() * other.z(),
                       x() * other.y() - y() * other.x());
    }

private:
    simd_vector_cluster<3, double> data;
};
} // namespace edvar::math