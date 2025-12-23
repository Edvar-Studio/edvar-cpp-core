#pragma once

#ifdef _WIN32
#    include "../ISwapchain.hpp"
#    include "../ICommandAllocator.hpp"
namespace Edvar::Renderer::RHI {
class IRenderDevice;
}
namespace Edvar::Renderer::RHI::D3D12 {
class D3D12SwapchainBufferTexture : public ISwapchainBufferTexture {
public:
    D3D12SwapchainBufferTexture(const SharedReference<IRenderDevice>& device, void* inNativeHandle)
        : ISwapchainBufferTexture(device, ResourceDataFormat::Unknown, {}, 0, 0), nativePtr(inNativeHandle) {}
    ~D3D12SwapchainBufferTexture() override;
    void* nativePtr;

    bool IsRenderTargetAllowed() const override { return true; }
    Math::Vector2i GetSize() const override;
    uint32_t GetMipLevels() const override;
    uint32_t GetSampleCount() const override;
    ResourceDataFormat GetFormat() const override;
    WeakPointer<ITextureView> CreateView(const ResourceDataFormat& withResourceFormat, uint32_t withMipLevel) override;
    WeakPointer<IDepthStencil2DView> CreateDepthStencilView(const ResourceDataFormat& withResourceFormat,
                                                            uint32_t withMipLevel) override;
    void* GetNativeHandle() const override { return nativePtr; }
};
class EDVAR_CPP_CORE_API D3D12Swapchain final : public ISwapchain {
public:
    D3D12Swapchain(const SharedReference<IRenderDevice>& renderDevice, const Windowing::Window& forWindow,
                   ResourceDataFormat dataFormat);
    bool IsFullscreen() const override;
    void Present(uint8_t interval) override;
    void SetFullscreen(bool fullscreen, const Math::Vector2i& newSize) override;
    Math::Vector2i GetSize() const override;
    void Resize(const Math::Vector2i& newSize, uint8_t bufferCount, const ResourceDataFormat& newFormat) override;
    void SetBackgroundColor(const Math::Color& color) override;
    void SetHDRMode(bool useHdr) override;

    virtual void OnRenderDeviceLost() override;
    virtual void OnRenderDeviceRestored(IRenderDevice& newDevice) override;

    void CreateNativeSwapchain(const Windowing::Window& forWindow, ResourceDataFormat newDataFormat);

    // Buffer operations
    SharedReference<ISwapchainBufferTexture> GetBufferTexture(uint32_t index) override;
    WeakPointer<IRenderTarget2D> GetBufferRenderTarget(uint32_t index) override;
    uint32_t GetBufferCount() const override;
    uint32_t GetCurrentBackBufferIndex() const override;
    void* nativePtr;

private:
    Containers::List<SharedReference<ISwapchainBufferTexture>> buffers;
    Containers::List<WeakPointer<IRenderTarget2D>> renderTargets;
    uint32_t currentBackBufferIndex = 0;
};
} // namespace Edvar::Renderer::RHI::D3D12

#endif