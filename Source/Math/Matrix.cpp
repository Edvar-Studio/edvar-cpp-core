#include "Math/Matrix.hpp"

namespace Edvar::Math {

// Explicit template instantiations for Matrix4x4
template struct Matrix4x4<float>;
template struct Matrix4x4<double>;
template struct Matrix4x4<int32_t>;

// Explicit template instantiations for Matrix3x3
template struct Matrix3x3<float>;
template struct Matrix3x3<double>;
template struct Matrix3x3<int32_t>;

// Explicit template instantiations for Matrix2x2
template struct Matrix2x2<float>;
template struct Matrix2x2<double>;

} // namespace Edvar::Math
