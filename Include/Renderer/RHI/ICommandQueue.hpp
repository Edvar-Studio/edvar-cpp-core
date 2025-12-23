#pragma once
#include "RenderingCore.hpp"

namespace Edvar::Renderer::RHI {
enum class CommandListType : uint8_t { Graphics, Compute, Copy };
class ICommandQueue : public IRenderDeviceDependent {
public:
    ICommandQueue(const RenderDeviceRef& renderDevice, const bool isHighPriority = true,
                  CommandListType type = CommandListType::Graphics)
        : IRenderDeviceDependent(renderDevice), queueIsHighPriority(isHighPriority), queueType(type) {}
    virtual ~ICommandQueue() override = default;

    bool IsHighPriority() const { return queueIsHighPriority; }
    CommandListType GetType() const { return queueType; }

protected:
    bool queueIsHighPriority;
    CommandListType queueType;
};
} // namespace Edvar::Rendering