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

} // namespace Edvar::Memory