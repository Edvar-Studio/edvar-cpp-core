#pragma once
#include "../Math/Math.hpp"

namespace Edvar::Renderer {
struct Viewport {
    Viewport() {}
    Viewport(float topLeftX = 0, float topLeftY = 0, float width = 1, float height = 1, float minDepth = 0.f,
             float maxDepth = 1.f)
        : topLeft(Math::Vector2f(topLeftX, topLeftY)), size(Math::Vector2f(width, height)), MinDepth(minDepth),
          MaxDepth(maxDepth) {}

    Math::Vector2f topLeft = Math::Vector2f::Zero;
    Math::Vector2f size = Math::Vector2f::One;
    float MinDepth = 0.f, MaxDepth = 1.f;
};
} // namespace Edvar::Renderer