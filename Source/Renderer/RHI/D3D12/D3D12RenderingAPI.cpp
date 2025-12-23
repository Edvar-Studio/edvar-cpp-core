#pragma once
#ifdef _WIN32
#    include "Renderer/RHI/D3D12/D3D12RenderingAPI.hpp"
#    include "Renderer/RHI/IRenderDevice.hpp"
#    include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"

#    include <dxgi1_6.h>
#    include <d3d12.h>
#    include <wrl/client.h>
namespace Edvar::Renderer::RHI::D3D12 {
IDXGIFactory2* GDXGIFactory = nullptr;
D3D12RenderingAPI::D3D12RenderingAPI() : IRenderingAPI(u"DirectX 12") {
    Platform::GetPlatform().PrintMessageToDebugger(u"D3D12: Initializing D3D12 Rendering API.\n");
    if (GDXGIFactory == nullptr) [[unlikely]] {
        UINT createFactoryFlags = 0;
#    ifdef EDVAR_CPP_CORE_GRAPHICS_API_DEBUG_RENDERING
        Microsoft::WRL::ComPtr<ID3D12Debug> debug;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
            debug->EnableDebugLayer();
            Platform::GetPlatform().PrintMessageToDebugger(u"D3D12: Enabled D3D12 Debug Layer.\n");
        } else {
            Platform::GetPlatform().PrintMessageToDebugger(u"D3D12: Failed to enable D3D12 Debug Layer.\n");
        }
        createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#    endif
        if (const HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&GDXGIFactory)); FAILED(hr)) {
            Platform::GetPlatform().OnFatalError(u"D3D12: Failed to create DXGI Factory.");
        }
    }
}
SharedPointer<IRenderDevice> D3D12RenderingAPI::CreatePrimaryRenderDevice() {
    IDXGIFactory6* requestedInterface;
    IDXGIAdapter* adapter = nullptr;
    Platform::GetPlatform().PrintMessageToDebugger(u"Creating new primary render device.\n");
    if (SUCCEEDED(GDXGIFactory->QueryInterface(&requestedInterface))) {
        if (FAILED(requestedInterface->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                                  IID_PPV_ARGS(&adapter)))) {
            adapter = nullptr;
        }
        requestedInterface->Release();
    }
    ID3D12Device* requestedDevice;
    if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&requestedDevice)))) {
        Platform::GetPlatform().OnFatalError(u"D3D12: Failed to create D3D12 Device.");
        return nullptr;
    }
#    ifdef EDVAR_CPP_CORE_GRAPHICS_API_DEBUG_RENDERING
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(requestedDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
        // D3D12_MESSAGE_ID hideMessages[] = {
        //     D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
        // };
        // D3D12_INFO_QUEUE_FILTER filter = {};
        // filter.DenyList.NumIDs = _countof(hideMessages);
        // filter.DenyList.pIDList = hideMessages;
        // infoQueue->AddStorageFilterEntries(&filter);
    }
#    endif
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
void D3D12RenderingAPI::PrintStoredMessagesToDebugger() {
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (FAILED(
            static_cast<ID3D12Device*>(StaticCastSharedReference<D3D12RenderDevice>(GetPrimaryDevice())->NativeHandle)
                ->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        return;
    }
    const UINT64 numMessages = infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
    for (UINT64 i = 0; i < numMessages; i++) {
        SIZE_T messageLength = 0;
        if (FAILED(infoQueue->GetMessageW(i, nullptr, &messageLength))) {
            continue;
        }
        auto* message = new uint8_t[messageLength];
        if (FAILED(infoQueue->GetMessage(i, reinterpret_cast<D3D12_MESSAGE*>(message), &messageLength))) {
            delete[] message;
            continue;
        }
        const D3D12_MESSAGE* d3dMessage = reinterpret_cast<D3D12_MESSAGE*>(message);
        switch (d3dMessage->Severity) {
        case D3D12_MESSAGE_SEVERITY_CORRUPTION:
            Platform::GetPlatform().PrintMessageToDebugger(u"[Corruption] ");
            break;
        case D3D12_MESSAGE_SEVERITY_ERROR:
            Platform::GetPlatform().PrintMessageToDebugger(u"[Error] ");
            break;
        case D3D12_MESSAGE_SEVERITY_WARNING:
            Platform::GetPlatform().PrintMessageToDebugger(u"[Warning] ");
            break;
        case D3D12_MESSAGE_SEVERITY_INFO:
            Platform::GetPlatform().PrintMessageToDebugger(u"[Info] ");
            break;
        case D3D12_MESSAGE_SEVERITY_MESSAGE:
            Platform::GetPlatform().PrintMessageToDebugger(u"[Message] ");
            break;
        }
        Platform::GetPlatform().PrintMessageToDebugger(u"D3D12 Message: ");
        const auto* convertedMessage = Utils::CStrings::CreateConvertedString<char16_t>(d3dMessage->pDescription);
        Platform::GetPlatform().PrintMessageToDebugger(convertedMessage);
        delete[] convertedMessage;
        delete[] message;
    }
    infoQueue->ClearStoredMessages();
}
#endif
} // namespace Edvar::Rendering::D3D12