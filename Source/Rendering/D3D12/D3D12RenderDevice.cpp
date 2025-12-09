#include "Rendering/D3D12/D3D12RenderDevice.hpp"
#ifdef _WIN32

#    include "Rendering/RenderingCore.hpp"
#    include "Rendering/D3D12/D3D12Swapchain.hpp"

#    include <d3d12.h>
namespace Edvar::Rendering::D3D12 {
D3D12RenderDevice::~D3D12RenderDevice() { D3D12RenderDevice::Destroy(); }
void D3D12RenderDevice::DoCheckDeviceSanity() const {
    if (nativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(nativeHandle);
        const auto reason = device->GetDeviceRemovedReason();
        if (reason == S_OK) {
            return;
        }

        switch (reason) {
        case DXGI_ERROR_DEVICE_REMOVED:
        case DXGI_ERROR_DEVICE_RESET: {
            IRenderingAPI::NotifyRenderDeviceLost(*this);
        }
        default: {
            return;
        }
        }
    }
}
bool D3D12RenderDevice::HasShaderModelSupport(const ShaderModel model) const {
    if (nativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(nativeHandle);
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModelFeature = {};
        shaderModelFeature.HighestShaderModel = model == ShaderModel::SM6 ? D3D_SHADER_MODEL_6_0 : D3D_SHADER_MODEL_5_1;
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModelFeature,
                                                  sizeof(shaderModelFeature)))) {
            return shaderModelFeature.HighestShaderModel >=
                   (model == ShaderModel::SM6 ? D3D_SHADER_MODEL_6_0 : D3D_SHADER_MODEL_5_1);
        }
    }
    return false;
}
bool D3D12RenderDevice::HasRayTracingSupport() const {
    if (nativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(nativeHandle);
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5)))) {
            return options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
        }
    }
    return false;
}
void D3D12RenderDevice::Destroy() {
    IRenderDevice::Destroy();
    if (nativeHandle != nullptr) {
        auto* device = static_cast<ID3D12Device*>(nativeHandle);
        device->Release();
        nativeHandle = nullptr;
    }
}
SharedReference<ISwapchain> D3D12RenderDevice::CreateSwapchain(const Windowing::Window& window,
                                                               ResourceDataFormat resourceDataFormat) {
    SharedReference<D3D12Swapchain> swapchain = MakeShared<D3D12Swapchain>(AsShared(), window, resourceDataFormat);
    return swapchain;
}
} // namespace Edvar::Rendering::D3D12

#endif
