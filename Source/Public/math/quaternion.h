#pragma once

namespace edvar::math {
template <typename type> struct quaternion {
    using simd_type = std::conditional_t<std::is_same_v<type, double>, simd::simd_256d, simd::simd_128f>;
    quaternion() : x(0), y(0), z(0), w(1) {}
    quaternion(type all) : x(all), y(all), z(all), w(all) {}
    quaternion(type x, type y, type z, type w) : x(x), y(y), z(z), w(w) {}
    quaternion(const type* to_place_data) : x(to_place_data[0]), y(to_place_data[1]), z(to_place_data[2]), w(to_place_data[3]) {}
    quaternion(const std::initializer_list<type>& list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : type(0);
        y = (it != list.end()) ? *it++ : type(0);
        z = (it != list.end()) ? *it++ : type(0);
        w = (it != list.end()) ? *it++ : type(1);
    }
    ~quaternion() = default;
    quaternion(const quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    quaternion(quaternion&& other) noexcept
        : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)), w(std::move(other.w)) {}
    quaternion& operator=(const quaternion& rhs) {
        if (this != &rhs) {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = rhs.w;
        }
        return *this;
    }
    quaternion& operator=(quaternion&& rhs) noexcept {
        if (this != &rhs) {
            x = std::move(rhs.x);
            y = std::move(rhs.y);
            z = std::move(rhs.z);
            w = std::move(rhs.w);
        }
        return *this;
    }
    quaternion& operator=(const type* to_place_data) {
        x = to_place_data[0];
        y = to_place_data[1];
        z = to_place_data[2];
        w = to_place_data[3];
        return *this;
    }
    quaternion& operator=(const std::initializer_list<type>& list) {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : type(0);
        y = (it != list.end()) ? *it++ : type(0);
        z = (it != list.end()) ? *it++ : type(0);
        w = (it != list.end()) ? *it++ : type(1);
        return *this;
    }

    
    // operators
    quaternion& operator+=(const quaternion& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }
    quaternion& operator-=(const quaternion& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }
    quaternion& operator*=(const quaternion& rhs) {
        const type new_x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
        const type new_y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
        const type new_z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
        const type new_w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
        x = new_x;
        y = new_y;
        z = new_z;
        w = new_w;
        return *this;
    }






    union {
        type data[4];
        struct {
            type x, y, z, w;
        };
        simd_type simd_data;
    };
};

} // namespace edvar::math