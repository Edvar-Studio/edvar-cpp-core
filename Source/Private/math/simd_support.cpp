
#include "math/simd_support.h"
#include "intrin.h"
#include "math/math.h"
#include "memory/operations.h"
#include <emmintrin.h>
#include <cmath>

namespace edvar::math::simd {

// simd_128ui implementation
simd_128ui::simd_128ui() : data(_mm_setzero_si128()) {}
simd_128ui::simd_128ui(const uint32_t all) {
    alignas(16) uint32_t arr[4] = {all, all, all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ui::simd_128ui(const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t w) {
    alignas(16) uint32_t arr[4] = {x, y, z, w};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ui::simd_128ui(const uint32_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
}
simd_128ui::simd_128ui(const std::initializer_list<uint32_t>& list) {
    alignas(16) uint32_t arr[4] = {0, 0, 0, 0};
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
simd_128ui& simd_128ui::operator=(const uint32_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128ui& simd_128ui::operator=(const std::initializer_list<uint32_t>& list) {
    alignas(16) uint32_t arr[4] = {0, 0, 0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 4; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}
simd_128ui& simd_128ui::operator=(const uint32_t all) {
    uint32_t arr[4] = {all, all, all, all};
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
    union helper {
        __m128i m;
       int32_t v[4];
    };
    helper h;
    h.m = cmp;
    return h.v[0] == -1 && h.v[1] == -1 && h.v[2] == -1 && h.v[3] == -1;
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
simd_128ul::simd_128ul(const uint64_t all) {
    alignas(16) uint64_t arr[2] = {all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ul::simd_128ul(const uint64_t x, const uint64_t y) {
    alignas(16) uint64_t arr[2] = {x, y};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128ul::simd_128ul(const uint64_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
}
simd_128ul::simd_128ul(const std::initializer_list<uint64_t>& list) {
    alignas(16) uint64_t arr[2] = {0, 0};
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
simd_128ul& simd_128ul::operator=(const uint64_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128ul& simd_128ul::operator=(const std::initializer_list<uint64_t>& list) {
    alignas(16) uint64_t arr[2] = {0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}
simd_128ul& simd_128ul::operator=(const uint64_t all) {
    alignas(16) uint64_t arr[2] = {all, all};
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
        result.v[i] = static_cast<uint64_t>(std::sqrt(static_cast<double>(this->v[i])));
    return result;
}
simd_128ul& simd_128ul::sqrt_inline() {
    for (int i = 0; i < 2; ++i)
        this->v[i] = static_cast<uint64_t>(std::sqrt(static_cast<double>(this->v[i])));
    return *this;
}

simd_128ul simd_128ul::shuffle(const simd_128ul& other, const simd_128ul::shuffle_opts& opts) const {
    simd_128ul result = *this;
    result.shuffle_inline(other, opts);
    return result;
}

simd_128ul& simd_128ul::shuffle_inline(const simd_128ul& other, const simd_128ul::shuffle_opts& opts) {
    x = (opts.x == shuffle_opts::use_first) ? x : y;
    y = (opts.y == shuffle_opts::use_first) ? other.x : other.y;
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
simd_128l::simd_128l(constint64_t all) {
   int64_t arr[2] = {all, all};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128l::simd_128l(constint64_t x, constint64_t y) {
   int64_t arr[2] = {x, y};
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
}
simd_128l::simd_128l(constint64_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
}
simd_128l::simd_128l(const std::initializer_listint64_t>& list) {
   int64_t arr[2] = {0, 0};
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
simd_128l& simd_128l::operator=(constint64_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128l& simd_128l::operator=(const std::initializer_listint64_t>& list) {
   int64_t arr[2] = {0, 0};
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
        arr[i] = *it;
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(arr));
    return *this;
}
simd_128l& simd_128l::operator=(constint64_t all) {
   int64_t arr[2] = {all, all};
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
        result.v[i] = static_castint64_t>(std::sqrt(static_cast<double>(this->v[i])));
    return result;
}
simd_128l& simd_128l::sqrt_inline() {
    x = math::square_root(x);
    y = math::square_root(y);
    return *this;
}

simd_128l simd_128l::shuffle(const simd_128l& other, const simd_128l::shuffle_opts& opts) const {
    simd_128l result = *this;
    result.shuffle_inline(other, opts);
    return result;
}

simd_128l& simd_128l::shuffle_inline(const simd_128l& other, const simd_128l::shuffle_opts& opts) {
    // no 64 bit shuffle instrinsics, fallback to scalar
    x = (opts.x == shuffle_opts::use_x) ? x : y;
    y = (opts.y == shuffle_opts::use_x) ? other.x : other.y;
    return *this;
}

// simd_128i implementation
simd_128i::simd_128i() : data(_mm_setzero_si128()) {}
simd_128i::simd_128i(constint32_t all) : data(_mm_set1_epi32(all)) {}
simd_128i::simd_128i(constint32_t x, constint32_t y, constint32_t z, constint32_t w) : data(_mm_set_epi32(w, z, y, x)) {}
simd_128i::simd_128i(constint32_t* to_place_data)
    : data(_mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data))) {}
simd_128i::simd_128i(const std::initializer_listint32_t>& list) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(list.begin()));
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
simd_128i& simd_128i::operator=(constint32_t* to_place_data) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(to_place_data));
    return *this;
}
simd_128i& simd_128i::operator=(const std::initializer_listint32_t>& list) {
    this->data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(list.begin()));
    return *this;
}
simd_128i& simd_128i::operator=(constint32_t all) {
    this->data = _mm_set1_epi32(all);
    return *this;
}

simd_128i simd_128i::operator+(const simd_128i& rhs) const {
    simd_128i copy = *this;
    copy += rhs;
    return copy;
}
simd_128i& simd_128i::operator+=(const simd_128i& rhs) {
    this->data = _mm_add_epi32(this->data, rhs.data);
    return *this;
}
simd_128i simd_128i::operator-(const simd_128i& rhs) const {
    simd_128i result = *this;
    result -= rhs;
    return result;
}
simd_128i& simd_128i::operator-=(const simd_128i& rhs) {
    this->data = _mm_sub_epi32(this->data, rhs.data);
    return *this;
}
simd_128i simd_128i::operator*(const simd_128i& rhs) const {
    simd_128i result;
#if EDVAR_CPP_CORE_USE_SSE4_2
    result.data = _mm_mul_epi32(data, rhs.data);
#else
    x = x * rhs.x;
    y = y * rhs.y;
    z = z * rhs.z;
    w = w * rhs.w;
#endif
    return result;
}
simd_128i& simd_128i::operator*=(const simd_128i& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    data = _mm_mul_epi32(data, rhs.data);
#else
    x = x * rhs.x;
    y = y * rhs.y;
    z = z * rhs.z;
    w = w * rhs.w;
#endif
    return *this;
}
simd_128i simd_128i::operator/(const simd_128i& rhs) const {
    simd_128i result;
    result.x = this->x / rhs.x;
    result.y = this->y / rhs.y;
    result.z = this->z / rhs.z;
    result.w = this->w / rhs.w;
    return result;
}
simd_128i& simd_128i::operator/=(const simd_128i& rhs) {
    x = x / rhs.x;
    y = y / rhs.y;
    z = z / rhs.z;
    w = w / rhs.w;
    return *this;
}

simd_128i simd_128i::operator&(const simd_128i& rhs) const {
    simd_128i result = *this;
    result.data = _mm_and_si128(this->data, rhs.data);
    return result;
}
simd_128i simd_128i::operator|(const simd_128i& rhs) const {
    simd_128i result = *this;
    result.data = _mm_or_si128(this->data, rhs.data);
    return result;
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
    simd_128i result = *this;
    result.data = _mm_xor_si128(this->data, rhs.data);
    return result;
}
simd_128i& simd_128i::operator^=(const simd_128i& rhs) {
    this->data = _mm_xor_si128(this->data, rhs.data);
    return *this;
}

simd_128i simd_128i::operator!() const {
    simd_128i result = *this;
    result.data = _mm_andnot_si128(this->data, _mm_set1_epi32(-1));
    return result;
}
simd_128i simd_128i::operator-() const {
    simd_128i result = *this;
    result.data = _mm_sub_epi32(_mm_setzero_si128(), this->data);
    return result;
}

bool simd_128i::operator==(const simd_128i& rhs) const {
    __m128i cmp = _mm_cmpeq_epi32(data, rhs.data);
    union helper {
        __m128i m128i;
       int32_t v[4];
    } h;
    h.m128i = cmp;
    return h.v[0] == -1 && h.v[1] == -1 && h.v[2] == -1 && h.v[3] == -1;
}

bool simd_128i::operator!=(const simd_128i& rhs) const { return !(*this == rhs); }

simd_128i simd_128i::operator<<(constint32_t shift) const {
    simd_128i result = *this;
    result <<= shift;
    return result;
}

simd_128i& simd_128i::operator<<=(constint32_t shift) {
    data = _mm_slli_epi32(data, shift);
    return *this;
}
simd_128i simd_128i::operator<<(const simd_128i& shift) const {
    simd_128i result = *this;
    result <<= shift;
    return result;
}
simd_128i& simd_128i::operator<<=(const simd_128i& shift) {
    data = _mm_sll_epi32(data, shift.data);
    return *this;
}

simd_128i simd_128i::operator>>(constint32_t shift) const {
    simd_128i result = *this;
    result >>= shift;
    return result;
}
simd_128i& simd_128i::operator>>=(constint32_t shift) {
    data = _mm_srli_epi32(data, shift);
    return *this;
}

simd_128i simd_128i::operator>>(const simd_128i& shift) const {
    simd_128i result = *this;
    result >>= shift;
    return result;
}
simd_128i& simd_128i::operator>>=(const simd_128i& shift) {
    data = _mm_srl_epi32(data, shift.data);
    return *this;
}

simd_128i simd_128i::max(const simd_128i& lhs, const simd_128i& rhs) {
    simd_128i result;
#if EDVAR_CPP_CORE_USE_SSE4_2
    result.data = _mm_max_epi32(lhs.data, rhs.data);
#else
    result.x = math::max(lhs.x, rhs.x);
    result.y = math::max(lhs.y, rhs.y);
    result.z = math::max(lhs.z, rhs.z);
    result.w = math::max(lhs.w, rhs.w);
#endif
    return result;
}
simd_128i simd_128i::min(const simd_128i& lhs, const simd_128i& rhs) {
    simd_128i result;
#if EDVAR_CPP_CORE_USE_SSE4_2
    result.data = _mm_min_epi32(lhs.data, rhs.data);
#else
    result.x = math::min(lhs.x, rhs.x);
    result.y = math::min(lhs.y, rhs.y);
    result.z = math::min(lhs.z, rhs.z);
    result.w = math::min(lhs.w, rhs.w);
#endif
    return result;
}

simd_128i simd_128i::sqrt() const {
    simd_128i result = *this;
    result.sqrt_inline();
    return result;
}
simd_128i& simd_128i::sqrt_inline() {
    x = static_castint32_t>(math::square_root(x));
    y = static_castint32_t>(math::square_root(y));
    z = static_castint32_t>(math::square_root(z));
    w = static_castint32_t>(math::square_root(w));
    return *this;
}

// simd_128f implementation
simd_128f::simd_128f() : data(_mm_setzero_ps()) {}
simd_128f::simd_128f(const float all) : data(_mm_set1_ps(all)) {}
simd_128f::simd_128f(const float x, const float y, const float z, const float w) : data(_mm_set_ps(w, z, y, x)) {}
simd_128f::simd_128f(const float* to_place_data) : data(_mm_loadu_ps(to_place_data)) {}
simd_128f::simd_128f(const std::initializer_list<float>& list) { this->data = _mm_loadu_ps(list.begin()); }

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
    this->data = _mm_loadu_ps(list.begin());
    return *this;
}
simd_128f& simd_128f::operator=(const float all) {
    this->data = _mm_set1_ps(all);
    return *this;
}

simd_128f simd_128f::operator+(const simd_128f& rhs) const {
    __m128 result = _mm_add_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f& simd_128f::operator+=(const simd_128f& rhs) {
    this->data = _mm_add_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator-(const simd_128f& rhs) const {
    __m128 result = _mm_sub_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f& simd_128f::operator-=(const simd_128f& rhs) {
    this->data = _mm_sub_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator*(const simd_128f& rhs) const {
    __m128 result = _mm_mul_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f& simd_128f::operator*=(const simd_128f& rhs) {
    this->data = _mm_mul_ps(this->data, rhs.data);
    return *this;
}
simd_128f simd_128f::operator/(const simd_128f& rhs) const {
    __m128 result = _mm_div_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f& simd_128f::operator/=(const simd_128f& rhs) {
    this->data = _mm_div_ps(this->data, rhs.data);
    return *this;
}

simd_128f simd_128f::operator&(const simd_128f& rhs) const {
    __m128 result = _mm_and_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f simd_128f::operator|(const simd_128f& rhs) const {
    __m128 result = _mm_or_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
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
    __m128 result = _mm_xor_ps(this->data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f& simd_128f::operator^=(const simd_128f& rhs) {
    this->data = _mm_xor_ps(this->data, rhs.data);
    return *this;
}

simd_128f simd_128f::operator!() const {
    __m128 result = _mm_xor_ps(this->data, _mm_set1_ps(-1.0f));
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f simd_128f::operator-() const {
    __m128 result = _mm_sub_ps(_mm_setzero_ps(), this->data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}

bool simd_128f::operator==(const simd_128f& rhs) const {
    __m128 cmp = _mm_cmpeq_ps(this->data, rhs.data);
    union helper {
        __m128 m;
        float f[4];
    };
    helper h;
    h.m = cmp;

    return h.f[0] != 0.0f && h.f[1] != 0.0f && h.f[2] != 0.0f && h.f[3] != 0.0f;
}
bool simd_128f::operator!=(const simd_128f& rhs) const { return !(*this == rhs); }

simd_128f simd_128f::max(const simd_128f& lhs, const simd_128f& rhs) {
    __m128 result = _mm_max_ps(lhs.data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f simd_128f::min(const simd_128f& lhs, const simd_128f& rhs) {
    __m128 result = _mm_min_ps(lhs.data, rhs.data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}

simd_128f simd_128f::sqrt() const {
    __m128 result = _mm_sqrt_ps(this->data);
    return simd_128f(reinterpret_cast<simd_128f&>(result));
}
simd_128f& simd_128f::sqrt_inline() {
    this->data = _mm_sqrt_ps(this->data);
    return *this;
}

simd_128f& simd_128f::horizontal_add_inline(const simd_128f& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    this->data = _mm_hadd_ps(this->data, rhs.data);
#else
    x = x + y;
    y = z + w;
    z = rhs.x + rhs.y;
    w = rhs.z + rhs.w;
#endif
    return *this;
}

simd_128f simd_128f::horizontal_sub(const simd_128f& rhs) const {
    simd_128f result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_128f& simd_128f::horizontal_sub_inline(const simd_128f& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    this->data = _mm_hsub_ps(this->data, rhs.data);
#else
    x = x - y;
    y = z - w;
    z = rhs.x - rhs.y;
    w = rhs.z - rhs.w;
#endif
    return *this;
}

float simd_128f::dot(const simd_128f& rhs) const {
    float dot_product;
#if EDVAR_CPP_CORE_USE_SSE4_2
    __m128 result_data = _mm_dp_ps(data, rhs.data, 0xF1);
    dot_product = _mm_cvtss_f32(result_data);
#else
    // no sse4.1 dot product intrinsic, fallback to mul + horizontal add
    simd_128f mul = (*this) * rhs;
    mul.horizontal_add_inline(mul);
    mul.horizontal_add_inline(mul);
    dot_product = mul.x;
#endif
    return dot_product;
}

// simd_128d implementation
simd_128d::simd_128d() : data(_mm_setzero_pd()) {}
simd_128d::simd_128d(const double all) : data(_mm_set1_pd(all)) {}
simd_128d::simd_128d(const double x, const double y) : data(_mm_set_pd(y, x)) {}
simd_128d::simd_128d(const double* to_place_data) : data(_mm_loadu_pd(to_place_data)) {}
simd_128d::simd_128d(const std::initializer_list<double>& list) { this->data = _mm_loadu_pd(list.begin()); }

simd_128d::~simd_128d() { memory::zero_bytes(this->v, sizeof(this->v)); }

simd_128d& simd_128d::operator=(const double* to_place_data) {
    this->data = _mm_loadu_pd(to_place_data);
    return *this;
}
simd_128d& simd_128d::operator=(const std::initializer_list<double>& list) {
    this->data = _mm_loadu_pd(list.begin());
    return *this;
}
simd_128d& simd_128d::operator=(const double all) {
    this->data = _mm_set1_pd(all);
    return *this;
}

simd_128d simd_128d::operator+(const simd_128d& rhs) const {
    __m128d result = _mm_add_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d& simd_128d::operator+=(const simd_128d& rhs) {
    this->data = _mm_add_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator-(const simd_128d& rhs) const {
    __m128d result = _mm_sub_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d& simd_128d::operator-=(const simd_128d& rhs) {
    this->data = _mm_sub_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator*(const simd_128d& rhs) const {
    __m128d result = _mm_mul_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d& simd_128d::operator*=(const simd_128d& rhs) {
    this->data = _mm_mul_pd(this->data, rhs.data);
    return *this;
}
simd_128d simd_128d::operator/(const simd_128d& rhs) const {
    __m128d result = _mm_div_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d& simd_128d::operator/=(const simd_128d& rhs) {
    this->data = _mm_div_pd(this->data, rhs.data);
    return *this;
}

simd_128d simd_128d::operator&(const simd_128d& rhs) const {
    __m128d result = _mm_and_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d simd_128d::operator|(const simd_128d& rhs) const {
    __m128d result = _mm_or_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
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
    __m128d result = _mm_xor_pd(this->data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d& simd_128d::operator^=(const simd_128d& rhs) {
    this->data = _mm_xor_pd(this->data, rhs.data);
    return *this;
}

simd_128d simd_128d::operator!() const {
    __m128d result = _mm_xor_pd(this->data, _mm_set1_pd(-1.0));
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d simd_128d::operator-() const {
    __m128d result = _mm_sub_pd(_mm_setzero_pd(), this->data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}

bool simd_128d::operator==(const simd_128d& rhs) const {
    __m128d cmp = _mm_cmpeq_pd(this->data, rhs.data);
    union helper {
        __m128d m;
        double f[2];
    };
    helper h;
    h.m = cmp;
    return h.f[0] != 0.0 && h.f[1] != 0.0;
}
bool simd_128d::operator!=(const simd_128d& rhs) const { return !(*this == rhs); }

simd_128d simd_128d::max(const simd_128d& lhs, const simd_128d& rhs) {
    __m128d result = _mm_max_pd(lhs.data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d simd_128d::min(const simd_128d& lhs, const simd_128d& rhs) {
    __m128d result = _mm_min_pd(lhs.data, rhs.data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}

simd_128d simd_128d::sqrt() const {
    __m128d result = _mm_sqrt_pd(this->data);
    return simd_128d(reinterpret_cast<simd_128d&>(result));
}
simd_128d& simd_128d::sqrt_inline() {
    this->data = _mm_sqrt_pd(this->data);
    return *this;
}

simd_128d simd_128d::horizontal_add(const simd_128d& rhs) const {
    simd_128d result = *this;
    result.horizontal_add_inline(rhs);
    return result;
}
simd_128d& simd_128d::horizontal_add_inline(const simd_128d& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    this->data = _mm_hadd_pd(this->data, rhs.data);
#else
    x = x + y;
    y = rhs.x + rhs.y;
#endif
    return *this;
}

simd_128d simd_128d::horizontal_sub(const simd_128d& rhs) const {
    simd_128d result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_128d& simd_128d::horizontal_sub_inline(const simd_128d& rhs) {
#if EDVAR_CPP_CORE_USE_SSE4_2
    this->data = _mm_hsub_pd(this->data, rhs.data);
#else
    x = x - y;
    y = rhs.x - rhs.y;
#endif
    return *this;
}

simd_256d::simd_256d()
    :
#if EDVAR_CPP_CORE_USE_AVX2
      data(_mm256_setzero_pd())
#else
      v{0.0, 0.0, 0.0, 0.0}
#endif
{
}
simd_256d::simd_256d(const double all) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_set1_pd(all);
#else
    v[0] = all;
    v[1] = all;
    v[2] = all;
    v[3] = all;
#endif
}
simd_256d::simd_256d(const double x, const double y, const double z, const double w) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_set_pd(w, z, y, x);
#else
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
#endif
}
simd_256d::simd_256d(const double* to_place_data) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_loadu_pd(to_place_data);
#else
    v[0] = to_place_data[0];
    v[1] = to_place_data[1];
    v[2] = to_place_data[2];
    v[3] = to_place_data[3];
#endif
}
simd_256d::simd_256d(const std::initializer_list<double>& list) : simd_256d(list.begin()) {}
simd_256d::~simd_256d() { memory::zero_bytes(this->v, sizeof(this->v)); }
simd_256d::simd_256d(const simd_256d& other) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = other.data;
#else
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
#endif
}
simd_256d::simd_256d(simd_256d&& other) noexcept
    :
#if EDVAR_CPP_CORE_USE_AVX2
      data(other.data)
#else
      v{other.v[0], other.v[1], other.v[2], other.v[3]}
#endif
{
    memory::zero_bytes(other.v, sizeof(other.v));
}

simd_256d& simd_256d::operator=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = rhs.data;
#else
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
#endif
    return *this;
}
simd_256d& simd_256d::operator=(simd_256d&& rhs) noexcept {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = std::move(rhs.data);
#else
    x = std::move(rhs.x);
    y = std::move(rhs.y);
    z = std::move(rhs.z);
    w = std::move(rhs.w);
#endif
    return *this;
}
simd_256d& simd_256d::operator=(const double* to_place_data) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_loadu_pd(to_place_data);
#else
    v[0] = to_place_data[0];
    v[1] = to_place_data[1];
    v[2] = to_place_data[2];
    v[3] = to_place_data[3];
#endif
    return *this;
}

simd_256d& simd_256d::operator=(const std::initializer_list<double>& list) { return (*this = list.begin()); }
simd_256d& simd_256d::operator=(const double all) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_set1_pd(all);
#else
    this->halves[0] = _mm_set1_pd(all);
    this->halves[1] = _mm_set1_pd(all);
#endif
    return *this;
}
#if EDVAR_CPP_CORE_USE_AVX2
simd_256d& simd_256d::operator=(const __m256d& other) {
    this->data = other;
    return *this;
}
#endif

simd_256d simd_256d::max(const simd_256d& lhs, const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_max_pd(lhs.data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_max_pd(lhs.halves[0], rhs.halves[0]);
    result.halves[1] = _mm_max_pd(lhs.halves[1], rhs.halves[1]);
    return result;
#endif
}
simd_256d simd_256d::min(const simd_256d& lhs, const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_min_pd(lhs.data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_min_pd(lhs.halves[0], rhs.halves[0]);
    result.halves[1] = _mm_min_pd(lhs.halves[1], rhs.halves[1]);
    return result;
#endif
}

simd_256d simd_256d::sqrt() const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_sqrt_pd(this->data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_sqrt_pd(this->halves[0]);
    result.halves[1] = _mm_sqrt_pd(this->halves[1]);
    return result;
#endif
}
simd_256d& simd_256d::sqrt_inline() {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_sqrt_pd(this->data);
#else
    this->halves[0] = _mm_sqrt_pd(this->halves[0]);
    this->halves[1] = _mm_sqrt_pd(this->halves[1]);
#endif
    return *this;
}

simd_256d simd_256d::operator+(const simd_256d& rhs) const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_add_pd(this->data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_add_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_add_pd(this->halves[1], rhs.halves[1]);
    return result;
#endif
}
simd_256d& simd_256d::operator+=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_add_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_add_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_add_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator-(const simd_256d& rhs) const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_sub_pd(this->data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_sub_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_sub_pd(this->halves[1], rhs.halves[1]);
    return result;
#endif
}
simd_256d& simd_256d::operator-=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_sub_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_sub_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_sub_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator*(const simd_256d& rhs) const {
    simd_256d result;
#if EDVAR_CPP_CORE_USE_AVX2
    result.data = _mm256_mul_pd(this->data, rhs.data);
#else
    result.halves[0] = _mm_mul_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_mul_pd(this->halves[1], rhs.halves[1]);
#endif
    return result;
}
simd_256d& simd_256d::operator*=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_mul_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_mul_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_mul_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator/(const simd_256d& rhs) const {
    simd_256d result;
#if EDVAR_CPP_CORE_USE_AVX2
    result.data = _mm256_div_pd(this->data, rhs.data);
#else
    result.halves[0] = _mm_div_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_div_pd(this->halves[1], rhs.halves[1]);
#endif
    return result;
}
simd_256d& simd_256d::operator/=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_div_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_div_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_div_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator&(const simd_256d& rhs) const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_and_pd(this->data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_and_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_and_pd(this->halves[1], rhs.halves[1]);
    return result;
#endif
}
simd_256d& simd_256d::operator&=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_and_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_and_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_and_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator|(const simd_256d& rhs) const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_or_pd(this->data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_or_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_or_pd(this->halves[1], rhs.halves[1]);
    return result;
#endif
}
simd_256d& simd_256d::operator|=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_or_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_or_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_or_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator^(const simd_256d& rhs) const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_xor_pd(this->data, rhs.data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_xor_pd(this->halves[0], rhs.halves[0]);
    result.halves[1] = _mm_xor_pd(this->halves[1], rhs.halves[1]);
    return result;
#endif
}
simd_256d& simd_256d::operator^=(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_xor_pd(this->data, rhs.data);
#else
    this->halves[0] = _mm_xor_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_xor_pd(this->halves[1], rhs.halves[1]);
#endif
    return *this;
}
simd_256d simd_256d::operator!() const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_xor_pd(this->data, _mm256_set1_pd(-1.0)).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_xor_pd(this->halves[0], _mm_set1_pd(-1.0));
    result.halves[1] = _mm_xor_pd(this->halves[1], _mm_set1_pd(-1.0));
    return result;
#endif
}
simd_256d simd_256d::operator-() const {
#if EDVAR_CPP_CORE_USE_AVX2
    return simd_256d(_mm256_sub_pd(_mm256_setzero_pd(), this->data).m256d_f64);
#else
    simd_256d result;
    result.halves[0] = _mm_sub_pd(_mm_setzero_pd(), this->halves[0]);
    result.halves[1] = _mm_sub_pd(_mm_setzero_pd(), this->halves[1]);
    return result;
#endif
}
bool simd_256d::operator==(const simd_256d& rhs) const {
#if EDVAR_CPP_CORE_USE_AVX2
    __m256d cmp = _mm256_cmp_pd(this->data, rhs.data, _CMP_EQ_OQ);
    union {
        __m256d m256d;
        double m256d_f64[4];
    } cmp_union;
    cmp_union.m256d = cmp;
    return cmp_union.m256d_f64[0] != 0.0 && cmp_union.m256d_f64[1] != 0.0 && cmp_union.m256d_f64[2] != 0.0 &&
           cmp_union.m256d_f64[3] != 0.0;
#else
    __m128d cmp1 = _mm_cmpeq_pd(this->halves[0], rhs.halves[0]);
    __m128d cmp2 = _mm_cmpeq_pd(this->halves[1], rhs.halves[1]);
    union helper {
        __m128d m;
        double f[2];
    };
    helper h1, h2;
    h1.m = cmp1;
    h2.m = cmp2;
    return h1.f[0] != 0.0 && h1.f[1] != 0.0 && h2.f[0] != 0.0 && h2.f[1] != 0.0;
#endif
}
bool simd_256d::operator!=(const simd_256d& rhs) const { return !(*this == rhs); }

simd_256d simd_256d::horizontal_add(const simd_256d& rhs) const {
    simd_256d result = *this;
    result.horizontal_add_inline(rhs);
    return result;
}
simd_256d& simd_256d::horizontal_add_inline(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_hadd_pd(this->data, rhs.data);

#elif EDVAR_CPP_CORE_USE_SSE4_2
    this->halves[0] = _mm_hadd_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_hadd_pd(this->halves[1], rhs.halves[1]);
#else
    x = x + y;
    y = z + w;
    z = rhs.x + rhs.y;
    w = rhs.z + rhs.w;
#endif
    return *this;
}

simd_256d simd_256d::horizontal_sub(const simd_256d& rhs) const {
    simd_256d result = *this;
    result.horizontal_sub_inline(rhs);
    return result;
}
simd_256d& simd_256d::horizontal_sub_inline(const simd_256d& rhs) {
#if EDVAR_CPP_CORE_USE_AVX2
    this->data = _mm256_hsub_pd(this->data, rhs.data);
#elif EDVAR_CPP_CORE_USE_SSE4_2
    this->halves[0] = _mm_hsub_pd(this->halves[0], rhs.halves[0]);
    this->halves[1] = _mm_hsub_pd(this->halves[1], rhs.halves[1]);
#else
    x = x - y;
    y = z - w;
    z = rhs.x - rhs.y;
    w = rhs.z - rhs.w;
#endif
    return *this;
}

simd_256d simd_256d::cross(const simd_256d& other) const {
    simd_256d result = *this;
    result.cross_inline(other);
    return result;
}
simd_256d& simd_256d::cross_inline(const simd_256d& other) {
#if EDVAR_CPP_CORE_USE_AVX2
    simd_256d a_yzx = this->shuffle<1, 0, 2, 3>();
    simd_256d b_yzx = other.shuffle<1, 0, 2, 3>();
    simd_256d c = ((*this) * b_yzx) - (a_yzx * other);
    this->data = c.shuffle<1, 2, 0, 3>().data;
#else
    double tx = y * other.z - z * other.y;
    double ty = z * other.x - x * other.z;
    double tz = x * other.y - y * other.x;
    this->x = tx;
    this->y = ty;
    this->z = tz;
#endif
    return *this;
}

double simd_128d::dot(const simd_128d& other) const {
#if EDVAR_CPP_CORE_USE_SSE4_2
    __m128d result_data = _mm_dp_pd(data, other.data, 0x31);
    return _mm_cvtsd_f64(result_data);
#else
    simd_128d mul = (*this) * other;
    mul.horizontal_add_inline(mul);
    mul.horizontal_add_inline(mul);
    return mul.x;
#endif
}

float simd_256d::dot(const simd_256d& other) const {
    // no avx _mm256_dp_pd intrinsic, fallback to mul + horizontal add
    simd_256d mul = (*this) * other;
    mul.horizontal_add_inline(mul);
    mul.horizontal_add_inline(mul);
    return mul.x;
}
} // namespace edvar::math::simd