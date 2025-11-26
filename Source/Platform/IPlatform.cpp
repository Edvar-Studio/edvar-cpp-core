#include "Platform/IPlatform.hpp"

#include "unicode/ustring.h"
#if WITH_MIMALLOC
#    include "mimalloc-new-delete.h" // IWYU pragma: keep
#    include "Memory/MiMemoryAllocator.hpp"
#else
#    include "Memory/CMemoryAllocator.hpp"
#endif


/**
 * Add include of the platform headers here to allow them to be selected.
 */
#include "Platform/Windows/WindowsPlatform.hpp"

namespace Edvar::Platform {

Memory::IMemoryAllocator* IPlatform::GetAllocator() const {
#if WITH_MIMALLOC
    static Memory::MiMemoryAllocator allocator;
    return &allocator;
#else
    static Memory::CMemoryAllocator allocator;
    return &allocator;
#endif
}

void IPlatform::Abort() { std::abort(); }
void IPlatform::OnFatalError(const char16_t* message) {
    Abort();
}

IPlatform& Get() {
    static PlatformType platform;
    return platform;
}
} // namespace Edvar::Platform
