#include "Renderer/RHI/D3D12/D3D12CommandAllocator.hpp"
#ifdef _WIN32
#    include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"

#    include <d3d12.h>

namespace Edvar::Renderer::RHI::D3D12 {

D3D12CommandAllocator::D3D12CommandAllocator(const SharedReference<IRenderDevice>& device,
                                             const CommandListType inQueueType)
    : ICommandAllocator(device, inQueueType) {
    SharedReference<D3D12RenderDevice> rhiDevice = StaticCastSharedReference<D3D12RenderDevice>(device);
    ID3D12Device* nativeDevice = static_cast<ID3D12Device*>(rhiDevice->NativeHandle);
    D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
    switch (inQueueType) {
    case CommandListType::Graphics:
        commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    case CommandListType::Compute:
        commandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    case CommandListType::Copy:
        commandListType = D3D12_COMMAND_LIST_TYPE_COPY;
        break;
    default:
        Platform::GetPlatform().OnFatalError(u"D3D12CommandAllocator: Unknown CommandQueue Type.");
        return;
    }
    ID3D12CommandAllocator* allocator = nullptr;
    if (FAILED(nativeDevice->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&allocator)))) {
        Platform::GetPlatform().OnFatalError(u"D3D12CommandAllocator::CreateCommandAllocator failed.");
    }
    nativePtr = allocator;
}
D3D12CommandAllocator::~D3D12CommandAllocator() {
    if (nativePtr) {
        ID3D12CommandAllocator* allocator = static_cast<ID3D12CommandAllocator*>(nativePtr);
        allocator->Release();
        nativePtr = nullptr;
    }
}
void D3D12CommandAllocator::Reset() {
    if (nativePtr) [[likely]] {
        if (ID3D12CommandAllocator* allocator = static_cast<ID3D12CommandAllocator*>(nativePtr);
            FAILED(allocator->Reset())) [[unlikely]] {
            Platform::GetPlatform().OnFatalError(u"D3D12CommandAllocator::Reset failed.");
        }
    } else {
        Platform::GetPlatform().OnFatalError(u"D3D12CommandAllocator::Reset called on null nativePtr.");
    }
}
} // namespace Edvar::Renderer::RHI::D3D12
#endif