#pragma once

namespace std {
struct nothrow_t;
}
namespace Edvar::Memory {
#if WITH_MIMALLOC
class EDVAR_CPP_CORE_API MiMemoryAllocator {
public:
    void* Allocate(uint64_t size, uint64_t alignment = alignof(std::max_align_t), uint64_t offset = 0);

    void Free(void* ptr, uint64_t alignment = alignof(std::max_align_t));
    void FreeSize(void* ptr, uint64_t size, uint64_t alignment = alignof(std::max_align_t));
};
typedef MiMemoryAllocator IMemoryAllocator;

#else
// WARNING: CMemoryAllocator not actually implements all the alignement, size and offset features. It currently just
// calls malloc with the specified size, free with just the ptr. Be careful when using it.
class EDVAR_CPP_CORE_API CMemoryAllocator {
public:
    void* Allocate(uint64_t size, uint64_t alignment = alignof(std::max_align_t), uint64_t offset = 0);

    void Free(void* ptr, uint64_t alignment = alignof(std::max_align_t));
    void FreeSize(void* ptr, uint64_t size, uint64_t alignment = alignof(std::max_align_t));
};

typedef CMemoryAllocator IMemoryAllocator;
#endif

EDVAR_CPP_CORE_API IMemoryAllocator& GetGlobalMemoryAllocator();

} // namespace Edvar::Memory

inline void operator delete(void* p) noexcept { Edvar::Memory::GetGlobalMemoryAllocator().Free(p); }
inline void operator delete[](void* p) noexcept { Edvar::Memory::GetGlobalMemoryAllocator().Free(p); }

inline void operator delete(void* p, const std::nothrow_t&) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().Free(p);
}
inline void operator delete[](void* p, const std::nothrow_t&) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().Free(p);
}
#if defined(_MSC_VER) && defined(_Ret_notnull_) && defined(_Post_writable_byte_size_)
// stay consistent with VCRT definitions
#    define DeclNew(n)                                                                                                 \
        [[nodiscard]] __declspec(allocator) __declspec(restrict) _Ret_notnull_ _Post_writable_byte_size_(n)
#    define DeclNewNoThrow(n)                                                                                          \
        [[nodiscard]] __declspec(allocator) __declspec(restrict) _Ret_maybenull_ _Success_(return != NULL)             \
            _Post_writable_byte_size_(n)
#else
#    define DeclNew(n) [[nodiscard]]
#    define DeclNewNoThrow(n) [[nodiscard]]
#endif

DeclNew(n) inline void* operator new(std::size_t n) noexcept(false) {
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n);
}
DeclNew(n) inline void* operator new[](std::size_t n) noexcept(false) {
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n);
}

DeclNewNoThrow(n) inline void* operator new(std::size_t n, const std::nothrow_t& tag) noexcept {
    (void)(tag);
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n);
}
DeclNewNoThrow(n) inline void* operator new[](std::size_t n, const std::nothrow_t& tag) noexcept {
    (void)(tag);
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n);
}

inline void operator delete(void* p, std::size_t n) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().FreeSize(p, n);
}
inline void operator delete[](void* p, std::size_t n) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().FreeSize(p, n);
}
inline void operator delete(void* p, std::align_val_t al) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().Free(p, static_cast<uint64_t>(al));
}
inline void operator delete[](void* p, std::align_val_t al) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().Free(p, static_cast<uint64_t>(al));
}
inline void operator delete(void* p, std::size_t n, std::align_val_t al) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().FreeSize(p, n, static_cast<uint64_t>(al));
};
inline void operator delete[](void* p, std::size_t n, std::align_val_t al) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().FreeSize(p, n, static_cast<uint64_t>(al));
};
inline void operator delete(void* p, std::align_val_t al, const std::nothrow_t&) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().Free(p, static_cast<uint64_t>(al));
}
inline void operator delete[](void* p, std::align_val_t al, const std::nothrow_t&) noexcept {
    Edvar::Memory::GetGlobalMemoryAllocator().Free(p, static_cast<uint64_t>(al));
}

inline void* operator new(std::size_t n, std::align_val_t al) noexcept(false) {
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n, static_cast<uint64_t>(al));
}
inline void* operator new[](std::size_t n, std::align_val_t al) noexcept(false) {
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n, static_cast<uint64_t>(al));
}
inline void* operator new(std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept {
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n, static_cast<uint64_t>(al));
}
inline void* operator new[](std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept {
    return Edvar::Memory::GetGlobalMemoryAllocator().Allocate(n, static_cast<uint64_t>(al));
}
#undef DeclNew
#undef DeclNewNoThrow