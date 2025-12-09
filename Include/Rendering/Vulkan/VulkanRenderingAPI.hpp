#pragma once
#include "../RenderingCore.hpp"

namespace Edvar::Rendering::Vulkan {
class VulkanRenderingAPI final : public IRenderingAPI {
public:
    VulkanRenderingAPI() : IRenderingAPI(u"Vulkan") {}
};
} // namespace Edvar::Rendering::Vulkan