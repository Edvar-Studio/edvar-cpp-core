#pragma once

namespace Edvar::Containers::Allocators {
template <typename T> class DefaultAllocator {
public:
    static constexpr uint64_t Alignment = alignof(T);
    void Allocate(const uint64_t Count) {
        DataPtr = static_cast<T*>(::operator new(sizeof(T) * Count, static_cast<std::align_val_t>(Alignment)));
        AllocatedSize = Count;
    }

    void Resize(const uint64_t NewCount) {
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

    T& operator[](const uint64_t Index) { return DataPtr[Index]; }

    const T& operator[](const uint64_t Index) const { return DataPtr[Index]; }

    ~DefaultAllocator() { ::operator delete(DataPtr, Alignment); }

    T* Data() { return DataPtr; }
    const T* Data() const { return DataPtr; }

    [[nodiscard]] bool HasAllocated() const { return AllocatedSize > 0; }

private:
    T* DataPtr = nullptr;
    uint64_t AllocatedSize = 0;
};
} // namespace Edvar::Containers::Allocators