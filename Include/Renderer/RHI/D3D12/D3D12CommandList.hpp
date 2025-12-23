#pragma once
#ifdef _WIN32
#    include "../ICommandList.hpp"

namespace Edvar::Renderer::RHI {
enum class CommandListType : uint8_t;
class ICommandAllocator;
} // namespace Edvar::Renderer::RHI
namespace Edvar::Renderer::RHI::D3D12 {
class D3D12CommandList : public ICommandList {
public:
    D3D12CommandList(const SharedReference<IRenderDevice>& device, const CommandListType& inType,
                     const SharedReference<ICommandAllocator>& inCommandAllocator);
    void* nativePtr = nullptr;
};
} // namespace Edvar::Renderer::RHI::D3D12
#endif