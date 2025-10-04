#pragma once
#include <intrin.h>

#ifndef EDVAR_CPP_CORE_USE_SSE4_2
#    define EDVAR_CPP_CORE_USE_SSE4_2 1
#endif

namespace edvar::math::simd {

/**
 * 128-bit SIMD wrapper for 2 doubles.
 */
struct alignas(16) simd_128d {
    union alignas(16) {
        __m128d data;
        double v[2];
        struct {
            double x, y;
        };
    };

    simd_128d();
    simd_128d(const double all);
    simd_128d(const double x, const double y);
    simd_128d(const double* to_place_data);
    simd_128d(const std::initializer_list<double>& list);
    ~simd_128d();

    simd_128d(const simd_128d& other) : data(other.data) {}
    simd_128d(simd_128d&& other) noexcept : data(other.data) {
        // clear the other data.
        memory::zero_bytes(other.v, sizeof(other.v));
    }

    inline simd_128d& operator=(const simd_128d& rhs) {
        this->data = rhs.data;
        return *this;
    }
    inline simd_128d& operator=(simd_128d&& rhs) noexcept {
        this->data = rhs.data;
        // clear the rhs's data.
        memory::zero_bytes(rhs.v, sizeof(rhs.v));
        return *this;
    }
    simd_128d& operator=(const double* to_place_data);
    simd_128d& operator=(const std::initializer_list<double>& list);
    simd_128d& operator=(const double all);

    simd_128d operator+(const simd_128d& rhs) const;
    simd_128d& operator+=(const simd_128d& rhs);
    simd_128d operator-(const simd_128d& rhs) const;
    simd_128d& operator-=(const simd_128d& rhs);
    simd_128d operator*(const simd_128d& rhs) const;
    simd_128d& operator*=(const simd_128d& rhs);
    simd_128d operator/(const simd_128d& rhs) const;
    simd_128d& operator/=(const simd_128d& rhs);

    simd_128d operator&(const simd_128d& rhs) const;
    simd_128d& operator&=(const simd_128d& rhs);
    simd_128d operator|(const simd_128d& rhs) const;
    simd_128d& operator|=(const simd_128d& rhs);
    simd_128d operator^(const simd_128d& rhs) const;
    simd_128d& operator^=(const simd_128d& rhs);
    simd_128d operator!() const;
    simd_128d operator-() const;

    bool operator==(const simd_128d& rhs) const;
    bool operator!=(const simd_128d& rhs) const;

    inline double& operator[](uint32 index) { return this->v[index]; }
    inline const double& operator[](uint32 index) const { return this->v[index]; }

    static simd_128d max(const simd_128d& lhs, const simd_128d& rhs);
    static simd_128d min(const simd_128d& lhs, const simd_128d& rhs);

    simd_128d sqrt() const;
    simd_128d& sqrt_inline();

    enum class shuffle_mode : int32 { xy_yx = 1, yx_xy = 2, xx_yy = 0, yy_xx = 3 };

    simd_128d shuffle(const simd_128d& other, const shuffle_mode mode = shuffle_mode::xy_yx) const;
    simd_128d& shuffle_inline(const simd_128d& other, const shuffle_mode mode = shuffle_mode::xy_yx);

    simd_128d horizontal_add(const simd_128d& rhs) const;
    simd_128d& horizontal_add_inline(const simd_128d& rhs);

    simd_128d horizontal_sub(const simd_128d& rhs) const;
    simd_128d& horizontal_sub_inline(const simd_128d& rhs);
};

/**
 * 128-bit SIMD wrapper for 4 floats.
 */
struct alignas(16) simd_128f {
    union alignas(16) {
        __m128 data;
        float v[4];
        struct {
            float x, y, z, w;
        };
    };
    simd_128f();
    simd_128f(const float all);
    simd_128f(const float x, const float y, const float z, const float w);
    simd_128f(const float* to_place_data);
    simd_128f(const std::initializer_list<float>& list);
    ~simd_128f();

    simd_128f(const simd_128f& other);
    simd_128f(simd_128f&& other) noexcept;

    simd_128f& operator=(const simd_128f& rhs);
    simd_128f& operator=(simd_128f&& rhs) noexcept;
    simd_128f& operator=(const float* to_place_data);
    simd_128f& operator=(const std::initializer_list<float>& list);
    simd_128f& operator=(const float all);

    simd_128f operator+(const simd_128f& rhs) const;
    simd_128f& operator+=(const simd_128f& rhs);
    simd_128f operator-(const simd_128f& rhs) const;
    simd_128f& operator-=(const simd_128f& rhs);
    simd_128f operator*(const simd_128f& rhs) const;
    simd_128f& operator*=(const simd_128f& rhs);
    simd_128f operator/(const simd_128f& rhs) const;
    simd_128f& operator/=(const simd_128f& rhs);

    simd_128f operator&(const simd_128f& rhs) const;
    simd_128f& operator&=(const simd_128f& rhs);
    simd_128f operator|(const simd_128f& rhs) const;
    simd_128f& operator|=(const simd_128f& rhs);
    simd_128f operator^(const simd_128f& rhs) const;
    simd_128f& operator^=(const simd_128f& rhs);
    simd_128f operator!() const;
    simd_128f operator-() const;

    bool operator==(const simd_128f& rhs) const;
    bool operator!=(const simd_128f& rhs) const;

    inline float& operator[](uint32 index) { return this->v[index]; }
    inline const float& operator[](uint32 index) const { return this->v[index]; }

    static simd_128f max(const simd_128f& lhs, const simd_128f& rhs);
    static simd_128f min(const simd_128f& lhs, const simd_128f& rhs);

    simd_128f sqrt() const;
    simd_128f& sqrt_inline();

    enum class shuffle_mode : int32 {
        xyzw_wzyx = 0,
        xxyy_zzww = 1,
        yxwz_zwxy = 2,
    };

    simd_128f shuffle(const simd_128f& other, const shuffle_mode mode = shuffle_mode::xyzw_wzyx) const;
    simd_128f& shuffle_inline(const simd_128f& other, const shuffle_mode mode = shuffle_mode::xyzw_wzyx);

    simd_128f horizontal_add(const simd_128f& rhs) const;
    simd_128f& horizontal_add_inline(const simd_128f& rhs);

    simd_128f horizontal_sub(const simd_128f& rhs) const;
    simd_128f& horizontal_sub_inline(const simd_128f& rhs);
};

/**
 * 128-bit SIMD wrapper for 4 int32.
 */
struct alignas(16) simd_128i {
    union alignas(16) {
        __m128i data;
        int32 v[4];
        struct {
            int32 x, y, z, w;
        };
    };

    simd_128i();
    simd_128i(const int32 all);
    simd_128i(const int32 x, const int32 y, const int32 z, const int32 w);
    simd_128i(const int32* to_place_data);
    simd_128i(const std::initializer_list<int32>& list);
    ~simd_128i();

    simd_128i(const simd_128i& other);
    simd_128i(simd_128i&& other) noexcept;

    simd_128i& operator=(const simd_128i& rhs);
    simd_128i& operator=(simd_128i&& rhs) noexcept;
    simd_128i& operator=(const int32* to_place_data);
    simd_128i& operator=(const std::initializer_list<int32>& list);
    simd_128i& operator=(const int32 all);

    simd_128i operator+(const simd_128i& rhs) const;
    simd_128i& operator+=(const simd_128i& rhs);
    simd_128i operator-(const simd_128i& rhs) const;
    simd_128i& operator-=(const simd_128i& rhs);
    simd_128i operator*(const simd_128i& rhs) const;
    simd_128i& operator*=(const simd_128i& rhs);
    simd_128i operator/(const simd_128i& rhs) const;
    simd_128i& operator/=(const simd_128i& rhs);

    simd_128i operator&(const simd_128i& rhs) const;
    simd_128i& operator&=(const simd_128i& rhs);
    simd_128i operator|(const simd_128i& rhs) const;
    simd_128i& operator|=(const simd_128i& rhs);
    simd_128i operator^(const simd_128i& rhs) const;
    simd_128i& operator^=(const simd_128i& rhs);
    simd_128i operator!() const;
    simd_128i operator-() const;

    bool operator==(const simd_128i& rhs) const;
    bool operator!=(const simd_128i& rhs) const;

    inline int32& operator[](uint32 index) { return this->v[index]; }
    inline const int32& operator[](uint32 index) const { return this->v[index]; }

    static simd_128i max(const simd_128i& lhs, const simd_128i& rhs);
    static simd_128i min(const simd_128i& lhs, const simd_128i& rhs);

    simd_128i sqrt() const;
    simd_128i& sqrt_inline();

    enum class shuffle_mode : int32 {
        xyzw_wzyx = 0,
        xxyy_zzww = 1,
        yxwz_zwxy = 2,
    };

    simd_128i shuffle(const simd_128i& other, const shuffle_mode mode = shuffle_mode::xyzw_wzyx) const;
    simd_128i& shuffle_inline(const simd_128i& other, const shuffle_mode mode = shuffle_mode::xyzw_wzyx);
};

/**
 * 128-bit SIMD wrapper for 2 int64.
 */
struct alignas(16) simd_128l {
    union alignas(16) {
        __m128i data;
        int64 v[2];
        struct {
            int64 x, y;
        };
    };
    simd_128l();
    simd_128l(const int64 all);
    simd_128l(const int64 x, const int64 y);
    simd_128l(const int64* to_place_data);
    simd_128l(const std::initializer_list<int64>& list);
    ~simd_128l();

    simd_128l(const simd_128l& other);
    simd_128l(simd_128l&& other) noexcept;

    simd_128l& operator=(const simd_128l& rhs);
    simd_128l& operator=(simd_128l&& rhs) noexcept;
    simd_128l& operator=(const int64* to_place_data);
    simd_128l& operator=(const std::initializer_list<int64>& list);
    simd_128l& operator=(const int64 all);

    simd_128l operator+(const simd_128l& rhs) const;
    simd_128l& operator+=(const simd_128l& rhs);
    simd_128l operator-(const simd_128l& rhs) const;
    simd_128l& operator-=(const simd_128l& rhs);
    simd_128l operator*(const simd_128l& rhs) const;
    simd_128l& operator*=(const simd_128l& rhs);
    simd_128l operator/(const simd_128l& rhs) const;
    simd_128l& operator/=(const simd_128l& rhs);

    simd_128l operator&(const simd_128l& rhs) const;
    simd_128l& operator&=(const simd_128l& rhs);
    simd_128l operator|(const simd_128l& rhs) const;
    simd_128l& operator|=(const simd_128l& rhs);
    simd_128l operator^(const simd_128l& rhs) const;
    simd_128l& operator^=(const simd_128l& rhs);
    simd_128l operator!() const;
    simd_128l operator-() const;

    bool operator==(const simd_128l& rhs) const;
    bool operator!=(const simd_128l& rhs) const;

    inline int64& operator[](uint32 index) { return this->v[index]; }
    inline const int64& operator[](uint32 index) const { return this->v[index]; }

    static simd_128l max(const simd_128l& lhs, const simd_128l& rhs);
    static simd_128l min(const simd_128l& lhs, const simd_128l& rhs);

    simd_128l sqrt() const;
    simd_128l& sqrt_inline();

    enum class shuffle_mode : int32 { xy_yx = 1, yx_xy = 2, xx_yy = 0, yy_xx = 3 };

    simd_128l shuffle(const simd_128l& other, const shuffle_mode mode = shuffle_mode::xy_yx) const;
    simd_128l& shuffle_inline(const simd_128l& other, const shuffle_mode mode = shuffle_mode::xy_yx);
};

/**
 * 128-bit SIMD wrapper for 4 uint32.
 */
struct alignas(16) simd_128ui {
    union alignas(16) {
        __m128i data;
        uint32 v[4];
        struct {
            uint32 x, y, z, w;
        };
    };
    simd_128ui();
    simd_128ui(const uint32 all);
    simd_128ui(const uint32 x, const uint32 y, const uint32 z, const uint32 w);
    simd_128ui(const uint32* to_place_data);
    simd_128ui(const std::initializer_list<uint32>& list);
    ~simd_128ui();

    simd_128ui(const simd_128ui& other);
    simd_128ui(simd_128ui&& other) noexcept;

    simd_128ui& operator=(const simd_128ui& rhs);
    simd_128ui& operator=(simd_128ui&& rhs) noexcept;
    simd_128ui& operator=(const uint32* to_place_data);
    simd_128ui& operator=(const std::initializer_list<uint32>& list);
    simd_128ui& operator=(const uint32 all);

    simd_128ui operator+(const simd_128ui& rhs) const;
    simd_128ui& operator+=(const simd_128ui& rhs);
    simd_128ui operator-(const simd_128ui& rhs) const;
    simd_128ui& operator-=(const simd_128ui& rhs);
    simd_128ui operator*(const simd_128ui& rhs) const;
    simd_128ui& operator*=(const simd_128ui& rhs);
    simd_128ui operator/(const simd_128ui& rhs) const;
    simd_128ui& operator/=(const simd_128ui& rhs);

    simd_128ui operator&(const simd_128ui& rhs) const;
    simd_128ui& operator&=(const simd_128ui& rhs);
    simd_128ui operator|(const simd_128ui& rhs) const;
    simd_128ui& operator|=(const simd_128ui& rhs);
    simd_128ui operator^(const simd_128ui& rhs) const;
    simd_128ui& operator^=(const simd_128ui& rhs);
    simd_128ui operator!() const;
    simd_128ui operator-() const;

    bool operator==(const simd_128ui& rhs) const;
    bool operator!=(const simd_128ui& rhs) const;

    inline uint32& operator[](uint32 index) { return this->v[index]; }
    inline const uint32& operator[](uint32 index) const { return this->v[index]; }

    static simd_128ui max(const simd_128ui& lhs, const simd_128ui& rhs);
    static simd_128ui min(const simd_128ui& lhs, const simd_128ui& rhs);

    simd_128ui sqrt() const;
    simd_128ui& sqrt_inline();

    struct shuffle_opts {
        enum use_mode { use_x = 0, use_y = 1, use_z = 2, use_w = 3 };
        use_mode x;
        use_mode y;
        use_mode z;
        use_mode w;

        int32 to_int32() const {
            return (static_cast<int32>(x) & 0x3) | ((static_cast<int32>(y) & 0x3) << 2) |
                   ((static_cast<int32>(z) & 0x3) << 4) | ((static_cast<int32>(w) & 0x3) << 6);
        }
    };

    simd_128ui shuffle(const shuffle_opts& opts) const;
    simd_128ui& shuffle_inline(const shuffle_opts& opts);

    simd_128ui horizontal_add(const simd_128ui& rhs) const;
    simd_128ui& horizontal_add_inline(const simd_128ui& rhs);

    simd_128ui horizontal_sub(const simd_128ui& rhs) const;
    simd_128ui& horizontal_sub_inline(const simd_128ui& rhs);
};

/**
 * 128-bit SIMD wrapper for 2 uint64.
 */
struct alignas(16) simd_128ul {
    union alignas(16) {
        __m128i data;
        uint64 v[2];
        struct {
            uint64 x, y;
        };
    };
    simd_128ul();
    simd_128ul(const uint64 all);
    simd_128ul(const uint64 x, const uint64 y);
    simd_128ul(const uint64* to_place_data);
    simd_128ul(const std::initializer_list<uint64>& list);
    ~simd_128ul();

    simd_128ul(const simd_128ul& other);
    simd_128ul(simd_128ul&& other) noexcept;

    simd_128ul& operator=(const simd_128ul& rhs);
    simd_128ul& operator=(simd_128ul&& rhs) noexcept;
    simd_128ul& operator=(const uint64* to_place_data);
    simd_128ul& operator=(const std::initializer_list<uint64>& list);
    simd_128ul& operator=(const uint64 all);

    simd_128ul operator+(const simd_128ul& rhs) const;
    simd_128ul& operator+=(const simd_128ul& rhs);
    simd_128ul operator-(const simd_128ul& rhs) const;
    simd_128ul& operator-=(const simd_128ul& rhs);
    simd_128ul operator*(const simd_128ul& rhs) const;
    simd_128ul& operator*=(const simd_128ul& rhs);
    simd_128ul operator/(const simd_128ul& rhs) const;
    simd_128ul& operator/=(const simd_128ul& rhs);

    simd_128ul operator&(const simd_128ul& rhs) const;
    simd_128ul& operator&=(const simd_128ul& rhs);
    simd_128ul operator|(const simd_128ul& rhs) const;
    simd_128ul& operator|=(const simd_128ul& rhs);
    simd_128ul operator^(const simd_128ul& rhs) const;
    simd_128ul& operator^=(const simd_128ul& rhs);
    simd_128ul operator!() const;
    simd_128ul operator-() const;

    bool operator==(const simd_128ul& rhs) const;
    bool operator!=(const simd_128ul& rhs) const;

    inline uint64& operator[](uint32 index) { return this->v[index]; }
    inline const uint64& operator[](uint32 index) const { return this->v[index]; }

    static simd_128ul max(const simd_128ul& lhs, const simd_128ul& rhs);
    static simd_128ul min(const simd_128ul& lhs, const simd_128ul& rhs);

    simd_128ul sqrt() const;
    simd_128ul& sqrt_inline();

    struct shuffle_opts {
        enum use_mode { use_x = 0, use_y = 1 };
        use_mode x;
        use_mode y;

        int32 to_int32() const { return (static_cast<int32>(x) & 0x1) | ((static_cast<int32>(y) & 0x1) << 2); }
    };

    simd_128ul shuffle(const simd_128ul& other, const shuffle_opts& opts) const;
    simd_128ul& shuffle_inline(const simd_128ul& other, const shuffle_opts& opts);

    simd_128ul horizontal_add(const simd_128ul& rhs) const;
    simd_128ul& horizontal_add_inline(const simd_128ul& rhs);

    simd_128ul horizontal_sub(const simd_128ul& rhs) const;
    simd_128ul& horizontal_sub_inline(const simd_128ul& rhs);
};

namespace meta {
template <typename in_type> struct simd_traits {
    static constexpr bool is_simd = false;
    static constexpr size_t element_count = 0;
    using element_type = void;
};

template <> struct simd_traits<simd_128d> {
    static constexpr bool is_simd = true;
    static constexpr size_t element_count = 2;
    using element_type = double;
};
template <> struct simd_traits<simd_128f> {
    static constexpr bool is_simd = true;
    static constexpr size_t element_count = 4;
    using element_type = float;
};
template <> struct simd_traits<simd_128i> {
    static constexpr bool is_simd = true;
    static constexpr size_t element_count = 4;
    using element_type = int32;
};
template <> struct simd_traits<simd_128l> {
    static constexpr bool is_simd = true;
    static constexpr size_t element_count = 2;
    using element_type = int64;
};
template <> struct simd_traits<simd_128ui> {
    static constexpr bool is_simd = true;
    static constexpr size_t element_count = 4;
    using element_type = uint32;
};
template <> struct simd_traits<simd_128ul> {
    static constexpr bool is_simd = true;
    static constexpr size_t element_count = 2;
    using element_type = uint64;
};
} // namespace meta

} // namespace edvar::math::simd