#pragma once

#include "Memory/IMemoryAllocator.hpp"
#include <cstdlib>

namespace Edvar::Memory {
class CMemoryAllocator : public IMemoryAllocator {
public:
    virtual void* Allocate(uint64_t size, uint64_t alignment = alignof(std::max_align_t),
                           uint64_t offset = 0) override {
        return std::malloc(size);
    }
    virtual void Free(void* ptr) override { return std::free(ptr); }
    virtual ~CMemoryAllocator() = default;
};
} // namespace Edvar::Memory