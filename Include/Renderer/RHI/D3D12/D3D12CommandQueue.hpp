#pragma once

#ifdef _WIN32
#    include "../ICommandQueue.hpp"
namespace Edvar::Renderer::RHI::D3D12 {
class D3D12CommandQueue final : public ICommandQueue {
public:
    explicit D3D12CommandQueue(const RenderDeviceRef& renderDevice, bool isHighPriority = true,
                               CommandListType type = CommandListType::Graphics);
    ~D3D12CommandQueue() override;

    void CreateNativeCommandQueue();
    void* nativeHandle{};
};
} // namespace Edvar::Rendering::D3D12
#endif