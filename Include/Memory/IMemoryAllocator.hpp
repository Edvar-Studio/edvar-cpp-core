#pragma once

#include <cstddef>
#include <cstdint>
namespace Edvar::Memory {
class IMemoryAllocator {

public:
    virtual void* Allocate(uint64_t size, uint64_t alignment = alignof(std::max_align_t), uint64_t offset = 0) = 0;
    virtual void Free(void* ptr) = 0;

protected:
    virtual ~IMemoryAllocator() = default;
};
} // namespace Edvar::Memory