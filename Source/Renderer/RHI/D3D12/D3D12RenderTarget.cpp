#include "Renderer/RHI/D3D12/D3D12RenderTarget.hpp"
#ifdef _WIN32
#    include <d3d12.h>

namespace Edvar::Renderer::RHI::D3D12 {

D3D12RenderTarget2D::D3D12RenderTarget2D(const SharedPointer<ITexture>& texture, const ResourceDataFormat& withFormat,
                                         const uint32_t withMipSlice, const uint32_t withPlaneSlice)
    : IRenderTarget2D(texture, withFormat, withMipSlice, withPlaneSlice) {
    // Get the texture as a resource
    if (texture && texture->GetNativeHandle()) {
        ID3D12Resource2* resource = static_cast<ID3D12Resource2*>(texture->GetNativeHandle());
        const SharedReference<D3D12RenderDevice> rhiDevice =
            StaticCastSharedReference<D3D12RenderDevice>(texture->GetAssociatedDevice());
        ID3D12Device* device = static_cast<ID3D12Device*>(rhiDevice->NativeHandle);
        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = static_cast<DXGI_FORMAT>(withFormat);
        desc.Texture2D.MipSlice = withMipSlice;
        desc.Texture2D.PlaneSlice = withPlaneSlice;
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        allocation = rhiDevice->RTVHeap->AllocateDescriptor();
        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = {};
        descriptorHandle.ptr = reinterpret_cast<SIZE_T>(allocation.GetCPUHandle());
        device->CreateRenderTargetView(resource, &desc, descriptorHandle);
    }
}
Math::Vector2i D3D12RenderTarget2D::GetSize() { return GetTexture()->GetSize(); }
} // namespace Edvar::Renderer::RHI::D3D12
#endif