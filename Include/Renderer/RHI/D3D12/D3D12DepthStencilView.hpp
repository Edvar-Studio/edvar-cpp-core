#pragma once
#ifdef _WIN32
#    include "D3D12RenderDevice.hpp"
#    include "../IDepthStencilView.hpp"
namespace Edvar::Renderer::RHI::D3D12 {
class D3D12DepthStencil2DView : public IDepthStencil2DView {
public:
    D3D12DepthStencil2DView(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat,
                            const uint32_t& withMipSlice, const uint32_t& withPlaneSlice,
                            DepthStencilViewType withType);
    ~D3D12DepthStencil2DView() override;
    D3D12DescriptorHeapAllocation allocation;
};
} // namespace Edvar::Renderer::RHI::D3D12

#endif