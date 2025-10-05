#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"

namespace edvar::math {
vector3d vector3d::zero() { return vector3d(0.0, 0.0, 0.0); }
vector3d vector3d::one() { return vector3d(1.0, 1.0, 1.0); }
vector3d vector3d::up() { return vector3d(0, 0, 1); }
vector3d vector3d::right() { return vector3d(0, 1, 0); }
vector3d vector3d::forward() { return vector3d(1, 0, 0); }

vector3d::vector3d() : _xyzw(0.0, 0.0, 0.0, 0.0) {}
vector3d::vector3d(const double all) : _xyzw(all, all, all, 0) {}
vector3d::vector3d(const simd::simd_128d& xy, const double z) : _xyzw(xy.x, xy.y, z, 0.0) {}
vector3d::vector3d(const double in_x, const double in_y, const double in_z) : _xyzw(in_x, in_y, in_z, 0.0) {}
vector3d::vector3d(const double* to_place_data) : _xyzw(to_place_data[0], to_place_data[1], to_place_data[2], 0.0) {}
vector3d::vector3d(const std::initializer_list<double>& list) : _xyzw(0.0, 0.0, 0.0, 0.0) {
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 3; ++it, ++i) {
        if (i == 0)
            _xyzw.x = *it;
        else if (i == 1)
            _xyzw.y = *it;
        else if (i == 2)
            _xyzw.z = *it;
    }
}
vector3d::~vector3d() { memory::zero_bytes(this->_elements, sizeof(this->_elements)); }
vector3d::vector3d(const vector3d& other) : _xyzw(other._xyzw) {}
vector3d::vector3d(vector3d&& other) noexcept : _xyzw(other._xyzw) {
    memory::zero_bytes(other._elements, sizeof(other._elements));
}
vector3d& vector3d::operator=(const vector3d& rhs) {
    this->_xyzw = rhs._xyzw;
    return *this;
}
vector3d& vector3d::operator=(vector3d&& rhs) noexcept {
    this->_xyzw = rhs._xyzw;
    memory::zero_bytes(rhs._elements, sizeof(rhs._elements));
    return *this;
}
vector3d vector3d::operator+(const vector3d& rhs) const { return vector3d(this->_xyzw + rhs._xyzw); }
vector3d& vector3d::operator+=(const vector3d& rhs) {
    this->_xyzw += rhs._xyzw;
    return *this;
}
vector3d vector3d::operator-(const vector3d& rhs) const { return vector3d(this->_xyzw - rhs._xyzw); }
vector3d& vector3d::operator-=(const vector3d& rhs) {
    this->_xyzw -= rhs._xyzw;
    return *this;
}
vector3d vector3d::operator*(const vector3d& rhs) const { return vector3d(this->_xyzw * rhs._xyzw); }
vector3d& vector3d::operator*=(const vector3d& rhs) {
    this->_xyzw *= rhs._xyzw;
    return *this;
}
vector3d vector3d::operator*(const double rhs) const { return vector3d(this->_xyzw * simd::simd_256d(rhs)); }
vector3d& vector3d::operator*=(const double rhs) {
    this->_xyzw *= simd::simd_256d(rhs);
    return *this;
}
vector3d vector3d::operator/(const vector3d& rhs) const { return vector3d(this->_xyzw / rhs._xyzw); }
vector3d& vector3d::operator/=(const vector3d& rhs) {
    this->_xyzw /= rhs._xyzw;
    return *this;
}
vector3d vector3d::operator/(const double& rhs) const { return vector3d(this->_xyzw / simd::simd_256d(rhs)); }
vector3d& vector3d::operator/=(const double& rhs) {
    this->_xyzw /= simd::simd_256d(rhs);
    return *this;
}
bool vector3d::operator==(const vector3d& rhs) const { return this->_xyzw == rhs._xyzw; }
bool vector3d::operator!=(const vector3d& rhs) const { return !(*this == rhs); }
bool vector3d::operator<(const vector3d& rhs) const {
    if (x() < rhs.x())
        return true;
    if (x() > rhs.x())
        return false;
    if (y() < rhs.y())
        return true;
    if (y() > rhs.y())
        return false;
    return z() < rhs.z();
}
bool vector3d::operator<=(const vector3d& rhs) const { return *this < rhs || *this == rhs; }
bool vector3d::operator>(const vector3d& rhs) const { return !(*this <= rhs); }
bool vector3d::operator>=(const vector3d& rhs) const { return !(*this < rhs); }

double vector3d::length() const { return math::square_root(length_squared()); }
double vector3d::length_squared() const { return _xyzw.x * _xyzw.x + _xyzw.y * _xyzw.y + _xyzw.z * _xyzw.z; }

bool vector3d::is_zero(const double tolerance) const {
    return math::abs(x()) <= tolerance && math::abs(y()) <= tolerance && math::abs(z()) <= tolerance;
}

vector3d vector3d::abs() const { return vector3d(math::abs(x()), math::abs(y()), math::abs(z())); }

double vector3d::distance(const vector3d& to) const { return distance(*this, to); }
double vector3d::distance_squared(const vector3d& to) const { return distance_squared(*this, to); }

double vector3d::distance(const vector3d& from, const vector3d& to) { return (from - to).length(); }
double vector3d::distance_squared(const vector3d& from, const vector3d& to) { return (from - to).length_squared(); }

vector3d vector3d::look_at(const vector3d& target) const { return (target - *this).normalized(); }

vector3d vector3d::normalized(const double tolerance) const {
    const double len = length();
    if (len <= tolerance) {
        return vector3d::zero();
    }
    return *this / len;
}
vector3d& vector3d::normalize_inline(const double tolerance) {
    const double len = length();
    if (len <= tolerance) {
        this->_xyzw = simd::simd_256d(0.0);
    } else {
        *this /= len;
    }
    return *this;
}

vector3d vector3d::cross(const vector3d& to) const { return vector3d(_xyzw.cross(to._xyzw)); }

double vector3d::dot(const vector3d& to) const {
    simd::simd_256d multiplied(_xyzw * to._xyzw);
    return multiplied.x + multiplied.y + multiplied.z;
}

vector3d vector3d::project(const vector3d& onto) const {
    const double onto_len_sq = onto.length_squared();
    if (onto_len_sq < 1e-6) {
        return vector3d::zero();
    }
    const double projection_scale = this->dot(onto) / onto_len_sq;
    return onto * projection_scale;
}
vector3d vector3d::reflect(const vector3d& normal) const {
    // R = V - 2 * (V · N) * N
    const double dot_product = this->dot(normal);
    return *this - normal * (2.0 * dot_product);
}
vector3d vector3d::refract(const vector3d& normal, const double eta) const {
    // Using Snell's law to calculate refraction
    const double dot_product = this->dot(normal);
    const double k = 1.0 - eta * eta * (1.0 - dot_product * dot_product);
    if (k < 0.0) {
        return vector3d::zero(); // Total internal reflection
    } else {
        return *this * eta - normal * (eta * dot_product + math::square_root(k));
    }
}

quaterniond vector3d::to_quaternion() const { return quaterniond::from_rotation_matrix(rot); }

matrix4x4d vector3d::to_translation_matrix() const {
    return matrix4x4d(1.0, 0.0, 0.0, x(), 0.0, 1.0, 0.0, y(), 0.0, 0.0, 1.0, z(), 0.0, 0.0, 0.0, 1.0);
}

matrix4x4d vector3d::to_rotation_matrix() const {
    // Convert the vector to a quaternion with w = 0
    vector3d f = this->normalized();
    vector3d r = vector3d::up().cross(f).normalized();
    vector3d u = f.cross(r);
    matrix4x4d rot(f, r, u, vector3d::zero());
    return rot;
}

matrix4x4d vector3d::to_scale_matrix() const {
    return matrix4x4d(x(), 0.0, 0.0, 0.0, 0.0, y(), 0.0, 0.0, 0.0, 0.0, z(), 0.0, 0.0, 0.0, 0.0, 1.0);
}
} // namespace edvar::math