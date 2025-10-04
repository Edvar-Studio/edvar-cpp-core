
#include "math/simd_support.h"
#include "intrin.h"
#include <emmintrin.h>
#include <cmath>

namespace edvar::math::simd {

// simd_128ui implementation
simd_128ui::simd_128ui() : data(_mm_setzero_si128()) {}
simd_128ui::simd_128ui(const uint32 all) {
    alignas(16) uint32 arr[4] = {all, all, all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ui::simd_128ui(const uint32 x, const uint32 y, const uint32 z, const uint32 w) {
    alignas(16) uint32 arr[4] = {x, y, z, w};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ui::simd_128ui(const uint32* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
}
simd_128ui::simd_128ui(const std::initializer_list<uint32>& list) {
    alignas(16) uint32 arr[4] = {0, 0, 0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 4; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}

simd_128ui::~simd_128ui() { memory::zero_bytes(this->v, sizeof(this->v)); }

simd_128ui::simd_128ui(const simd_128ui& other) : data(other.data) {}
simd_128ui::simd_128ui(simd_128ui&& other) noexcept : data(other.data) { memory::zero_bytes(other.v, sizeof(other.v)); }

simd_128ui& simd_128ui::operator=(const simd_128ui& rhs) {
    this->data = rhs.data;
    return *this;
}
simd_128ui& simd_128ui::operator=(simd_128ui&& rhs) noexcept {
    this->data = rhs.data;
    memory::zero_bytes(rhs.v, sizeof(rhs.v));
    return *this;
}
simd_128ui& simd_128ui::operator=(const uint32* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128ui& simd_128ui::operator=(const std::initializer_list<uint32>& list) {
    alignas(16) uint32 arr[4] = {0, 0, 0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 4; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}
simd_128ui& simd_128ui::operator=(const uint32 all) {
    uint32 arr[4] = {all, all, all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}

simd_128ui simd_128ui::operator+(const simd_128ui& rhs) const {
    simd_128ui result;
    result.data = _mm_add_epi32(this->data, rhs.data);
    return result;
}
simd_128ui& simd_128ui::operator+=(const simd_128ui& rhs) {
    this->data = _mm_add_epi32(this->data, rhs.data);
    return *this;
    return *this;
}
simd_128ui simd_128ui::operator-(const simd_128ui& rhs) const {
    simd_128ui result;
    result.data = _mm_sub_epi32(this->data, rhs.data);
    return result;
}
simd_128ui& simd_128ui::operator-=(const simd_128ui& rhs) {
    this->data = _mm_sub_epi32(this->data, rhs.data);
    return *this;
}
simd_128ui simd_128ui::operator*(const simd_128ui& rhs) const {
    simd_128ui result;
    result.data = _mm_mul_epu32(this->data, rhs.data);
    return result;
}
simd_128ui& simd_128ui::operator*=(const simd_128ui& rhs) {
    this->data = _mm_mul_epu32(this->data, rhs.data);
    return *this;
}
simd_128ui simd_128ui::operator/(const simd_128ui& rhs) const {
    simd_128ui result;
    result.x = this->x / rhs.x;
    result.y = this->y / rhs.y;
    result.z = this->z / rhs.z;
    result.w = this->w / rhs.w;
    return result;
}
simd_128ui& simd_128ui::operator/=(const simd_128ui& rhs) {
    x = x / rhs.x;
    y = y / rhs.y;
    z = z / rhs.z;
    w = w / rhs.w;
    return *this;
}

simd_128ui simd_128ui::operator&(const simd_128ui& rhs) const {
    simd_128ui result;
    result.data = _mm_and_si128(this->data, rhs.data);
    return result;
}
simd_128ui simd_128ui::operator|(const simd_128ui& rhs) const {
    simd_128ui result;
    result.data = _mm_or_si128(this->data, rhs.data);
    return result;
}
simd_128ui& simd_128ui::operator&=(const simd_128ui& rhs) {
    this->data = _mm_and_si128(this->data, rhs.data);
    return *this;
}
simd_128ui& simd_128ui::operator|=(const simd_128ui& rhs) {
    this->data = _mm_or_si128(this->data, rhs.data);
    return *this;
}
simd_128ui simd_128ui::operator^(const simd_128ui& rhs) const {
    simd_128ui result;
    result.data = _mm_xor_si128(this->data, rhs.data);
    return result;
}
simd_128ui& simd_128ui::operator^=(const simd_128ui& rhs) {
    this->data = _mm_xor_si128(this->data, rhs.data);
    return *this;
}

simd_128ui simd_128ui::operator!() const {
    simd_128ui result;
    result.data = _mm_andnot_si128(this->data, _mm_set1_epi32(-1));
    return result;
}
simd_128ui simd_128ui::operator-() const {
    simd_128ui result;
    result.data = _mm_sub_epi32(_mm_setzero_si128(), this->data);
    return result;
}

bool simd_128ui::operator==(const simd_128ui& rhs) const {
    __m128i cmp = _mm_cmpeq_epi32(this->data, rhs.data);
    return cmp.m128i_i32[0] == -1 && cmp.m128i_i32[1] == -1 && cmp.m128i_i32[2] == -1 && cmp.m128i_i32[3] == -1;
}
bool simd_128ui::operator!=(const simd_128ui& rhs) const { return !(*this == rhs); }

simd_128ui simd_128ui::max(const simd_128ui& lhs, const simd_128ui& rhs) {
    simd_128ui result;
#if EDVAR_CPP_CORE_USE_SSE4_2
    result.data = _mm_max_epu32(lhs.data, rhs.data);
#else
    result.v[0] = (lhs.v[0] > rhs.v[0]) ? lhs.v[0] : rhs.v[0];
    result.v[1] = (lhs.v[1] > rhs.v[1]) ? lhs.v[1] : rhs.v[1];
    result.v[2] = (lhs.v[2] > rhs.v[2]) ? lhs.v[2] : rhs.v[2];
    result.v[3] = (lhs.v[3] > rhs.v[3]) ? lhs.v[3] : rhs.v[3];
#endif
    return result;
}
simd_128ui simd_128ui::min(const simd_128ui& lhs, const simd_128ui& rhs) {
    simd_128ui result;
#if EDVAR_CPP_CORE_USE_SSE4_2
    result.data = _mm_min_epu32(lhs.data, rhs.data);
#else
    result.v[0] = (lhs.v[0] < rhs.v[0]) ? lhs.v[0] : rhs.v[0];
    result.v[1] = (lhs.v[1] < rhs.v[1]) ? lhs.v[1] : rhs.v[1];
    result.v[2] = (lhs.v[2] < rhs.v[2]) ? lhs.v[2] : rhs.v[2];
    result.v[3] = (lhs.v[3] < rhs.v[3]) ? lhs.v[3] : rhs.v[3];
#endif
    return result;
}

simd_128ui simd_128ui::sqrt() const {
    simd_128ui result;
    // No SIMD sqrt for integers. Fallback to scalar.
    result.x = math::square_root(x);
    result.y = math::square_root(y);
    result.z = math::square_root(z);
    result.w = math::square_root(w);
    return result;
}
simd_128ui& simd_128ui::sqrt_inline() {
    x = math::square_root(x);
    y = math::square_root(y);
    z = math::square_root(z);
    w = math::square_root(w);
    return *this;
}

simd_128ui simd_128ui::shuffle(const simd_128ui::shuffle_opts& opts) const {
    simd_128ui result = *this;
    result.shuffle_inline(opts);
    return result;
}

simd_128ui& simd_128ui::shuffle_inline(const simd_128ui::shuffle_opts& opts) {
    this->data = _mm_shuffle_epi32(this->data, opts.to_int32());
    return *this;
}

simd_128ui simd_128ui::horizontal_add(const simd_128ui& rhs) const {
    simd_128ui result = *this;
    result.horizontal_add_inline(rhs);
    return result;
}

simd_128ui& simd_128ui::horizontal_add_inline(const simd_128ui& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    data = _mm_hadd_epi32(this->data, rhs.data);
#else
    x = x + y;
    y = z + w;
    z = rhs.x + rhs.y;
    w = rhs.z + rhs.w;
#endif
    return *this;
}

simd_128ui simd_128ui::horizontal_sub(const simd_128ui& rhs) const {
    simd_128ui result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_128ui& simd_128ui::horizontal_sub_inline(const simd_128ui& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    data = _mm_hsub_epi32(data, rhs.data);
#else
    x = x - y;
    y = z - w;
    z = rhs.x - rhs.y;
    w = rhs.z - rhs.w;
#endif
    return *this;
}

// simd_128ul implementation
simd_128ul::simd_128ul() : data(_mm_setzero_si128()) {}
simd_128ul::simd_128ul(const uint64 all) {
    alignas(16) uint64 arr[2] = {all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ul::simd_128ul(const uint64 x, const uint64 y) {
    alignas(16) uint64 arr[2] = {x, y};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ul::simd_128ul(const uint64* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
}
simd_128ul::simd_128ul(const std::initializer_list<uint64>& list) {
    alignas(16) uint64 arr[2] = {0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}

simd_128ul::~simd_128ul() { memory::zero_bytes(this->v, sizeof(this->v)); }

simd_128ul::simd_128ul(const simd_128ul& other) : data(other.data) {}
simd_128ul::simd_128ul(simd_128ul&& other) noexcept : data(other.data) { memory::zero_bytes(other.v, sizeof(other.v)); }

simd_128ul& simd_128ul::operator=(const simd_128ul& rhs) {
    this->data = rhs.data;
    return *this;
}
simd_128ul& simd_128ul::operator=(simd_128ul&& rhs) noexcept {
    this->data = rhs.data;
    memory::zero_bytes(rhs.v, sizeof(rhs.v));
    return *this;
}
simd_128ul& simd_128ul::operator=(const uint64* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128ul& simd_128ul::operator=(const std::initializer_list<uint64>& list) {
    alignas(16) uint64 arr[2] = {0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}
simd_128ul& simd_128ul::operator=(const uint64 all) {
    alignas(16) uint64 arr[2] = {all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}

simd_128ul simd_128ul::operator+(const simd_128ul& rhs) const {
    simd_128ul result;
    result.data = _mm_add_epi64(data, rhs.data);
    return result;
}
simd_128ul& simd_128ul::operator+=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] += rhs.v[i];
    return *this;
}
simd_128ul simd_128ul::operator-(const simd_128ul& rhs) const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] - rhs.v[i];
    return result;
}
simd_128ul& simd_128ul::operator-=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] -= rhs.v[i];
    return *this;
}
simd_128ul simd_128ul::operator*(const simd_128ul& rhs) const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] * rhs.v[i];
    return result;
}
simd_128ul& simd_128ul::operator*=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] *= rhs.v[i];
    return *this;
}
simd_128ul simd_128ul::operator/(const simd_128ul& rhs) const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] / rhs.v[i];
    return result;
}
simd_128ul& simd_128ul::operator/=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] /= rhs.v[i];
    return *this;
}

simd_128ul simd_128ul::operator&(const simd_128ul& rhs) const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] & rhs.v[i];
    return result;
}
simd_128ul simd_128ul::operator|(const simd_128ul& rhs) const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] | rhs.v[i];
    return result;
}
simd_128ul& simd_128ul::operator&=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] &= rhs.v[i];
    return *this;
}
simd_128ul& simd_128ul::operator|=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] |= rhs.v[i];
    return *this;
}
simd_128ul simd_128ul::operator^(const simd_128ul& rhs) const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] ^ rhs.v[i];
    return result;
}
simd_128ul& simd_128ul::operator^=(const simd_128ul& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] ^= rhs.v[i];
    return *this;
}

simd_128ul simd_128ul::operator!() const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = ~this->v[i];
    return result;
}
simd_128ul simd_128ul::operator-() const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = -this->v[i];
    return result;
}

bool simd_128ul::operator==(const simd_128ul& rhs) const {
    for (int i = 0; i < 2; ++i)
        if (this->v[i] != rhs.v[i])
            return false;
    return true;
}
bool simd_128ul::operator!=(const simd_128ul& rhs) const { return !(*this == rhs); }

simd_128ul simd_128ul::max(const simd_128ul& lhs, const simd_128ul& rhs) {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = lhs.v[i] > rhs.v[i] ? lhs.v[i] : rhs.v[i];
    return result;
}
simd_128ul simd_128ul::min(const simd_128ul& lhs, const simd_128ul& rhs) {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = lhs.v[i] < rhs.v[i] ? lhs.v[i] : rhs.v[i];
    return result;
}

simd_128ul simd_128ul::sqrt() const {
    simd_128ul result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = static_cast<uint64>(std::sqrt(static_cast<double>(this->v[i])));
    return result;
}
simd_128ul& simd_128ul::sqrt_inline() {
    for (int i = 0; i < 2; ++i)
        this->v[i] = static_cast<uint64>(std::sqrt(static_cast<double>(this->v[i])));
    return *this;
}

simd_128ul simd_128ul::shuffle(const simd_128ul& other, const simd_128ul::shuffle_opts& opts) const {
    simd_128ul result = *this;
    result.shuffle_inline(other, opts);
    return result;
}

simd_128ul& simd_128ul::shuffle_inline(const simd_128ul& other, const simd_128ul::shuffle_opts& opts) {
    x = (opts.x == shuffle_opts::use_x) ? x : y;
    y = (opts.y == shuffle_opts::use_x) ? other.x : other.y;
    return *this;
}

simd_128ul simd_128ul::horizontal_add(const simd_128ul& rhs) const {
    simd_128ul result = *this;
    result.horizontal_add_inline(rhs);
    return result;
}
simd_128ul& simd_128ul::horizontal_add_inline(const simd_128ul& rhs) {
    x = x + y;
    y = rhs.x + rhs.y;
    return *this;
}

simd_128ul simd_128ul::horizontal_sub(const simd_128ul& rhs) const {
    simd_128ul result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_128ul& simd_128ul::horizontal_sub_inline(const simd_128ul& rhs) {
    x = x - y;
    y = rhs.x - rhs.y;
    return *this;
}

// simd_128l implementation
simd_128l::simd_128l() : data(_mm_setzero_si128()) {}
simd_128l::simd_128l(const int64 all) {
    int64 arr[2] = {all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128l::simd_128l(const int64 x, const int64 y) {
    int64 arr[2] = {x, y};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128l::simd_128l(const int64* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
}
simd_128l::simd_128l(const std::initializer_list<int64>& list) {
    int64 arr[2] = {0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}

simd_128l::~simd_128l() { memory::zero_bytes(this->v, sizeof(this->v)); }

simd_128l::simd_128l(const simd_128l& other) : data(other.data) {}
simd_128l::simd_128l(simd_128l&& other) noexcept : data(other.data) { memory::zero_bytes(other.v, sizeof(other.v)); }

simd_128l& simd_128l::operator=(const simd_128l& rhs) {
    this->data = rhs.data;
    return *this;
}
simd_128l& simd_128l::operator=(simd_128l&& rhs) noexcept {
    this->data = rhs.data;
    memory::zero_bytes(rhs.v, sizeof(rhs.v));
    return *this;
}
simd_128l& simd_128l::operator=(const int64* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128l& simd_128l::operator=(const std::initializer_list<int64>& list) {
    int64 arr[2] = {0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}
simd_128l& simd_128l::operator=(const int64 all) {
    int64 arr[2] = {all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}

simd_128l simd_128l::operator+(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] + rhs.v[i];
    return result;
}
simd_128l& simd_128l::operator+=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] += rhs.v[i];
    return *this;
}
simd_128l simd_128l::operator-(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] - rhs.v[i];
    return result;
}
simd_128l& simd_128l::operator-=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] -= rhs.v[i];
    return *this;
}
simd_128l simd_128l::operator*(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] * rhs.v[i];
    return result;
}
simd_128l& simd_128l::operator*=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] *= rhs.v[i];
    return *this;
}
simd_128l simd_128l::operator/(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] / rhs.v[i];
    return result;
}
simd_128l& simd_128l::operator/=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] /= rhs.v[i];
    return *this;
}

simd_128l simd_128l::operator&(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] & rhs.v[i];
    return result;
}
simd_128l simd_128l::operator|(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] | rhs.v[i];
    return result;
}
simd_128l& simd_128l::operator&=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] &= rhs.v[i];
    return *this;
}
simd_128l& simd_128l::operator|=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] |= rhs.v[i];
    return *this;
}
simd_128l simd_128l::operator^(const simd_128l& rhs) const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = this->v[i] ^ rhs.v[i];
    return result;
}
simd_128l& simd_128l::operator^=(const simd_128l& rhs) {
    for (int i = 0; i < 2; ++i)
        this->v[i] ^= rhs.v[i];
    return *this;
}

simd_128l simd_128l::operator!() const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = ~this->v[i];
    return result;
}
simd_128l simd_128l::operator-() const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = -this->v[i];
    return result;
}

bool simd_128l::operator==(const simd_128l& rhs) const {
    for (int i = 0; i < 2; ++i)
        if (this->v[i] != rhs.v[i])
            return false;
    return true;
}
bool simd_128l::operator!=(const simd_128l& rhs) const { return !(*this == rhs); }

simd_128l simd_128l::max(const simd_128l& lhs, const simd_128l& rhs) {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = lhs.v[i] > rhs.v[i] ? lhs.v[i] : rhs.v[i];
    return result;
}
simd_128l simd_128l::min(const simd_128l& lhs, const simd_128l& rhs) {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = lhs.v[i] < rhs.v[i] ? lhs.v[i] : rhs.v[i];
    return result;
}

simd_128l simd_128l::sqrt() const {
    simd_128l result;
    for (int i = 0; i < 2; ++i)
        result.v[i] = static_cast<int64>(std::sqrt(static_cast<double>(this->v[i])));
    return result;
}
simd_128l& simd_128l::sqrt_inline() {
    for (int i = 0; i < 2; ++i)
        this->v[i] = static_cast<int64>(std::sqrt(static_cast<double>(this->v[i])));
    return *this;
}

simd_128l simd_128l::shuffle(const simd_128l& other, const simd_128l::shuffle_mode mode) const {
    simd_128l result = *this;
    result.shuffle_inline(other, mode);
    return result;
}

simd_128l& simd_128l::shuffle_inline(const simd_128l& other, const simd_128l::shuffle_mode mode) {
    // Only basic shuffle implemented, extend as needed
    int64 tmp[2];
    switch (mode) {
    case shuffle_mode::xy_yx:
        tmp[0] = this->v[0];
        tmp[1] = other.v[1];
        break;
    case shuffle_mode::yx_xy:
        tmp[0] = this->v[1];
        tmp[1] = other.v[0];
        break;
    case shuffle_mode::xx_yy:
        tmp[0] = this->v[0];
        tmp[1] = other.v[0];
        break;
    case shuffle_mode::yy_xx:
        tmp[0] = this->v[1];
        tmp[1] = other.v[1];
        break;
    default:
        tmp[0] = this->v[0];
        tmp[1] = this->v[1];
        break;
    }
    this->v[0] = tmp[0];
    this->v[1] = tmp[1];
    return *this;
}

// simd_128i implementation
simd_128i::simd_128i() : data(_mm_setzero_si128()) {}
simd_128i::simd_128i(const int32 all) : data(_mm_set1_epi32(all)) {}
simd_128i::simd_128i(const int32 x, const int32 y, const int32 z, const int32 w) : data(_mm_set_epi32(w, z, y, x)) {}
simd_128i::simd_128i(const int32* to_place_data)
    : data(_mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data))) {}
simd_128i::simd_128i(const std::initializer_list<int32>& list) {
    const int count = list.size();
    switch (count) {
    case 0:
        this->data = _mm_setzero_si128();
        return;
    case 1:
        this->data = _mm_set1_epi32(*list.begin());
        return;
    case 4:
        this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(list.begin()));
        return;
    default:
        this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(list.begin()));
        return;
    }
}

simd_128i::~simd_128i() { memory::zero_bytes(this->v, sizeof(this->v)); }

simd_128i::simd_128i(const simd_128i& other) : data(other.data) {}
simd_128i::simd_128i(simd_128i&& other) noexcept : data(other.data) { memory::zero_bytes(other.v, sizeof(other.v)); }

simd_128i& simd_128i::operator=(const simd_128i& rhs) {
    this->data = rhs.data;
    return *this;
}
simd_128i& simd_128i::operator=(simd_128i&& rhs) noexcept {
    this->data = rhs.data;
    memory::zero_bytes(rhs.v, sizeof(rhs.v));
    return *this;
}
simd_128i& simd_128i::operator=(const int32* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128i& simd_128i::operator=(const std::initializer_list<int32>& list) {
    const int count = list.size();
    switch (count) {
    case 0:
        this->data = _mm_setzero_si128();
        return *this;
    case 1:
        this->data = _mm_set1_epi32(*list.begin());
        return *this;
    case 4:
        this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(list.begin()));
        return *this;
    default:
        this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(list.begin()));
        return *this;
    }
}
simd_128i& simd_128i::operator=(const int32 all) {
    this->data = _mm_set1_epi32(all);
    return *this;
}

simd_128i simd_128i::operator+(const simd_128i& rhs) const {
    return simd_128i(_mm_add_epi32(this->data, rhs.data).m128i_i32);
}
simd_128i& simd_128i::operator+=(const simd_128i& rhs) {
    this->data = _mm_add_epi32(this->data, rhs.data);
    return *this;
}
simd_128i simd_128i::operator-(const simd_128i& rhs) const {
    return simd_128i(_mm_sub_epi32(this->data, rhs.data).m128i_i32);
}
simd_128i& simd_128i::operator-=(const simd_128i& rhs) {
    this->data = _mm_sub_epi32(this->data, rhs.data);
    return *this;
}
simd_128i simd_128i::operator*(const simd_128i& rhs) const {
    // SSE2 does not support integer multiplication for 32-bit, so fallback to manual
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = this->v[i] * rhs.v[i];
    return result;
}
simd_128i& simd_128i::operator*=(const simd_128i& rhs) {
    for (int i = 0; i < 4; ++i)
        this->v[i] *= rhs.v[i];
    return *this;
}
simd_128i simd_128i::operator/(const simd_128i& rhs) const {
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = this->v[i] / rhs.v[i];
    return result;
}
simd_128i& simd_128i::operator/=(const simd_128i& rhs) {
    for (int i = 0; i < 4; ++i)
        this->v[i] /= rhs.v[i];
    return *this;
}

simd_128i simd_128i::operator&(const simd_128i& rhs) const {
    return simd_128i(_mm_and_si128(this->data, rhs.data).m128i_i32);
}
simd_128i simd_128i::operator|(const simd_128i& rhs) const {
    return simd_128i(_mm_or_si128(this->data, rhs.data).m128i_i32);
}
simd_128i& simd_128i::operator&=(const simd_128i& rhs) {
    this->data = _mm_and_si128(this->data, rhs.data);
    return *this;
}
simd_128i& simd_128i::operator|=(const simd_128i& rhs) {
    this->data = _mm_or_si128(this->data, rhs.data);
    return *this;
}
simd_128i simd_128i::operator^(const simd_128i& rhs) const {
    return simd_128i(_mm_xor_si128(this->data, rhs.data).m128i_i32);
}
simd_128i& simd_128i::operator^=(const simd_128i& rhs) {
    this->data = _mm_xor_si128(this->data, rhs.data);
    return *this;
}

simd_128i simd_128i::operator!() const {
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = ~this->v[i];
    return result;
}
simd_128i simd_128i::operator-() const {
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = -this->v[i];
    return result;
}

bool simd_128i::operator==(const simd_128i& rhs) const {
    for (int i = 0; i < 4; ++i)
        if (this->v[i] != rhs.v[i])
            return false;
    return true;
}
bool simd_128i::operator!=(const simd_128i& rhs) const { return !(*this == rhs); }

simd_128i simd_128i::max(const simd_128i& lhs, const simd_128i& rhs) {
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = lhs.v[i] > rhs.v[i] ? lhs.v[i] : rhs.v[i];
    return result;
}
simd_128i simd_128i::min(const simd_128i& lhs, const simd_128i& rhs) {
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = lhs.v[i] < rhs.v[i] ? lhs.v[i] : rhs.v[i];
    return result;
}

simd_128i simd_128i::sqrt() const {
    simd_128i result;
    for (int i = 0; i < 4; ++i)
        result.v[i] = static_cast<int32>(std::sqrt(static_cast<float>(this->v[i])));
    return result;
}
simd_128i& simd_128i::sqrt_inline() {
    for (int i = 0; i < 4; ++i)
        this->v[i] = static_cast<int32>(std::sqrt(static_cast<float>(this->v[i])));
    return *this;
}

simd_128i simd_128i::shuffle(const simd_128i& other, const simd_128i::shuffle_mode mode) const {
    simd_128i result = *this;
    result.shuffle_inline(other, mode);
    return result;
}

simd_128i& simd_128i::shuffle_inline(const simd_128i& other, const simd_128i::shuffle_mode mode) {
    // Only basic shuffle implemented, extend as needed
    this->data = _mm_shuffle_epi32(this->data, static_cast<int>(mode));
    return *this;
}

// simd_128f implementation
simd_128f::simd_128f() : data(_mm_setzero_ps()) {}
simd_128f::simd_128f(const float all) : data(_mm_set1_ps(all)) {}
simd_128f::simd_128f(const float x, const float y, const float z, const float w) : data(_mm_set_ps(w, z, y, x)) {}
simd_128f::simd_128f(const float* to_place_data) : data(_mm_loadu_ps(to_place_data)) {}
simd_128f::simd_128f(const std::initializer_list<float>& list) {
    const int count = list.size();
    switch (count) {
    case 0:
        this->data = _mm_setzero_ps();
        return;
    case 1:
        this->data = _mm_set1_ps(*list.begin());
        return;
    case 4:
        this->data = _mm_loadu_ps(list.begin());
        return;
    default:
        this->data = _mm_loadu_ps(list.begin());
        return;
    }
}

simd_128f::~simd_128f() { memory::zero_bytes(this->v, sizeof(this->v)); }

simd_128f::simd_128f(const simd_128f& other) : data(other.data) {}
simd_128f::simd_128f(simd_128f&& other) noexcept : data(other.data) { memory::zero_bytes(other.v, sizeof(other.v)); }

simd_128f& simd_128f::operator=(const simd_128f& rhs) {
    this->data = rhs.data;
    return *this;
}
simd_128f& simd_128f::operator=(simd_128f&& rhs) noexcept {
    this->data = rhs.data;
    memory::zero_bytes(rhs.v, sizeof(rhs.v));
    return *this;
}
simd_128f& simd_128f::operator=(const float* to_place_data) {
    this->data = _mm_loadu_ps(to_place_data);
    return *this;
}
simd_128f& simd_128f::operator=(const std::initializer_list<float>& list) {
    const int count = list.size();
    switch (count) {
    case 0:
        this->data = _mm_setzero_ps();
        return *this;
    case 1:
        this->data = _mm_set1_ps(*list.begin());
        return *this;
    case 4:
        this->data = _mm_loadu_ps(list.begin());
        return *this;
    default:
        this->data = _mm_loadu_ps(list.begin());
        return *this;
    }
}
simd_128f& simd_128f::operator=(const float all) {
    this->data = _mm_set1_ps(all);
    return *this;
}

simd_128f simd_128f::operator+(const simd_128f& rhs) const {
    return simd_128f(_mm_add_ps(this->data, rhs.data).m128_f32);
}
simd_128f& simd_128f::operator+=(const simd_128f& rhs) {
    this->data = _mm_add_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator-(const simd_128f& rhs) const {
    return simd_128f(_mm_sub_ps(this->data, rhs.data).m128_f32);
}
simd_128f& simd_128f::operator-=(const simd_128f& rhs) {
    this->data = _mm_sub_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator*(const simd_128f& rhs) const {
    return simd_128f(_mm_mul_ps(this->data, rhs.data).m128_f32);
}
simd_128f& simd_128f::operator*=(const simd_128f& rhs) {
    this->data = _mm_mul_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator/(const simd_128f& rhs) const {
    return simd_128f(_mm_div_ps(this->data, rhs.data).m128_f32);
}
simd_128f& simd_128f::operator/=(const simd_128f& rhs) {
    this->data = _mm_div_ps(this->data, rhs.data);
    return *this;
}

simd_128f simd_128f::operator&(const simd_128f& rhs) const {
    return simd_128f(_mm_and_ps(this->data, rhs.data).m128_f32);
}
simd_128f simd_128f::operator|(const simd_128f& rhs) const {
    return simd_128f(_mm_or_ps(this->data, rhs.data).m128_f32);
}
simd_128f& simd_128f::operator&=(const simd_128f& rhs) {
    this->data = _mm_and_ps(this->data, rhs.data);
    return *this;
}
simd_128f& simd_128f::operator|=(const simd_128f& rhs) {
    this->data = _mm_or_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator^(const simd_128f& rhs) const {
    return simd_128f(_mm_xor_ps(this->data, rhs.data).m128_f32);
}
simd_128f& simd_128f::operator^=(const simd_128f& rhs) {
    this->data = _mm_xor_ps(this->data, rhs.data);
    return *this;
}

simd_128f simd_128f::operator!() const { return simd_128f(_mm_xor_ps(this->data, _mm_set1_ps(-1.0f)).m128_f32); }
simd_128f simd_128f::operator-() const { return simd_128f(_mm_sub_ps(_mm_setzero_ps(), this->data).m128_f32); }

bool simd_128f::operator==(const simd_128f& rhs) const {
    __m128 cmp = _mm_cmpeq_ps(this->data, rhs.data);
    return cmp.m128_f32[0] != 0.0f && cmp.m128_f32[1] != 0.0f && cmp.m128_f32[2] != 0.0f && cmp.m128_f32[3] != 0.0f;
}
bool simd_128f::operator!=(const simd_128f& rhs) const { return !(*this == rhs); }

simd_128f simd_128f::max(const simd_128f& lhs, const simd_128f& rhs) {
    return simd_128f(_mm_max_ps(lhs.data, rhs.data).m128_f32);
}
simd_128f simd_128f::min(const simd_128f& lhs, const simd_128f& rhs) {
    return simd_128f(_mm_min_ps(lhs.data, rhs.data).m128_f32);
}

simd_128f simd_128f::sqrt() const { return simd_128f(_mm_sqrt_ps(this->data).m128_f32); }
simd_128f& simd_128f::sqrt_inline() {
    this->data = _mm_sqrt_ps(this->data);
    return *this;
}

simd_128f simd_128f::shuffle(const simd_128f& other, const simd_128f::shuffle_mode mode) const {
    simd_128f result = *this;
    result.shuffle_inline(other, mode);
    return result;
}

simd_128f& simd_128f::shuffle_inline(const simd_128f& other, const simd_128f::shuffle_mode mode) {
    this->data = _mm_shuffle_ps(this->data, other.data, static_cast<int>(mode));
    return *this;
}

simd_128f simd_128f::horizontal_add(const simd_128f& rhs) const {
    simd_128f result = *this;
    result.horizontal_add_inline(rhs);
    return result;
}
simd_128f& simd_128f::horizontal_add_inline(const simd_128f& rhs) {
    this->data = _mm_hadd_ps(this->data, rhs.data);
    return *this;
}

simd_128f simd_128f::horizontal_sub(const simd_128f& rhs) const {
    simd_128f result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_128f& simd_128f::horizontal_sub_inline(const simd_128f& rhs) {
    this->data = _mm_hsub_ps(this->data, rhs.data);
    return *this;
}

// simd_128d implementation
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

simd_128d::~simd_128d() { memory::zero_bytes(this->v, sizeof(this->v)); }

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