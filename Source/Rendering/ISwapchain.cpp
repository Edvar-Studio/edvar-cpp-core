#include "Rendering/ISwapchain.hpp"

#include "Windowing/Window.hpp"

namespace Edvar::Rendering {
ISwapchain::ISwapchain(const RenderDeviceRef& RenderDevice, const Windowing::Window& forWindow,
                       const ResourceDataFormat dataFormat)
    : IRenderDeviceDependent(RenderDevice), AssociatedWindow(forWindow.AsShared()), Format(dataFormat) {}
} // namespace Edvar::Rendering
