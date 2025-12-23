#include "Renderer/CommandContext.hpp"

#include "Renderer/RHI/IRenderDevice.hpp"

namespace Edvar::Renderer {

CommandContext::CommandContext(const Memory::SharedReference<RHI::ICommandQueue>& queue,
                               const Memory::SharedReference<RHI::ICommandAllocator>& allocator,
                               const Memory::SharedReference<RHI::ICommandList>& list)
    : commandQueue(queue), commandAllocator(allocator), commandList(list) {}
CommandContextManager::CommandContextManager() {}
CommandContextManager& CommandContextManager::Get() {
    static CommandContextManager instance;
    return instance;
}
CommandContext& CommandContextManager::AcquireContext(CommandListType type) {
    // see if there is an available context in the pool
    for (CommandContext& context : createdContexts) {
        if (!context.inUse.Load()) {
            continue;
        }
        if (context.commandQueue->GetType() == type) {
            context.inUse.Store(true);
            return context;
        }
    }
    // create a new context
    const auto primaryDevice = RHI::IRenderingAPI::GetActiveAPI()->GetPrimaryDevice();
    SharedPointer<RHI::ICommandQueue> commandQueue;
    switch (type) {
    default:
    case CommandListType::Graphics:
        commandQueue = primaryDevice->GetGraphicsQueue();
        break;
    case CommandListType::Compute:
        break;
    case CommandListType::Copy:
        break;
    }
    const SharedPointer<RHI::ICommandAllocator> allocator = primaryDevice->CreateCommandAllocator(type);
    const SharedPointer<RHI::ICommandList> commandList = primaryDevice->CreateCommandList(type, allocator);
    const CommandContext newContext(commandQueue, allocator, commandList);
    createdContexts.Add(newContext);
    return createdContexts.Get(createdContexts.Length() - 1);
}
void CommandContextManager::ReleaseContext(const CommandContext& context) {
    // find in the createdContexts.
    if (const int32_t i = createdContexts.IndexOf(context); i != -1) {
        createdContexts.Get(i).inUse.Store(false);
    }
}
CommandContext& CommandContextManager::GetImmediateContext() {
    if (immediateContext == nullptr) [[unlikely]] {
        immediateContext = &AcquireContext(CommandListType::Graphics);
    }
    return *immediateContext;
}
void CommandContextManager::FlushAll() {

}
} // namespace Edvar::Renderer