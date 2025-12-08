#pragma once

// This file serves as a central inclusion point for all Math-related headers in
// the Edvar::Math namespace. This is due to circular dependencies between some
// of the Math headers, which can lead to inclusion issues

#include "Math.hpp"       // IWYU pragma: keep
#include "Matrix.hpp"     // IWYU pragma: keep
#include "Quaternion.hpp" // IWYU pragma: keep
#include "Rotator.hpp"    // IWYU pragma: keep
#include "Transform.hpp"  // IWYU pragma: keep
#include "Vector.hpp"     // IWYU pragma: keep

#include "Matrix.inl"     // IWYU pragma: keep
#include "Quaternion.inl" // IWYU pragma: keep
#include "Rotator.inl"    // IWYU pragma: keep
#include "Transform.inl"  // IWYU pragma: keep
#include "Vector.inl"     // IWYU pragma: keep

#include "Math/Color.hpp" // IWYU pragma: keep
#include "Random.hpp"     // IWYU pragma: keep