#pragma once
#include "math/simd_support.h"

namespace edvar::math {
struct matrix4x4d;
struct vector3d;

struct quaterniond {

    quaterniond();
    quaterniond(const double all);
    quaterniond(const double x, const double y, const double z, const double w);
    quaterniond(const simd::simd_256d& xyzw_simd);
    quaterniond(const double* to_place_data);
    quaterniond(const std::initializer_list<double>& list);
    ~quaterniond();

    quaterniond(const quaterniond& other);
    quaterniond(quaterniond&& other) noexcept;

    quaterniond& operator=(const quaterniond& rhs);
    quaterniond& operator=(quaterniond&& rhs) noexcept;

    quaterniond operator+(const quaterniond& rhs) const;
    quaterniond& operator+=(const quaterniond& rhs);

    quaterniond operator-(const quaterniond& rhs) const;
    quaterniond& operator-=(const quaterniond& rhs);

    quaterniond operator*(const quaterniond& rhs) const;
    quaterniond& operator*=(const quaterniond& rhs);

    quaterniond operator*(const double rhs) const;
    quaterniond& operator*=(const double rhs);

    quaterniond operator/(const double rhs) const;
    quaterniond& operator/=(const double rhs);

    inline quaterniond operator-() const { return negate(); }
    bool operator==(const quaterniond& rhs) const;
    bool operator!=(const quaterniond& rhs) const;

    inline double& operator[](uint32 index) { return this->_data[index]; }
    inline const double& operator[](uint32 index) const { return this->_data[index]; }

    double length() const;
    double length_squared() const;
    quaterniond normalized() const;
    quaterniond& normalize_inline();
    quaterniond conjugate() const;
    quaterniond inverse() const;
    quaterniond& inverse_inline();
    quaterniond negate() const;
    quaterniond& negate_inline();

    matrix4x4d to_rotation_matrix() const;

    static quaterniond identity();

    static quaterniond from_rotation_matrix(const matrix4x4d& mat);

    inline const double& x() const { return _data[0]; }
    inline const double& y() const { return _data[1]; }
    inline const double& z() const { return _data[2]; }
    inline const double& w() const { return _data[3]; }

private:
    union alignas(32) {
        simd::simd_256d _xyzw;
        double _data[4];
        struct {
            double _x;
            double _y;
            double _z;
            double _w;
        };
    };
};
} // namespace edvar::math