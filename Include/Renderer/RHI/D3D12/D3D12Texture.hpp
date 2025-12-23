#pragma once
#include "../ITexture.hpp"

namespace Edvar::Renderer::RHI::D3D12 {
class D3D12Texture2D : public ITexture2D {
public:
    D3D12Texture2D(const SharedReference<IRenderDevice>& device, const ResourceDataFormat& inFormat,
                   const Math::Vector2i& inSize, const uint32_t& inMipLevels, const uint32_t& sampleCount)
        : ITexture2D(device, inFormat, inSize, inMipLevels, sampleCount) {}

    void* nativePtr = nullptr;
};
} // namespace Edvar::Renderer::RHI::D3D12