#include "math/simd_support.h"
#include <emmintrin.h>

namespace edvar::math::simd {
simd_128d::simd_128d() : data(_mm_setzero_pd()) {}
simd_128d::simd_128d(const double all) : data(_mm_set1_pd(all)) {}
simd_128d::simd_128d(const double x, const double y) : data(_mm_set_pd(y, x)) {}
simd_128d::simd_128d(const double* to_place_data) : data(_mm_loadu_pd(to_place_data)) {}
simd_128d::simd_128d(const std::initializer_list<double>& list) {
    const int count = list.size();
    switch (count) {
    case 0:
        this->data = _mm_setzero_pd();
        return;
    case 1:
        this->data = _mm_set1_pd(*list.begin());
        return;
    case 2:
        this->data = _mm_loadu_pd(list.begin());
        return;
    default:
        // If more than 2 elements are provided, only the first two are used.
        this->data = _mm_loadu_pd(list.begin());
        return;
    }
}

simd_128d::~simd_128d() {
    _mm_clflush(&data);
    memory::zero_bytes(this->v, sizeof(this->v));
}

simd_128d& simd_128d::operator=(const double* to_place_data) {
    this->data = _mm_loadu_pd(to_place_data);
    return *this;
}
simd_128d& simd_128d::operator=(const std::initializer_list<double>& list) {
    const int count = list.size();
    switch (count) {
    case 0:
        this->data = _mm_setzero_pd();
        return *this;
    case 1:
        this->data = _mm_set1_pd(*list.begin());
        return *this;
    case 2:
        this->data = _mm_loadu_pd(list.begin());
        return *this;
    default:
        // If more than 2 elements are provided, only the first two are used.
        this->data = _mm_loadu_pd(list.begin());
        return *this;
    }
}
simd_128d& simd_128d::operator=(const double all) {
    this->data = _mm_set1_pd(all);
    return *this;
}

simd_128d simd_128d::operator+(const simd_128d& rhs) const {
    return simd_128d(_mm_add_pd(this->data, rhs.data).m128d_f64);
}
simd_128d& simd_128d::operator+=(const simd_128d& rhs) {
    this->data = _mm_add_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator-(const simd_128d& rhs) const {
    return simd_128d(_mm_sub_pd(this->data, rhs.data).m128d_f64);
}
simd_128d& simd_128d::operator-=(const simd_128d& rhs) {
    this->data = _mm_sub_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator*(const simd_128d& rhs) const {
    return simd_128d(_mm_mul_pd(this->data, rhs.data).m128d_f64);
}
simd_128d& simd_128d::operator*=(const simd_128d& rhs) {
    this->data = _mm_mul_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator/(const simd_128d& rhs) const {
    return simd_128d(_mm_div_pd(this->data, rhs.data).m128d_f64);
}
simd_128d& simd_128d::operator/=(const simd_128d& rhs) {
    this->data = _mm_div_pd(this->data, rhs.data);
    return *this;
}

simd_128d simd_128d::operator&(const simd_128d& rhs) const {
    return simd_128d(_mm_and_pd(this->data, rhs.data).m128d_f64);
}
simd_128d simd_128d::operator|(const simd_128d& rhs) const {
    return simd_128d(_mm_or_pd(this->data, rhs.data).m128d_f64);
}
simd_128d& simd_128d::operator&=(const simd_128d& rhs) {
    this->data = _mm_and_pd(this->data, rhs.data);
    return *this;
}
simd_128d& simd_128d::operator|=(const simd_128d& rhs) {
    this->data = _mm_or_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator^(const simd_128d& rhs) const {
    return simd_128d(_mm_xor_pd(this->data, rhs.data).m128d_f64);
}
simd_128d& simd_128d::operator^=(const simd_128d& rhs) {
    this->data = _mm_xor_pd(this->data, rhs.data);
    return *this;
}

simd_128d simd_128d::operator!() const { return simd_128d(_mm_xor_pd(this->data, _mm_set1_pd(-1.0)).m128d_f64); }
simd_128d simd_128d::operator-() const { return simd_128d(_mm_sub_pd(_mm_setzero_pd(), this->data).m128d_f64); }

bool simd_128d::operator==(const simd_128d& rhs) const {
    __m128d cmp = _mm_cmpeq_pd(this->data, rhs.data);
    return cmp.m128d_f64[0] != 0.0 && cmp.m128d_f64[1] != 0.0;
}
bool simd_128d::operator!=(const simd_128d& rhs) const { return !(*this == rhs); }

simd_128d simd_128d::max(const simd_128d& lhs, const simd_128d& rhs) {
    return simd_128d(_mm_max_pd(lhs.data, rhs.data).m128d_f64);
}
simd_128d simd_128d::min(const simd_128d& lhs, const simd_128d& rhs) {
    return simd_128d(_mm_min_pd(lhs.data, rhs.data).m128d_f64);
}

simd_128d simd_128d::sqrt() const { return simd_128d(_mm_sqrt_pd(this->data).m128d_f64); }
simd_128d& simd_128d::sqrt_inline() {
    this->data = _mm_sqrt_pd(this->data);
    return *this;
}

simd_128d simd_128d::shuffle(const simd_128d& other, const simd_128d::shuffle_mode mode) const {
    simd_128d result = *this;
    result.shuffle_inline(other, mode);
    return result;
}

simd_128d& simd_128d::shuffle_inline(const simd_128d& other, const simd_128d::shuffle_mode mode) {
    this->data = _mm_shuffle_pd(this->data, other.data, static_cast<int>(mode));
    return *this;
}

simd_128d simd_128d::horizontal_add(const simd_128d& rhs) const {
    simd_128d result = *this;
    result.horizontal_add_inline(rhs);
    return result;
}
simd_128d& simd_128d::horizontal_add_inline(const simd_128d& rhs) {
    this->data = _mm_hadd_pd(this->data, rhs.data);
    return *this;
}

simd_128d simd_128d::horizontal_sub(const simd_128d& rhs) const {
    simd_128d result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_128d& simd_128d::horizontal_sub_inline(const simd_128d& rhs) {
    this->data = _mm_hsub_pd(this->data, rhs.data);
    return *this;
}
} // namespace edvar::math::simd