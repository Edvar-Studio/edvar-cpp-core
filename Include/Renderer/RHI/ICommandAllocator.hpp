#pragma once
#include "ICommandQueue.hpp"
#include "RenderingCore.hpp"

namespace Edvar::Renderer::RHI {
class ICommandAllocator : public IRenderDeviceDependent {
public:
    explicit ICommandAllocator(const SharedReference<IRenderDevice>& device, CommandListType inQueueType)
        : IRenderDeviceDependent(device), queueType(inQueueType) {}
    ~ICommandAllocator() override = default;

    CommandListType GetQueueType() const { return queueType; }
    virtual void Reset() = 0;

private:
    CommandListType queueType;
};
} // namespace Edvar::Renderer::RHI