#pragma once

#include "math/simd_support.h"
namespace edvar::math {

struct quaterniond;
struct matrix4x4d;

struct vector3d {

    vector3d();
    vector3d(const double all);
    // construct from simd_256d for x, y, z, w. w is ignored in vector3d.
    vector3d(const simd::simd_256d& xyzw_simd);
    // construct from simd_128d for x and y, and double for z.
    vector3d(const simd::simd_128d& xy, const double z);

    vector3d(const double in_x, const double in_y, const double in_z);
    vector3d(const double* to_place_data);
    vector3d(const std::initializer_list<double>& list);

    ~vector3d();

    vector3d(const vector3d& other);
    vector3d(vector3d&& other) noexcept;

    vector3d& operator=(const vector3d& rhs);
    vector3d& operator=(vector3d&& rhs) noexcept;

    vector3d operator+(const vector3d& rhs) const;
    vector3d& operator+=(const vector3d& rhs);

    vector3d operator-(const vector3d& rhs) const;
    vector3d& operator-=(const vector3d& rhs);

    vector3d operator*(const vector3d& rhs) const;
    vector3d& operator*=(const vector3d& rhs);

    vector3d operator*(const double rhs) const;
    vector3d& operator*=(const double rhs);

    vector3d operator/(const vector3d& rhs) const;
    vector3d& operator/=(const vector3d& rhs);

    vector3d operator/(const double& rhs) const;
    vector3d& operator/=(const double& rhs);

    inline vector3d operator-() const { return negate(); }
    bool operator==(const vector3d& rhs) const;
    bool operator!=(const vector3d& rhs) const;

    bool operator<(const vector3d& rhs) const;
    bool operator<=(const vector3d& rhs) const;
    bool operator>(const vector3d& rhs) const;
    bool operator>=(const vector3d& rhs) const;

    inline double& operator[](uint32 index) { return this->_elements[index]; }
    inline const double& operator[](uint32 index) const { return this->_elements[index]; }

    double length() const;
    double length_squared() const;

    bool is_zero(const double tolerance = 1e-6) const;

    vector3d abs() const;

    double distance(const vector3d& to) const;
    static double distance(const vector3d& from, const vector3d& to);

    double distance_squared(const vector3d& to) const;
    static double distance_squared(const vector3d& from, const vector3d& to);

    vector3d look_at(const vector3d& target) const;

    vector3d normalized(const double tolerance = 1e-6) const;
    vector3d& normalize_inline(const double tolerance = 1e-6);

    vector3d cross(const vector3d& rhs) const;
    double dot(const vector3d& rhs) const;

    vector3d project(const vector3d& onto) const;
    vector3d reflect(const vector3d& normal) const;
    vector3d refract(const vector3d& normal, const double eta) const;
    vector3d rotate(const quaternion& rotation) const;
    vector3d& rotate_inline(const quaternion& rotation);
    inline vector3d negate() const { return vector3d(); }

    static vector3d zero();
    static vector3d one();

    static vector3d up();
    static vector3d right();
    static vector3d forward();

    inline double& x() { return _x; }
    inline double& y() { return _y; }
    inline double& z() { return _z; }

    inline const double& x() const { return _x; }
    inline const double& y() const { return _y; }
    inline const double& z() const { return _z; }

    quaterniond to_quaternion() const;

    matrix4x4d to_translation_matrix() const;
    matrix4x4d to_rotation_matrix() const;
    matrix4x4d to_scale_matrix() const;

private:
    union {
        simd::simd_256d _xyzw;
        struct {
            double _x, _y, _z, _w;
        };
        double _elements[4];
    };
};

} // namespace edvar::math