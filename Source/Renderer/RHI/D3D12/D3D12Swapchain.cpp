#include "Renderer/RHI/D3D12/D3D12Swapchain.hpp"

#include "Renderer/RHI/D3D12/D3D12CommandQueue.hpp"
#include "Renderer/RHI/D3D12/D3D12RenderTarget.hpp"
#ifdef _WIN32

#    include "Renderer/RHI/RenderingCore.hpp"
#    include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"
#    include "Renderer/RHI/D3D12/D3D12RenderingAPI.hpp"
#    include "Windowing/Window.hpp"

#    include <dxgi1_5.h>
#    include <d3d12.h>
#    include <dxgi1_2.h>

#endif

namespace Edvar::Renderer::RHI::D3D12 {

D3D12SwapchainBufferTexture::~D3D12SwapchainBufferTexture() {
    if (nativePtr) {
        static_cast<ID3D12Resource*>(nativePtr)->Release();
    }
}
Math::Vector2i D3D12SwapchainBufferTexture::GetSize() const {
    ID3D12Resource2* resource = static_cast<ID3D12Resource2*>(nativePtr);
    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    return Math::Vector2i(static_cast<int32_t>(desc.Width), static_cast<int32_t>(desc.Height));
}
uint32_t D3D12SwapchainBufferTexture::GetMipLevels() const {
    ID3D12Resource2* resource = static_cast<ID3D12Resource2*>(nativePtr);
    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    return desc.MipLevels;
}
uint32_t D3D12SwapchainBufferTexture::GetSampleCount() const {
    ID3D12Resource2* resource = static_cast<ID3D12Resource2*>(nativePtr);
    D3D12_RESOURCE_DESC1 desc = resource->GetDesc1();
    return desc.SampleDesc.Count;
}
ResourceDataFormat D3D12SwapchainBufferTexture::GetFormat() const {
    ID3D12Resource2* resource = static_cast<ID3D12Resource2*>(nativePtr);
    D3D12_RESOURCE_DESC1 desc = resource->GetDesc1();
    return static_cast<ResourceDataFormat>(desc.Format);
}
WeakPointer<ITextureView> D3D12SwapchainBufferTexture::CreateView(const ResourceDataFormat& withResourceFormat,
                                                                  uint32_t withMipLevel) {
    SharedReference<D3D12RenderTarget2D> renderTarget =
        MakeShared<D3D12RenderTarget2D>(SharedFromThis(this), withResourceFormat, withMipLevel, 0);
    ownedViews.Add(renderTarget);
    return renderTarget;
}
WeakPointer<IDepthStencil2DView>
D3D12SwapchainBufferTexture::CreateDepthStencilView(const ResourceDataFormat& withResourceFormat,
                                                    uint32_t withMipLevel) {
    return nullptr; // not supported.
}
D3D12Swapchain::D3D12Swapchain(const SharedReference<IRenderDevice>& renderDevice, const Windowing::Window& forWindow,
                               ResourceDataFormat dataFormat)
    : ISwapchain(renderDevice, forWindow, dataFormat), nativePtr(nullptr) {
    CreateNativeSwapchain(forWindow, dataFormat);
}
bool D3D12Swapchain::IsFullscreen() const {
    const auto swapChain = static_cast<IDXGISwapChain*>(nativePtr);
    BOOL fullscreen = FALSE;
    if (FAILED(swapChain->GetFullscreenState(&fullscreen, nullptr))) {
        CheckDeviceSanity();
    }
    return fullscreen == TRUE;
}
void D3D12Swapchain::Present(uint8_t interval) {
    if (const auto swapChain = static_cast<IDXGISwapChain*>(nativePtr); FAILED(swapChain->Present(interval, 0))) {
        CheckDeviceSanity();
    }
    currentBackBufferIndex = (currentBackBufferIndex + 1) % GetBufferCount();
}
void D3D12Swapchain::SetFullscreen(const bool fullscreen, const Math::Vector2i& newSize) {
    const auto swapChain = static_cast<IDXGISwapChain*>(nativePtr);
    if (const auto result = swapChain->SetFullscreenState(fullscreen ? TRUE : FALSE, nullptr);
        FAILED(result) && result != DXGI_ERROR_NOT_CURRENTLY_AVAILABLE ||
        result != DXGI_STATUS_MODE_CHANGE_IN_PROGRESS) {
        CheckDeviceSanity();
    }
}
Math::Vector2i D3D12Swapchain::GetSize() const {
    if (nativePtr != nullptr) {
        const auto swapChain = static_cast<IDXGISwapChain1*>(nativePtr);
        UINT width = 0;
        UINT height = 0;
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        if (SUCCEEDED(swapChain->GetDesc1(&desc))) {
            width = desc.Width;
            height = desc.Height;
        } else {
            GetAssociatedDevice()->DoCheckDeviceSanity();
            return -Math::Vector2i::One;
        }
        return Math::Vector2i(static_cast<int32_t>(width), static_cast<int32_t>(height));
    }
    return -Math::Vector2i::One;
}
void D3D12Swapchain::Resize(const Math::Vector2i& newSize, uint8_t bufferCount, const ResourceDataFormat& newFormat) {
    auto* swapChain = static_cast<IDXGISwapChain2*>(nativePtr);
    if (FAILED(swapChain->ResizeBuffers(bufferCount, static_cast<UINT>(newSize.X), static_cast<UINT>(newSize.Y),
                                        static_cast<DXGI_FORMAT>(newFormat), DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING))) {
        CheckDeviceSanity();
        // Fail silently.
        return;
    }
    if (IsFullscreen()) {
        DXGI_MODE_DESC newTargetDesc = {};
        newTargetDesc.Width = static_cast<UINT>(newSize.X);
        newTargetDesc.Height = static_cast<UINT>(newSize.Y);
        newTargetDesc.Format = static_cast<DXGI_FORMAT>(newFormat);
        newTargetDesc.RefreshRate = {0, 0};
        newTargetDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        newTargetDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        if (FAILED(swapChain->ResizeTarget(&newTargetDesc))) {
            CheckDeviceSanity();
            // Fail silently.
            return;
        }
    }
    // TODO: Recreate the render target views. Flush the work on the command contexts.
}
void D3D12Swapchain::SetBackgroundColor(const Math::Color& color) {
    if (nativePtr != nullptr) {
        auto* swapChain = static_cast<IDXGISwapChain4*>(nativePtr);
        const DXGI_RGBA clearColor = {static_cast<float>(color.R) / 255.f, static_cast<float>(color.G) / 255.f,
                                      static_cast<float>(color.B) / 255.f, static_cast<float>(color.A) / 255.f};
        if (FAILED(swapChain->SetBackgroundColor(&clearColor))) {
            CheckDeviceSanity();
        }
    }
}
void D3D12Swapchain::SetHDRMode(const bool useHdr) {
    ISwapchain::SetHDRMode(useHdr);
    if (nativePtr != nullptr && GetAssociatedWindow().DoesCurrentOutputSupportHDR()) {
        const auto swapChain = static_cast<IDXGISwapChain4*>(nativePtr);
        const DXGI_COLOR_SPACE_TYPE colorSpace =
            useHdr ? DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709 : DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
        if (FAILED(swapChain->SetColorSpace1(colorSpace))) {
            CheckDeviceSanity();
        } else if (useHdr) {
            Resize(GetSize(), 3, ResourceDataFormat::R10G10B10A2_UNorm);
        }
    }
}
void D3D12Swapchain::OnRenderDeviceLost() {
    ISwapchain::OnRenderDeviceLost();
    // Release the swap chain
    if (nativePtr != nullptr) {
        const auto swapChain = static_cast<IDXGISwapChain*>(nativePtr);
        swapChain->Release();
        nativePtr = nullptr;
    }
}
void D3D12Swapchain::OnRenderDeviceRestored(IRenderDevice& newDevice) {
    ISwapchain::OnRenderDeviceRestored(newDevice);
    // Recreate the swap chain
    CreateNativeSwapchain(GetAssociatedWindow(), GetFormat());
}

static constexpr int32_t EDVAR_CPP_CORE_D3D12_WINDOW_BUFFER_COUNT = 3;

void D3D12Swapchain::CreateNativeSwapchain(const Windowing::Window& forWindow, ResourceDataFormat newDataFormat) {
    Platform::GetPlatform().PrintMessageToDebugger(u"D3D12: Creating Swapchain for window.\n");
    const SharedPointer<D3D12RenderDevice> device = StaticCastSharedReference<D3D12RenderDevice>(GetAssociatedDevice());
    Platform::GetPlatform().PrintMessageToDebugger(
        *String::Format(u"D3D12: Using device handle {}.\n", device->NativeHandle));
    IDXGISwapChain1* swapChain = nullptr;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = EDVAR_CPP_CORE_D3D12_WINDOW_BUFFER_COUNT;
    swapChainDesc.Format = static_cast<DXGI_FORMAT>(newDataFormat);
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc = {1, 0};
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    auto* commandQ =
        static_cast<IUnknown*>(StaticCastSharedPointer<D3D12CommandQueue>(device->GetGraphicsQueue())->nativeHandle);
    auto windowHandle = static_cast<HWND>(forWindow.GetImplementation().GetNativeHandle());
    if (FAILED(GDXGIFactory->CreateSwapChainForHwnd(commandQ, windowHandle, &swapChainDesc, nullptr, nullptr,
                                                    &swapChain))) {
        Platform::GetPlatform().PrintMessageToDebugger(u"D3D12: Failed to create swapchain for window.\n");
        CheckDeviceSanity();
        return;
    }
    nativePtr = swapChain;
    // set up the back-buffer array.
    buffers.Clear();
    for (uint32_t i = 0; i < GetBufferCount(); ++i) {
        ID3D12Resource* backBufferResource = nullptr;
        if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResource)))) {
            Platform::GetPlatform().PrintMessageToDebugger(
                *String::Format(u"D3D12: Failed to get back buffer {} from swapchain.\n", i));
            CheckDeviceSanity();
            continue;
        }
        SharedReference<D3D12SwapchainBufferTexture> bufferTexture =
            MakeShared<D3D12SwapchainBufferTexture>(GetAssociatedDevice(), backBufferResource);
        buffers.Add(bufferTexture);
        WeakPointer<ITextureView> renderTarget = bufferTexture->CreateView(ResourceDataFormat::Unknown, 0);
        renderTargets.Add(renderTarget);
    }
}
SharedReference<ISwapchainBufferTexture> D3D12Swapchain::GetBufferTexture(const uint32_t index) {
    return buffers.Get(index);
}
WeakPointer<IRenderTarget2D> D3D12Swapchain::GetBufferRenderTarget(const uint32_t index) {
    return renderTargets.Get(index);
}
uint32_t D3D12Swapchain::GetBufferCount() const { return EDVAR_CPP_CORE_D3D12_WINDOW_BUFFER_COUNT; }
uint32_t D3D12Swapchain::GetCurrentBackBufferIndex() const { return currentBackBufferIndex; }
} // namespace Edvar::Renderer::RHI::D3D12