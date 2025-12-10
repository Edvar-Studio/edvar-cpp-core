#include "Memory/MemoryAllocator.hpp"
#if WITH_MIMALLOC
#    include "mimalloc.h"
#else
#    include <cstdlib>
#endif
namespace Edvar::Memory {
IMemoryAllocator GMemoryAllocator;

IMemoryAllocator& GetGlobalMemoryAllocator() { return GMemoryAllocator; }
#if WITH_MIMALLOC
void* MiMemoryAllocator::Allocate(const uint64_t size, const uint64_t alignment, const uint64_t offset) {
    return mi_malloc_aligned_at(size, alignment, offset);
}
void MiMemoryAllocator::Free(void* ptr, uint64_t alignment) { return mi_free_aligned(ptr, alignment); }
void MiMemoryAllocator::FreeSize(void* ptr, uint64_t size, uint64_t alignment) {
    return mi_free_size_aligned(ptr, size, alignment);
}
#else
void* CMemoryAllocator::Allocate(uint64_t size, uint64_t alignment, uint64_t offset) { return std::malloc(size); }
void CMemoryAllocator::Free(void* ptr, uint64_t alignment) { return std::free(ptr); }
void CMemoryAllocator::FreeSize(void* ptr, uint64_t size, uint64_t alignment) { return std::fre(ptr); }
#endif

} // namespace Edvar::Memory