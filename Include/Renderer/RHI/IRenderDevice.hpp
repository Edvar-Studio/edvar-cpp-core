#pragma once
#include "ICommandQueue.hpp"
#include "ISwapchain.hpp"

namespace Edvar::Renderer::RHI {
class ICommandAllocator;
class ICommandList;
class ICommandQueue;
class IFence;

enum class ShaderModel : uint8_t { SM5 = 5, SM6 = 6 };

class EDVAR_CPP_CORE_API IRenderDevice : public Memory::EnableSharedFromThis<IRenderDevice> {
public:
    virtual bool HasShaderModelSupport(ShaderModel model) const = 0;
    virtual bool HasRayTracingSupport() const = 0;

    virtual void DoCheckDeviceSanity() const = 0;

    virtual void Destroy() {};
    virtual SharedReference<ISwapchain> CreateSwapchain(const Windowing::Window& window,
                                                        ResourceDataFormat resourceDataFormat) = 0;
    virtual SharedReference<ICommandAllocator> CreateCommandAllocator(CommandListType type) = 0;
    virtual SharedReference<ICommandList> CreateCommandList(CommandListType type,
                                                            const SharedReference<ICommandAllocator>& allocator) = 0;



    virtual SharedPointer<ICommandQueue> GetGraphicsQueue() = 0;
    virtual SharedPointer<ICommandQueue> GetComputeQueue() = 0;
    virtual SharedPointer<ICommandQueue> GetCopyQueue() = 0;

private:
    friend class IRenderDeviceDependent;
    Containers::List<IRenderDeviceDependent*> dependents;
};
} // namespace Edvar::Renderer::RHI