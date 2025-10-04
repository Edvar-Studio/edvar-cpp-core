#pragma once
#include "intrin.h"
#include <emmintrin.h>

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


    enum class shuffle_mode : int32{
        xy_yx = 1,
        yx_xy = 2,
        xx_yy = 0,
        yy_xx = 3
    };

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
    
};

} // namespace edvar::math::simd