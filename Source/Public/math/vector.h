#pragma once

#include "diagnostics/assertion.h"
namespace edvar::math {
template <typename type> struct vector3;

template <typename type> struct vector2 {
    vector2() : x(0), y(0) {}
    vector2(type all) : x(all), y(all) {}
    vector2(type x, type y) : x(x), y(y) {}
    vector2(const type* to_place_data) : x(to_place_data[0]), y(to_place_data[1]) {}
    vector2(const std::initializer_list<type>& list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : type(0);
        y = (it != list.end()) ? *it++ : type(0);
    }
    ~vector2() = default;
    vector2(const vector2& other) : x(other.x), y(other.y) {}
    vector2(vector2&& other) noexcept : x(edvar::move(other.x)), y(edvar::move(other.y)) {}
    vector2& operator=(const vector2& rhs) {
        if (this != &rhs) {
            x = rhs.x;
            y = rhs.y;
        }
        return *this;
    }
    vector2& operator=(vector2&& rhs) noexcept {
        if (this != &rhs) {
            x = edvar::move(rhs.x);
            y = edvar::move(rhs.y);
        }
        return *this;
    }
    vector2& operator=(const type* to_place_data) {
        x = to_place_data[0];
        y = to_place_data[1];
        return *this;
    }
    vector2& operator=(const std::initializer_list<type>& list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : type(0);
        y = (it != list.end()) ? *it++ : type(0);
        return *this;
    }

    // Conversion from vector3
    template <typename other_type> explicit vector2(const vector3<other_type>& other) {
        x = static_cast<type>(other.x);
        y = static_cast<type>(other.y);
    }
    template <typename other_type> vector2& operator=(const vector3<other_type>& other) {
        x = static_cast<type>(other.x);
        y = static_cast<type>(other.y);
        return *this;
    }

    // Arithmetic operators
    vector2 operator+(const vector2& rhs) const { return vector2(*this) += rhs; }
    vector2& operator+=(const vector2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    vector2 operator-(const vector2& rhs) const { return vector2(*this) -= rhs; }
    vector2& operator-=(const vector2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    vector2 operator*(const vector2& rhs) const { return vector2(*this) *= rhs; }
    vector2& operator*=(const vector2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }
    vector2 operator/(const vector2& rhs) const { return vector2(*this) /= rhs; }
    vector2& operator/=(const vector2& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }
    vector2 operator*(const type scalar) const { return vector2(*this) *= scalar; }
    vector2& operator*=(const type scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    vector2 operator/(const type scalar) const { return vector2(*this) /= scalar; }
    vector2& operator/=(const type scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    // Dot product
    type dot(const vector2& rhs) const { return x * rhs.x + y * rhs.y; }
    type length_squared() const { return this->dot(*this); }
    type length() const { return math::square_root(length_squared()); }
    vector2 normalized() const {
        type len = length();
        if (len == type(0))
            return vector2(0);
        return *this / len;
    }
    vector2& normalize_inline() {
        type len = length();
        if (len == type(0)) {
            x = y = type(0);
        } else {
            *this /= len;
        }
        return *this;
    }
    vector2 mirror(const vector2& normal) const {
        vector2 result = *this;
        result.mirror_inline(normal);
        return result;
    }
    vector2& mirror_inline(const vector2& normal) {
        if (is_normalized()) {
            *this -= normal * (type(2) * this->dot(normal));
            return *this;
        } else {
            vector2 n = normal.normalized();
            *this -= n * (type(2) * this->dot(n));
            return *this;
        }
    }
    vector2 negate() const { return vector2(-x, -y); }
    vector2& negate_inline() {
        x = -x;
        y = -y;
        return *this;
    }
    vector2 abs() const { return vector2(math::abs(x), math::abs(y)); }
    vector2& abs_inline() {
        x = math::abs(x);
        y = math::abs(y);
        return *this;
    }
    vector2 clamp(const vector2& min, const vector2& max) const {
        vector2 result = *this;
        result.clamp_inline(min, max);
        return result;
    }
    vector2& clamp_inline(const vector2& min, const vector2& max) {
        x = math::clamp(x, min.x, max.x);
        y = math::clamp(y, min.y, max.y);
        return *this;
    }

    bool inside_aabb(const vector2& min, const vector2& max) const {
        return x >= min.x && x <= max.x && y >= min.y && y <= max.y;
    }

    bool is_normalized(type epsilon = static_cast<type>(1e-6)) const {
        return math::abs(length_squared() - type(1)) < epsilon;
    }

    vector2& project_inline(const vector2& onto) {
        type onto_len_sq = onto.length_squared();
        if (onto_len_sq == type(0)) {
            x = y = type(0);
        } else {
            type projection_scale = this->dot(onto) / onto_len_sq;
            *this = onto * projection_scale;
        }
        return *this;
    }
    vector2 project(const vector2& onto) const {
        vector2 result = *this;
        result.project_inline(onto);
        return result;
    }

    vector2& rotate_inline(type angle_radians) {
        double cos_angle = math::cos(static_cast<double>(angle_radians));
        double sin_angle = math::sin(static_cast<double>(angle_radians));
        type new_x = x * cos_angle - y * sin_angle;
        type new_y = x * sin_angle + y * cos_angle;
        x = new_x;
        y = new_y;
        return *this;
    }
    vector2 rotate(type angle_radians) const {
        vector2 result = *this;
        result.rotate_inline(angle_radians);
        return result;
    }

    vector2 operator-() const { return negate(); }

    bool operator==(const vector2& rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const vector2& rhs) const { return !(*this == rhs); }

    inline type& operator[](uint32 index) {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else {
            edvar::diagnostics::check(false, L"Index out of range");
            return xy[index % 2]; // modulo to avoid compiler warning
        }
    }
    inline const type& operator[](uint32 index) const {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else {
            edvar::diagnostics::check(false, L"Index out of range");
            return xy[index % 2]; // modulo to avoid compiler warning
        }
    }

    static vector2 one() { return vector2(1); }
    static vector2 zero() { return vector2(0); }
    static vector2 unit_x() { return vector2(1, 0); }
    static vector2 unit_y() { return vector2(0, 1); }

    static vector2 right() { return unit_x(); }
    static vector2 left() { return vector2(-1, 0); }
    static vector2 up() { return unit_y(); }
    static vector2 down() { return vector2(0, -1); }

    static vector2 construct_max(const vector2& lhs, const vector2& rhs) {
        return vector2(math::max(lhs.x, rhs.x), math::max(lhs.y, rhs.y));
    }
    static vector2 construct_min(const vector2& lhs, const vector2& rhs) {
        return vector2(math::min(lhs.x, rhs.x), math::min(lhs.y, rhs.y));
    }
    static vector2 lerp(const vector2& start, const vector2& end, type t) { return start + (end - start) * t; }
    static vector2 lerp_clamped(const vector2& start, const vector2& end, type t) {
        t = math::clamp(t, type(0), type(1));
        return lerp(start, end, t);
    }

    // Comparison by length (for ascending order)
    static bool compare_lengths(const vector2& lhs, const vector2& rhs) {
        return lhs.length_squared() < rhs.length_squared();
    }
    // Reverse comparison by length (for descending order)
    static bool compare_lengths_reverse(const vector2& lhs, const vector2& rhs) {
        return lhs.length_squared() > rhs.length_squared();
    }

    union {
        type xy[2];
        struct {
            type x, y;
        };
    };
};

template <typename type> struct vector3 {
    vector3() : x(0), y(0), z(0) {}
    vector3(type all) : x(all), y(all), z(all) {}
    vector3(type x, type y, type z) : x(x), y(y), z(z) {}
    vector3(const type* to_place_data) : x(to_place_data[0]), y(to_place_data[1]), z(to_place_data[2]) {}
    vector3(const std::initializer_list<type>& list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : type(0);
        y = (it != list.end()) ? *it++ : type(0);
        z = (it != list.end()) ? *it++ : type(0);
    }
    ~vector3() = default;
    vector3(const vector3& other) : x(other.x), y(other.y), z(other.z) {}
    vector3(vector3&& other) noexcept : x(edvar::move(other.x)), y(edvar::move(other.y)), z(edvar::move(other.z)) {}
    vector3& operator=(const vector3& rhs) {
        if (this != &rhs) {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
        }
        return *this;
    }
    vector3& operator=(vector3&& rhs) noexcept {
        if (this != &rhs) {
            x = edvar::move(rhs.x);
            y = edvar::move(rhs.y);
            z = edvar::move(rhs.z);
        }
        return *this;
    }
    vector3& operator=(const type* to_place_data) {
        x = to_place_data[0];
        y = to_place_data[1];
        z = to_place_data[2];
        return *this;
    }
    vector3& operator=(const std::initializer_list<type>& list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : type(0);
        y = (it != list.end()) ? *it++ : type(0);
        z = (it != list.end()) ? *it++ : type(0);
        return *this;
    }
    // Conversion from vector2
    template <typename other_type> explicit vector3(const vector2<other_type>& other, type z = type(0)) {
        x = static_cast<type>(other.x);
        y = static_cast<type>(other.y);
        this->z = z;
    }
    template <typename other_type> vector3& operator=(const vector2<other_type>& other) {
        x = static_cast<type>(other.x);
        y = static_cast<type>(other.y);
        z = type(0);
        return *this;
    }
    // Arithmetic operators
    vector3 operator+(const vector3& rhs) const { return vector3(*this) += rhs; }
    vector3& operator+=(const vector3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    vector3 operator-(const vector3& rhs) const { return vector3(*this) -= rhs; }
    vector3& operator-=(const vector3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    vector3 operator*(const vector3& rhs) const { return vector3(*this) *= rhs; }
    vector3& operator*=(const vector3& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }
    vector3 operator/(const vector3& rhs) const { return vector3(*this) /= rhs; }
    vector3& operator/=(const vector3& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    vector3 operator*(const type scalar) const { return vector3(*this) *= scalar; }
    vector3& operator*=(const type scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    vector3 operator/(const type scalar) const { return vector3(*this) /= scalar; }
    vector3& operator/=(const type scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    // Dot product
    type dot(const vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
    type length_squared() const { return this->dot(*this); }
    type length() const { return math::square_root(length_squared()); }
    vector3 normalized() const {
        type len = length();
        if (len == type(0))
            return vector3(0);
        return *this / len;
    }
    vector3& normalize_inline() {
        type len = length();
        if (len == type(0)) {
            x = y = z = type(0);
        } else {
            *this /= len;
        }
        return *this;
    }
    vector3 cross(const vector3& rhs) const {
        return vector3(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x);
    }
    vector3 mirror(const vector3& normal) const {
        vector3 result = *this;
        result.mirror_inline(normal);
        return result;
    }
    vector3& mirror_inline(const vector3& normal) {
        if (is_normalized()) {
            *this -= normal * (type(2) * this->dot(normal));
            return *this;
        } else {
            vector3 n = normal.normalized();
            *this -= n * (type(2) * this->dot(n));
            return *this;
        }
    }
    vector3 negate() const { return vector3(-x, -y, -z); }
    vector3& negate_inline() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }
    vector3 abs() const { return vector3(math::abs(x), math::abs(y), math::abs(z)); }
    vector3& abs_inline() {
        x = math::abs(x);
        y = math::abs(y);
        z = math::abs(z);
        return *this;
    }
    vector3 clamp(const vector3& min, const vector3& max) const {
        vector3 result = *this;
        result.clamp_inline(min, max);
        return result;
    }
    vector3& clamp_inline(const vector3& min, const vector3& max) {
        x = math::clamp(x, min.x, max.x);
        y = math::clamp(y, min.y, max.y);
        z = math::clamp(z, min.z, max.z);
        return *this;
    }
    bool inside_aabb(const vector3& min, const vector3& max) const {
        return x >= min.x && x <= max.x && y >= min.y && y <= max.y && z >= min.z && z <= max.z;
    }
    bool is_normalized(type epsilon = static_cast<type>(1e-6)) const {
        return math::abs(length_squared() - type(1)) < epsilon;
    }
    vector3& project_inline(const vector3& onto) {
        type onto_len_sq = onto.length_squared();
        if (onto_len_sq == type(0)) {
            x = y = z = type(0);
        } else {
            type projection_scale = this->dot(onto) / onto_len_sq;
            *this = onto * projection_scale;
        }
        return *this;
    }
    vector3 project(const vector3& onto) const {
        vector3 result = *this;
        result.project_inline(onto);
        return result;
    }
    vector3 operator-() const { return negate(); }
    bool operator==(const vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const vector3& rhs) const { return !(*this == rhs); }
    inline type& operator[](uint32 index) {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else if (index == 2)
            return z;
        else {
            edvar::diagnostics::check(false, L"Index out of range");
            return xyz[index % 3]; // modulo to avoid compiler warning
        }
    }
    inline const type& operator[](uint32 index) const {
        if (index == 0)
            return x;
        else if (index == 1)
            return y;
        else if (index == 2)
            return z;
        else {
            edvar::diagnostics::check(false, L"Index out of range");
            return xyz[index % 3]; // modulo to avoid compiler warning
        }
    }
    static vector3 one() { return vector3(1); }
    static vector3 zero() { return vector3(0); }
    static vector3 unit_x() { return vector3(1, 0, 0); }
    static vector3 unit_y() { return vector3(0, 1, 0); }
    static vector3 unit_z() { return vector3(0, 0, 1); }
    static vector3 right() { return unit_x(); }
    static vector3 left() { return vector3(-1, 0, 0); }
    static vector3 up() { return unit_y(); }
    static vector3 down() { return vector3(0, -1, 0); }
    static vector3 forward() { return unit_z(); }
    static vector3 backward() { return vector3(0, 0, -1); }

    static vector3 construct_max(const vector3& lhs, const vector3& rhs) {
        return vector3(math::max(lhs.x, rhs.x), math::max(lhs.y, rhs.y), math::max(lhs.z, rhs.z));
    }
    static vector3 construct_min(const vector3& lhs, const vector3& rhs) {
        return vector3(math::min(lhs.x, rhs.x), math::min(lhs.y, rhs.y), math::min(lhs.z, rhs.z));
    }
    static vector3 lerp(const vector3& start, const vector3& end, type t) { return start + (end - start) * t; }
    static vector3 lerp_clamped(const vector3& start, const vector3& end, type t) {
        t = math::clamp(t, type(0), type(1));
        return lerp(start, end, t);
    }
    // Comparison by length (for ascending order)
    static bool compare_lengths(const vector3& lhs, const vector3& rhs) {
        return lhs.length_squared() < rhs.length_squared();
    }
    // Reverse comparison by length (for descending order)
    static bool compare_lengths_reverse(const vector3& lhs, const vector3& rhs) {
        return lhs.length_squared() > rhs.length_squared();
    }



    union {
        type xyz[3];
        struct {
            type x, y, z;
        };
    };
};


using vector2f = vector2<float>;
using vector2d = vector2<double>;
using vector3f = vector3<float>;
using vector3d = vector3<double>;

} // namespace edvar::math