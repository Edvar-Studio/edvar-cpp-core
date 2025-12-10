#include "Containers/Allocators/DefaultAllocator.hpp"
namespace Edvar::Containers::Allocators {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
int AllocateAllocatorID() {
    static int NextAllocatorID = 1;
    return NextAllocatorID++;
}
#endif
} // namespace Edvar::Containers::Allocators