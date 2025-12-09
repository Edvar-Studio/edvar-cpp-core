#pragma once

#include "RenderingCore.hpp"
#include "ResourceDataFormat.hpp"
namespace Edvar::Windowing {
class Window;
}
namespace Edvar::Rendering {
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

private:
    WeakPointer<Windowing::Window> AssociatedWindow;
    ResourceDataFormat Format;
};

} // namespace Edvar::Rendering