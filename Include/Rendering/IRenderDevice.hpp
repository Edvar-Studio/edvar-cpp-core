#pragma once

#include "Memory/SmartPointers.hpp"
namespace Edvar::Rendering {
class ICommandAllocator;
class ICommandList;
class ICommandQueue;
class IFence;

enum class ShaderModel : uint8_t { SM5 = 5, SM6 = 6 };
class IRenderDevice : Memory::EnableSharedFromThis<IRenderDevice> {
public:
    bool HasShaderModelSupport(ShaderModel model) const;
    bool HasRayTracingSupport() const;
};
} // namespace Edvar::Rendering