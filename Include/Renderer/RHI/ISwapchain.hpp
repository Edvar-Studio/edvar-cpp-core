#pragma once

#include "ITexture.hpp"
#include "RenderingCore.hpp"
#include "ResourceDataFormat.hpp"
namespace Edvar::Windowing {
class Window;
}
namespace Edvar::Renderer::RHI {
class IRenderTarget2D;
class ISwapchainBufferTexture : public ITexture2D {
public:
    ISwapchainBufferTexture(const SharedReference<IRenderDevice>& device, const ResourceDataFormat& inFormat,
                            const Math::Vector2i& inSize, uint32_t inMipLevels, uint32_t sampleCount)
        : ITexture2D(device, inFormat, inSize, inMipLevels, sampleCount) {}
};
class ISwapchain : public IRenderDeviceDependent {
public:
    ~ISwapchain() override = default;

    ISwapchain(const RenderDeviceRef& RenderDevice, const Windowing::Window& forWindow, ResourceDataFormat dataFormat);
    [[nodiscard]] const Windowing::Window& GetAssociatedWindow() const { return *AssociatedWindow.Get(); }
    [[nodiscard]] ResourceDataFormat GetFormat() const { return Format; }
    virtual void Present(uint8_t interval) = 0;

    virtual void SetFullscreen(bool fullscreen, const Math::Vector2i& newSize) = 0;
    virtual bool IsFullscreen() const = 0;
    virtual Math::Vector2i GetSize() const = 0;
    virtual void Resize(const Math::Vector2i& newSize, uint8_t bufferCount, const ResourceDataFormat& newFormat) = 0;
    virtual void SetBackgroundColor(const Math::Color& color) = 0;
    virtual void SetHDRMode(bool useHdr) {}

    // Buffer operations

    virtual SharedReference<ISwapchainBufferTexture> GetBufferTexture(uint32_t index) = 0;
    virtual WeakPointer<IRenderTarget2D> GetBufferRenderTarget(uint32_t index) = 0;
    virtual uint32_t GetBufferCount() const = 0;
    virtual uint32_t GetCurrentBackBufferIndex() const = 0;

private:
    WeakPointer<Windowing::Window> AssociatedWindow;
    ResourceDataFormat Format;
};

} // namespace Edvar::Renderer::RHI