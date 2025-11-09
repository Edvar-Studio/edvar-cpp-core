#pragma once

#include <cstdint>
namespace edvar {
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t hash(const bool& v) { return v ? 1 : 0; }
inline uint64_t hash(const int8_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const uint8_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const int16_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const uint16_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const int32_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const uint32_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const int64_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const uint64_t& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const float& v) {
    uint32_t as_int = *reinterpret_cast<const uint32_t*>(&v);
    return static_cast<uint64_t>(as_int);
}

inline uint64_t hash(const double& v) { return *reinterpret_cast<const uint64_t*>(&v); }

inline uint64_t hash(const long double& v) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&v);
    uint64_t h = FNV_OFFSET_BASIS;
    for (uint32_t i = 0; i < sizeof(long double); ++i) {
        h ^= bytes[i];
        h *= FNV_PRIME;
    }
    return h;
}

inline uint64_t hash(const char_utf8& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const char_utf16& v) { return static_cast<uint64_t>(v); }
inline uint64_t hash(const char_utf32& v) { return static_cast<uint64_t>(v); }

template <typename in_type>
concept has_hash_method = requires(const in_type& v) {
    { v.get_hash() } -> std::same_as<uint64_t>;
};

template <typename in_type>
inline uint64_t hash(const in_type& v)
    requires has_hash_method<in_type>
{
    return v.get_hash();
}

// Pointer hash overload: matches all pointer types
inline uint64_t hash(const void* v) {
    return hash(reinterpret_cast<uint64_t>(v)); // treat pointer as 64-bit integer
}
} // namespace edvar