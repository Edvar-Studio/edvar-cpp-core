#pragma once

#include "../Memory/SmartPointers.hpp"
#include "RHI/ICommandAllocator.hpp"
#include "RHI/ICommandQueue.hpp"
#include "RHI/ICommandList.hpp"

namespace Edvar::Renderer {
using RHI::CommandListType;
// Command context encapsulates a command queue, command allocator, and command list for rendering operations.
class CommandContext final {
public:
    CommandContext(const Memory::SharedReference<RHI::ICommandQueue>& queue,
                   const Memory::SharedReference<RHI::ICommandAllocator>& allocator,
                   const Memory::SharedReference<RHI::ICommandList>& list);

    Memory::SharedReference<RHI::ICommandQueue> commandQueue;
    Memory::SharedReference<RHI::ICommandAllocator> commandAllocator;
    Memory::SharedReference<RHI::ICommandList> commandList;

    Memory::Atomic<bool> inUse = true;

    bool operator==(const CommandContext& other) const {
        return commandQueue == other.commandQueue && commandAllocator == other.commandAllocator &&
               commandList == other.commandList;
    }

    // Allow direct access to the command list's members.
    RHI::ICommandList* operator->() const { return commandList.Get(); }
};

class CommandContextManager {
    CommandContextManager();

public:
    static CommandContextManager& Get();

    CommandContext& AcquireContext(CommandListType type);
    void ReleaseContext(const CommandContext& context);

    // Gets the immediate use ready context.
    CommandContext& GetImmediateContext();

    void FlushAll();
private:
    Containers::List<CommandContext> createdContexts;
    CommandContext* immediateContext = nullptr;
};

} // namespace Edvar::Renderer