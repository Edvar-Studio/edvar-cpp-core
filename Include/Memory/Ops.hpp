#pragma once
#include <cstdint>

namespace Edvar::Memory {
template <typename T> inline void ZeroMemory(T* Ptr, uint64_t Count) {
    auto* BytePtr = reinterpret_cast<unsigned char*>(Ptr);
    for (uint64_t i = 0; i < Count * sizeof(T); ++i) {
        BytePtr[i] = 0;
    }
}

template <typename T> inline void CopyMemory(T* Dest, const T* Src, uint64_t Count) {
    auto* DestBytePtr = reinterpret_cast<unsigned char*>(Dest);
    const auto* SrcBytePtr = reinterpret_cast<const unsigned char*>(Src);
    for (uint64_t i = 0; i < Count * sizeof(T); ++i) {
        DestBytePtr[i] = SrcBytePtr[i];
    }
}

template <typename T> inline void SetMemory(T* Ptr, uint8_t Value, uint64_t Count) {
    auto* BytePtr = reinterpret_cast<unsigned char*>(Ptr);
    for (uint64_t i = 0; i < Count * sizeof(T); ++i) {
        BytePtr[i] = Value;
    }
}

template <typename T> inline int32_t CompareMemory(const T* Ptr1, const T* Ptr2, uint64_t Count) {
    const auto* BytePtr1 = reinterpret_cast<const unsigned char*>(Ptr1);
    const auto* BytePtr2 = reinterpret_cast<const unsigned char*>(Ptr2);
    for (uint64_t i = 0; i < Count * sizeof(T); ++i) {
        if (BytePtr1[i] != BytePtr2[i]) {
            return static_cast<int32_t>(BytePtr1[i]) - static_cast<int32_t>(BytePtr2[i]);
        }
    }
    return 0;
}

} // namespace Edvar::Memory