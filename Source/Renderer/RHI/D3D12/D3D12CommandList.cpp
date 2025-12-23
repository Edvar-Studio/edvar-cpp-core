#include "Renderer/RHI/D3D12/D3D12CommandList.hpp"
#ifdef _WIN32
#    include "Renderer/RHI/D3D12/D3D12CommandAllocator.hpp"
#    include "Renderer/RHI/D3D12/D3D12RenderDevice.hpp"

#    include <d3d12.h>

namespace Edvar::Renderer::RHI::D3D12 {
D3D12CommandList::D3D12CommandList(const SharedReference<IRenderDevice>& device, const CommandListType& inType,
                                   const SharedReference<ICommandAllocator>& inCommandAllocator)
    : ICommandList(device, inType, inCommandAllocator) {
    const auto rhiDevice = StaticCastSharedReference<D3D12RenderDevice>(device);
    auto* nativeDevice = static_cast<ID3D12Device*>(rhiDevice->NativeHandle);
    D3D12_COMMAND_LIST_TYPE commandListType;
    switch (inType) {
    default:
    case CommandListType::Graphics:
        commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    case CommandListType::Compute:
        commandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    case CommandListType::Copy:
        commandListType = D3D12_COMMAND_LIST_TYPE_COPY;
        break;
    }
    auto* nativeCommandAllocator = static_cast<ID3D12CommandAllocator*>(
        StaticCastSharedReference<D3D12CommandAllocator>(inCommandAllocator)->nativePtr);
    ID3D12GraphicsCommandList* commandList = nullptr;
    if (FAILED(nativeDevice->CreateCommandList(0, commandListType, nativeCommandAllocator, nullptr,
                                               IID_PPV_ARGS(&commandList)))) {
        Platform::GetPlatform().OnFatalError(
            *String::Format(u"D3D12: Failed to create command list of type {}.\n", static_cast<uint8_t>(inType)));
    }
    nativePtr = commandList;
    Close();
}
} // namespace Edvar::Renderer::RHI::D3D12
#endif