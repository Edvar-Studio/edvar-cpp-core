#pragma once
// TODO: remove the #include below after bugs are cleaned.
#if defined(_DEBUG) && EDVAR_CPP_CORE_ALLOCATOR_TRACING
#    define EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED 1
#else
#    define EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED 0
#endif
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
#    include <stdio.h>
#endif
namespace Edvar::Containers::Allocators {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
EDVAR_CPP_CORE_API int AllocateAllocatorID();
#endif
template <typename T> class DefaultAllocator {
public:
    static constexpr uint64_t Alignment = alignof(T);

#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
    int AllocatorId;
#endif
    DefaultAllocator() {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        AllocatorId = AllocateAllocatorID();
        printf("Allocator %d: Created allocator\n", AllocatorId);
#endif
    }
    DefaultAllocator(const DefaultAllocator&) = delete;
    DefaultAllocator& operator=(const DefaultAllocator&) = delete;
    DefaultAllocator(DefaultAllocator&& other) noexcept : DataPtr(other.DataPtr), AllocatedSize(other.AllocatedSize) {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: Moving to allocator %d\n", AllocatorId, other.AllocatorId);
#endif
        other.DataPtr = nullptr;
        other.AllocatedSize = 0;
    }
    DefaultAllocator& operator=(DefaultAllocator&& other) noexcept {
        if (this != &other) {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
            printf("Allocator %d: Moving allocator from allocator %d\n", AllocatorId, other.AllocatorId);
            AllocatorId = other.AllocatorId;
#endif
            other.DataPtr = nullptr;
            other.AllocatedSize = 0;
        }
        return *this;
    }
    void Allocate(const uint64_t Count) {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: allocation of size %zu with alignment %zu\n", AllocatorId, sizeof(T) * Count, Alignment);
#endif
        DataPtr = static_cast<T*>(::operator new(sizeof(T) * Count, static_cast<std::align_val_t>(Alignment)));
        AllocatedSize = Count;
    }

    void Resize(const uint64_t NewCount) {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: resize of size %zu with alignment %zu\n", AllocatorId, sizeof(T) * NewCount, Alignment);
#endif
        T* NewDataPtr = static_cast<T*>(::operator new(sizeof(T) * NewCount, static_cast<std::align_val_t>(Alignment)));
        const uint64_t CopyCount = (NewCount < AllocatedSize) ? NewCount : AllocatedSize;
        if constexpr (std::is_move_constructible_v<T>) {
            for (uint64_t i = 0; i < CopyCount; ++i) {
                new (NewDataPtr + i) T(std::move(DataPtr[i]));
                DataPtr[i].~T();
            }
        } else {
            for (uint64_t i = 0; i < CopyCount; ++i) {
                new (NewDataPtr + i) T(DataPtr[i]);
                DataPtr[i].~T();
            }
        }
        ::operator delete(DataPtr);
        DataPtr = NewDataPtr;
        AllocatedSize = NewCount;
    }

    T& operator[](const uint64_t Index) {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: Access to element with idx %llu\n", AllocatorId, Index);
#endif
        return DataPtr[Index];
    }

    const T& operator[](const uint64_t Index) const {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: Const access to element with idx %llu\n", AllocatorId, Index);
#endif
        return DataPtr[Index];
    }

    ~DefaultAllocator() {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        __debugbreak();
        printf("Allocator %d: Destroying allocator\n", AllocatorId);
#endif
        ::operator delete(DataPtr, Alignment);
        DataPtr = nullptr;
    }

    T* Data() {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: Raw access to data\n", AllocatorId);
#endif
        return DataPtr;
    }
    const T* Data() const {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: Raw access to const data\n", AllocatorId);
#endif
        return DataPtr;
    }

    [[nodiscard]] bool HasAllocated() const {
#if EDVAR_CPP_CORE_ALLOCATOR_TRACING_ENABLED
        printf("Allocator %d: Queried has allocated. %s\n", AllocatorId, AllocatedSize > 0 ? "true" : "false");
#endif
        return AllocatedSize > 0;
    }

private:
    T* DataPtr = nullptr;
    uint64_t AllocatedSize = 0;
};
} // namespace Edvar::Containers::Allocators