#pragma once

#ifdef _WIN32
#    include "../ISwapchain.hpp"
namespace Edvar::Rendering {
class IRenderDevice;
}
namespace Edvar::Rendering::D3D12 {
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
    void* nativePtr;
};
} // namespace Edvar::Rendering::D3D12

#endif