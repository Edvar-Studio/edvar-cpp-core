#pragma once
#ifdef _WIN32
#    include "D3D12RenderDevice.hpp"
#    include "../IRenderTarget.hpp"

namespace Edvar::Renderer::RHI::D3D12 {
class D3D12RenderTarget2D : public IRenderTarget2D {
public:
    D3D12RenderTarget2D(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat,
                        uint32_t withMipSlice, uint32_t withPlaneSlice);
    ~D3D12RenderTarget2D() override { allocation.Free(); }
    bool CanClear() const override { return true; }
    Math::Vector2i GetSize() override;

    D3D12DescriptorHeapAllocation allocation;
};
} // namespace Edvar::Renderer::RHI::D3D12
#endif