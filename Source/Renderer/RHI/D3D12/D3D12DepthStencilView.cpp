#ifdef _WIN32
#    include "Renderer/RHI/D3D12/D3D12DepthStencilView.hpp"

#    include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"
#    include <d3d12.h>
namespace Edvar::Renderer::RHI::D3D12 {
D3D12DepthStencil2DView::D3D12DepthStencil2DView(const SharedPointer<ITexture>& texture,
                                                 const ResourceDataFormat& withFormat, const uint32_t& withMipSlice,
                                                 const uint32_t& withPlaneSlice, const DepthStencilViewType withType)
    : IDepthStencil2DView(texture, withFormat, withMipSlice, withPlaneSlice, withType) {
    if (texture && texture->GetNativeHandle()) {
        const auto rhiDevice = StaticCastSharedReference<D3D12RenderDevice>(texture->GetAssociatedDevice());
        auto* nativeDevice = static_cast<ID3D12Device*>(rhiDevice->NativeHandle);
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = static_cast<DXGI_FORMAT>(withFormat);
        dsvDesc.Flags = static_cast<D3D12_DSV_FLAGS>(withType);
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
        dsvDesc.Texture2D.MipSlice = withMipSlice;
        allocation = rhiDevice->DSVHeap->AllocateDescriptor();
        D3D12_CPU_DESCRIPTOR_HANDLE descHandle = {};
        descHandle.ptr = reinterpret_cast<uintptr_t>(allocation.GetCPUHandle());
        nativeDevice->CreateDepthStencilView(static_cast<ID3D12Resource*>(texture->GetNativeHandle()), &dsvDesc,
                                             descHandle);
    }
}
D3D12DepthStencil2DView::~D3D12DepthStencil2DView() { allocation.Free(); }
} // namespace Edvar::Renderer::RHI::D3D12
#endif
