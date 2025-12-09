#pragma once
#ifdef _WIN32
#    include "Rendering/D3D12/D3D12RenderingAPI.hpp"
#    include "Rendering/IRenderDevice.hpp"
#    include "Rendering/D3D12/D3D12RenderDevice.hpp"

#    include <dxgi1_6.h>
#    include <d3d12.h>
#    include <wrl/client.h>
namespace Edvar::Rendering::D3D12 {
IDXGIFactory2* GDXGIFactory = nullptr;
D3D12RenderingAPI::D3D12RenderingAPI() : IRenderingAPI(u"DirectX 12") {
    Platform::Get().PrintMessageToDebugger(*String::PrintF(u"D3D12: Initializing D3D12 Rendering API.\n"));
    if (GDXGIFactory == nullptr) [[unlikely]] {
        UINT createFactoryFlags = 0;
#    ifdef _DEBUG
        createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#    endif
        if (const HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&GDXGIFactory)); FAILED(hr)) {
            Platform::Get().OnFatalError(u"D3D12: Failed to create DXGI Factory.");
        }
    }
}
SharedPointer<IRenderDevice> D3D12RenderingAPI::CreatePrimaryRenderDevice() {
    IDXGIFactory6* requestedInterface;
    IDXGIAdapter* adapter = nullptr;
    Platform::Get().PrintMessageToDebugger(u"Creating new primary render device.\n");
    if (SUCCEEDED(GDXGIFactory->QueryInterface(&requestedInterface))) {
        if (FAILED(requestedInterface->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                                  IID_PPV_ARGS(&adapter)))) {
            adapter = nullptr;
        }
        requestedInterface->Release();
    }
    ID3D12Device* requestedDevice;
    if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&requestedDevice)))) {
        Platform::Get().OnFatalError(u"D3D12: Failed to create D3D12 Device.");
        return nullptr;
    }
    SharedPointer<D3D12RenderDevice> nativeDevice = MakeShared<D3D12RenderDevice>(requestedDevice);
    return nativeDevice;
}

using namespace Microsoft::WRL;
bool D3D12RenderingAPI::DoesOutputSupportHDR(const Platform::MonitorInfo& monitorInfo) {
    ComPtr<IDXGIAdapter1> adapter;
    for (UINT a = 0; GDXGIFactory->EnumAdapters1(a, &adapter) != DXGI_ERROR_NOT_FOUND; a++) {
        ComPtr<IDXGIOutput> output;
        for (UINT o = 0; adapter->EnumOutputs(o, &output) != DXGI_ERROR_NOT_FOUND; o++) {
            DXGI_OUTPUT_DESC desc;
            if (FAILED(output->GetDesc(&desc))) {
                continue;
            }

            if (desc.Monitor == monitorInfo.NativeHandle) {
                // Found the correct output!
                // Cast to IDXGIOutput6 below.
                ComPtr<IDXGIOutput6> output6;
                if (FAILED(output.As(&output6))) {
                    continue;
                }
                DXGI_OUTPUT_DESC1 desc1;
                if (FAILED(output6->GetDesc1(&desc1))) {
                    continue;
                }

                const bool hdrSupported = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ||
                                           desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709 ||
                                           desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020) &&
                                          desc1.MaxLuminance > 0.0f;
                return hdrSupported;
            }
        }
    }
    return false;
}
#endif
} // namespace Edvar::Rendering::D3D12