#pragma once
#ifdef _WIN32
#    include "../ICommandAllocator.hpp"

namespace Edvar::Renderer::RHI::D3D12 {
class D3D12CommandAllocator final : public ICommandAllocator {
public:
    D3D12CommandAllocator(const SharedReference<IRenderDevice>& device, const CommandListType inQueueType);
    ~D3D12CommandAllocator() override;

    void Reset() override;

    void* nativePtr;
};
} // namespace Edvar::Renderer::RHI::D3D12

#endif