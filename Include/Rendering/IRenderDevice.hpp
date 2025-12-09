#pragma once
#include "ISwapchain.hpp"

namespace Edvar::Rendering {
class ICommandAllocator;
class ICommandList;
class ICommandQueue;
class IFence;

enum class ShaderModel : uint8_t { SM5 = 5, SM6 = 6 };

class IRenderDevice : public Memory::EnableSharedFromThis<IRenderDevice> {
public:
    virtual bool HasShaderModelSupport(ShaderModel model) const = 0;
    virtual bool HasRayTracingSupport() const = 0;

    virtual void DoCheckDeviceSanity() const = 0;

    virtual void Destroy() {};
    virtual SharedReference<ISwapchain> CreateSwapchain(const Windowing::Window& window,
                                                        ResourceDataFormat resourceDataFormat) = 0;

private:
    friend class IRenderDeviceDependent;
    Containers::List<WeakPointer<IRenderDeviceDependent>> dependents;
};
} // namespace Edvar::Rendering