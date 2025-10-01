#pragma once

#include "meta/meta.h"
#include "platform/basic_types.h"
namespace edvar {
static constexpr uint64 FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64 FNV_PRIME = 1099511628211ULL;

inline uint64 hash(const bool& v) { return v ? 1 : 0; }
inline uint64 hash(const int8& v) { return static_cast<uint64>(v); }
inline uint64 hash(const uint8& v) { return static_cast<uint64>(v); }
inline uint64 hash(const int16& v) { return static_cast<uint64>(v); }
inline uint64 hash(const uint16& v) { return static_cast<uint64>(v); }
inline uint64 hash(const int32& v) { return static_cast<uint64>(v); }
inline uint64 hash(const uint32& v) { return static_cast<uint64>(v); }
inline uint64 hash(const int64& v) { return static_cast<uint64>(v); }
inline uint64 hash(const uint64& v) { return static_cast<uint64>(v); }
inline uint64 hash(const float& v) {
    uint32 as_int = *reinterpret_cast<const uint32*>(&v);
    return static_cast<uint64>(as_int);
}

inline uint64 hash(const double& v) { return *reinterpret_cast<const uint64*>(&v); }

inline uint64 hash(const long double& v) {
    const uint8* bytes = reinterpret_cast<const uint8*>(&v);
    uint64 h = FNV_OFFSET_BASIS;
    for (uint32 i = 0; i < sizeof(long double); ++i) {
        h ^= bytes[i];
        h *= FNV_PRIME;
    }
    return h;
}

inline uint64 hash(const char_utf8& v) { return static_cast<uint64>(v); }
inline uint64 hash(const char_utf16& v) { return static_cast<uint64>(v); }
inline uint64 hash(const char_utf32& v) { return static_cast<uint64>(v); }

template <typename type>
inline edvar::meta::enable_if<edvar::meta::has_hash_function<type>, uint64> hash(const type& v) {
    return v.get_hash();
}

// Pointer hash overload: matches all pointer types
inline uint64 hash(const void* v) {
    return hash(reinterpret_cast<uint64>(v)); // treat pointer as 64-bit integer
}
} // namespace edvar