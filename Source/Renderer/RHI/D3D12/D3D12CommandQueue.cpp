#include "Renderer/RHI/D3D12/D3D12CommandQueue.hpp"
#ifdef _WIN32
#    include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"

#    include <d3d12.h>

namespace Edvar::Renderer::RHI::D3D12 {
D3D12CommandQueue::D3D12CommandQueue(const RenderDeviceRef& renderDevice, bool isHighPriority, CommandListType type)
    : ICommandQueue(renderDevice, isHighPriority, type) {
    CreateNativeCommandQueue();
}
D3D12CommandQueue::~D3D12CommandQueue() {
    if (nativeHandle) {
        static_cast<ID3D12CommandQueue*>(nativeHandle)->Release();
        nativeHandle = nullptr;
    }
}
void D3D12CommandQueue::CreateNativeCommandQueue() {
    const SharedPointer<D3D12RenderDevice> device = StaticCastSharedReference<D3D12RenderDevice>(GetAssociatedDevice());
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (queueType == CommandListType::Compute) {
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    } else if (queueType == CommandListType::Copy) {
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    }
    queueDesc.Priority = queueIsHighPriority ? D3D12_COMMAND_QUEUE_PRIORITY_HIGH : D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags =
        queueIsHighPriority ? D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT : D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;
    ID3D12CommandQueue* commandQueue = nullptr;
    auto* d3Device = static_cast<ID3D12Device*>(device->NativeHandle);
    if (FAILED(d3Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) {
        CheckDeviceSanity();
        return;
    }
    nativeHandle = commandQueue;
}
} // namespace Edvar::Rendering::D3D12
#endif