#pragma once

#include "Math/Vector.hpp"
namespace Edvar::Math {
struct Quaternion {
    static const Quaternion Identity;

    Quaternion();
    Quaternion(float x, float y, float z, float w);

    Quaternion operator*(const Quaternion& other) const;
    Vector3f RotateVector(const Vector3f& vec) const;

    union {
        struct {
            float X, Y, Z, W;
        };
        float XYZW[4];
        SIMD::VectorDataType<double> SIMDData;
    };
};
} // namespace Edvar::Math