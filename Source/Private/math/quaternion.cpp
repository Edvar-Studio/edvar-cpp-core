#pragma once

#include "math/quaternion.h"
#include "math/matrix.h"
namespace edvar::math {
quaterniond::quaterniond() : _data{0.0, 0.0, 0.0, 1.0} {}
quaterniond::quaterniond(const double all) : _data{all, all, all, all} {}
quaterniond::quaterniond(const double x, const double y, const double z, const double w) : _data{x, y, z, w} {}
quaterniond::quaterniond(const simd::simd_256d& xyzw_simd) : _xyzw(xyzw_simd) {}
quaterniond::quaterniond(const double* to_place_data)
    : _data{to_place_data[0], to_place_data[1], to_place_data[2], to_place_data[3]} {}
quaterniond::quaterniond(const std::initializer_list<double>& list) : quaterniond(list.begin()) {}
quaterniond::~quaterniond() { memory::zero_bytes(this->_data, sizeof(this->_data)); }
quaterniond::quaterniond(const quaterniond& other) : _xyzw(other._xyzw) {}
quaterniond::quaterniond(quaterniond&& other) noexcept : _xyzw(other._xyzw) {
    memory::zero_bytes(other._data, sizeof(other._data));
}
quaterniond& quaterniond::operator=(const quaterniond& rhs) {
    this->_xyzw = rhs._xyzw;
    return *this;
}
quaterniond& quaterniond::operator=(quaterniond&& rhs) noexcept {
    this->_xyzw = rhs._xyzw;
    memory::zero_bytes(rhs._data, sizeof(rhs._data));
    return *this;
}
quaterniond quaterniond::operator+(const quaterniond& rhs) const { return quaterniond(this->_xyzw + rhs._xyzw); }
quaterniond& quaterniond::operator+=(const quaterniond& rhs) {
    this->_xyzw += rhs._xyzw;
    return *this;
}
quaterniond quaterniond::operator-(const quaterniond& rhs) const { return quaterniond(this->_xyzw - rhs._xyzw); }
quaterniond& quaterniond::operator-=(const quaterniond& rhs) {
    this->_xyzw -= rhs._xyzw;
    return *this;
}

quaterniond quaterniond::operator*(const quaterniond& rhs) const {
    quaterniond result = *this;
    result *= rhs;
    return result;
}
quaterniond& quaterniond::operator*=(const quaterniond& rhs) {
    const simd::simd_256d wwww = simd::simd_256d(this->w());
    const simd::simd_256d xxxx = simd::simd_256d(this->x());
    const simd::simd_256d yyyy = simd::simd_256d(this->y());
    const simd::simd_256d zzzz = simd::simd_256d(this->z());

    const simd::simd_256d rw = rhs._xyzw * wwww;
    const simd::simd_256d rx = rhs._xyzw.shuffle<3, 0, 1, 2>() * xxxx;
    const simd::simd_256d ry = rhs._xyzw.shuffle<2, 3, 0, 1>() * yyyy;
    const simd::simd_256d rz = rhs._xyzw.shuffle<1, 2, 3, 0>() * zzzz;

    this->_xyzw = rw + rx.shuffle<0, 1, 3, 2>() + ry.shuffle<1, 0, 2, 3>() + rz.shuffle<2, 3, 0, 1>();
    return *this;
}

quaterniond quaterniond::operator*(const double rhs) const { return quaterniond(this->_xyzw * simd::simd_256d(rhs)); }
quaterniond& quaterniond::operator*=(const double rhs) {
    this->_xyzw *= simd::simd_256d(rhs);
    return *this;
}
quaterniond quaterniond::operator/(const double rhs) const { return quaterniond(this->_xyzw / simd::simd_256d(rhs)); }
quaterniond& quaterniond::operator/=(const double rhs) {
    this->_xyzw /= simd::simd_256d(rhs);
    return *this;
}

bool quaterniond::operator==(const quaterniond& rhs) const { return this->_xyzw == rhs._xyzw; }
bool quaterniond::operator!=(const quaterniond& rhs) const { return !(*this == rhs); }

double quaterniond::length() const { return math::square_root(this->length_squared()); }
double quaterniond::length_squared() const {
    simd::simd_256d sq = this->_xyzw * this->_xyzw;
    return sq.x + sq.y + sq.z + sq.w;
}

quaterniond quaterniond::normalized() const {
    quaterniond result = *this;
    result.normalize_inline();
    return result;
}
quaterniond& quaterniond::normalize_inline() {
    const double len = length();
    if (len <= 0.0) {
        this->_xyzw = simd::simd_256d(0.0, 0.0, 0.0, 1.0);
    } else {
        this->_xyzw /= simd::simd_256d(len);
    }
    return *this;
}

quaterniond quaterniond::conjugate() const { return quaterniond(-x(), -y(), -z(), w()); }

quaterniond quaterniond::inverse() const {
    quaterniond result = *this;
    result.inverse_inline();
    return result;
}
quaterniond& quaterniond::inverse_inline() {
    const double len_sq = length_squared();
    if (len_sq <= 0.0) {
        this->_xyzw = simd::simd_256d(0.0, 0.0, 0.0, 1.0);
    } else {
        const double inv_len_sq = 1.0 / len_sq;
        this->_xyzw = this->conjugate()._xyzw * simd::simd_256d(inv_len_sq);
    }
    return *this;
}

quaterniond quaterniond::negate() const { return quaterniond(-x(), -y(), -z(), -w()); }
quaterniond& quaterniond::negate_inline() {
    this->_xyzw = -this->_xyzw;
    return *this;
}

matrix4x4d quaterniond::to_rotation_matrix() const {
    quaterniond normalized_this = normalized();
    const simd::simd_256d xxyyzzww = normalized_this._xyzw * normalized_this._xyzw;
    const double xx = xxyyzzww.x;
    const double yy = xxyyzzww.y;
    const double zz = xxyyzzww.z;

    const simd::simd_256d xxxyxzxw = normalized_this._xyzw * normalized_this._xyzw.shuffle<0, 0, 0, 0>();
    const double xy = xxxyxzxw.y;
    const double xz = xxxyxzxw.z;
    const double xw = xxxyxzxw.w;

    const simd::simd_128d yzyw = simd::simd_128d(reinterpret_cast<const double*>(&normalized_this._xyzw.halves[1])) *
                                 simd::simd_128d(normalized_this.y());
    const double yz = yzyw.x; // z: remapped to x
    const double yw = yzyw.y; // w: remapped to y

    const double wz = normalized_this.w() * normalized_this.z(); // scalar multiply for single data

    return matrix4x4d{1.0 - 2.0 * (yy + zz),
                      2.0 * (xy - wz),
                      2.0 * (xz + yw),
                      0.0,
                      2.0 * (xy + wz),
                      1.0 - 2.0 * (xx + zz),
                      2.0 * (yz - xw),
                      0.0,
                      2.0 * (xz - yw),
                      2.0 * (yz + xw),
                      1.0 - 2.0 * (xx + yy),
                      0.0,
                      0.0,
                      0.0,
                      0.0,
                      1.0};
}
} // namespace edvar::math