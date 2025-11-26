#pragma once
#if WITH_MIMALLOC
#    include "Memory/IMemoryAllocator.hpp"
#    include "mimalloc.h"
#    include <cstddef>

namespace Edvar::Memory {
class MiMemoryAllocator : public IMemoryAllocator {
public:
    void* Allocate(const uint64_t size, const uint64_t alignment, const uint64_t offset) override {
        return mi_malloc_aligned_at(size, alignment, offset);
    }
    void Free(void* ptr) override { mi_free(ptr); }
    ~MiMemoryAllocator() override = default;
};
} // namespace Edvar::Memory
#endif