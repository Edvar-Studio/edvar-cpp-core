#pragma once

#include "ResourceDataFormat.hpp"
namespace Edvar::Windowing {
class Window;
}
namespace Edvar::Rendering {
class ISwapchain : Memory::EnableSharedFromThis<ISwapchain> {
public:
    virtual ~ISwapchain() = default;
    virtual void Present(uint8_t interval = 0) = 0;

    virtual void SetFullscreen(bool fullscreen, const Math::Vector2i& newSize = (-Math::Vector2i::One)) = 0;
    virtual bool IsFullscreen() const = 0;
    virtual Math::Vector2i GetSize() const = 0;
    virtual void Resize(const Math::Vector2i& newSize, uint8_t bufferCount, const ResourceDataFormat& newFormat) = 0;
    virtual void SetBackgroundColor(const Math::Color& color) = 0;

    static SharedPointer<ISwapchain> CreateForWindow(Windowing::Window& window);
};
} // namespace Edvar::Rendering