#pragma once

#include "IRenderResource.hpp"
#include "ResourceDataFormat.hpp"

namespace Edvar::Renderer::RHI {
class EDVAR_CPP_CORE_API IBuffer : public IRenderResource {
public:
    explicit IBuffer(const SharedReference<IRenderDevice>& device, const uint32_t inSize)
        : IRenderResource(device), size(inSize) {}
    Math::Vector2i GetSize() const override { return Math::Vector2i(static_cast<int32_t>(size), 1); }
    uint32_t GetMipLevels() const override { return 1; }
    uint32_t GetDimensionsCount() const override { return 1; }
    ResourceDataFormat GetFormat() const override { return ResourceDataFormat::Unknown; }

private:
    uint32_t size;
};

} // namespace Edvar::Renderer::RHI