#pragma once

#include "meta/meta.h"
namespace edvar {
static constexpr uint64 FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64 FNV_PRIME = 1099511628211ULL;

template <typename type> uint64 hash(const type& v) {
    // shouldn't enter here
    static_assert(edvar::meta::has_hash_function<type>, "Type does not have a hash function defined");
    return 0;
}

template <typename type> inline edvar::meta::enable_if<edvar::meta::is_integral<type>, uint64> hash(const type& v) {
    return static_cast<uint64>(v);
}

template <typename type>
inline edvar::meta::enable_if<edvar::meta::is_floating_point<type>, uint64> hash(const type& v) {
    if constexpr (edvar::meta::is_same_type<type, float>) {
        uint32 as_int = *reinterpret_cast<const uint32*>(&v);
        return static_cast<uint64>(as_int);
    } else if constexpr (edvar::meta::is_same_type<type, double>) {
        uint64 as_int = *reinterpret_cast<const uint64*>(&v);
        return as_int;
    } else { // long double
        // Note: This is a naive implementation and may not cover all cases.
        const uint8* bytes = reinterpret_cast<const uint8*>(&v);
        uint64 h = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < sizeof(long double); ++i) {
            h ^= bytes[i];
            h *= FNV_PRIME;
        }
        return h;
    }
}

template <typename type> inline edvar::meta::enable_if<edvar::meta::is_character<type>, uint64> hash(const type& v) {
    return static_cast<uint64>(v);
}

template <typename type>
inline edvar::meta::enable_if<edvar::meta::has_hash_function<type>, uint64> hash(const type& v) {
    return v.get_hash();
}
} // namespace edvar