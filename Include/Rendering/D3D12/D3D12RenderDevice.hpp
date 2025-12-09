#pragma once
#ifdef _WIN32

#    include "../IRenderDevice.hpp"
namespace Edvar::Rendering::D3D12 {
class D3D12RenderDevice : public IRenderDevice {
public:
    explicit D3D12RenderDevice(void* nativeDeviceHandle) : nativeHandle(nativeDeviceHandle) {}
    ~D3D12RenderDevice() override;
    void DoCheckDeviceSanity() const override;
    bool HasShaderModelSupport(ShaderModel model) const override;
    bool HasRayTracingSupport() const override;
    void Destroy() override;
    SharedReference<ISwapchain> CreateSwapchain(const Windowing::Window& window,
                                                ResourceDataFormat resourceDataFormat) override;

    void* nativeHandle;
};
} // namespace Edvar::Rendering::D3D12

#endif