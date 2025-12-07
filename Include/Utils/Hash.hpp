#pragma once

namespace Edvar::Utils {
static constexpr uint64_t FNV1A_64_OFFSET_BASIS = 0xCBF29CE484222325;
static constexpr uint64_t FNV1A_64_PRIME = 0x100000001B3;
static constexpr uint64_t KNUTH_CONSTANT_64 = 0x9e3779b97f4a7c15;

template <typename T> EDVAR_CPP_CORE_FORCE_INLINE uint64_t FNV1AHash64(const T* data, size_t byteLength) {
    uint64_t hash = FNV1A_64_OFFSET_BASIS;
    const uint8_t* byteData = reinterpret_cast<const uint8_t*>(data);
    for (size_t i = 0; i < byteLength; ++i) {
        hash ^= byteData[i];
        hash *= FNV1A_64_PRIME;
    }
    return hash;
}

inline uint64_t CombineHashCodes(uint64_t hash1, uint64_t hash2) {
    return hash1 ^ (hash2 + KNUTH_CONSTANT_64 + (hash1 << 12) + (hash1 >> 4));
}
template <typename... ArgsT> inline uint64_t CombineHashCodes(uint64_t hash1, uint64_t hash2, ArgsT... args) {
    uint64_t combined = CombineHashCodes(hash1, hash2);
    if constexpr (sizeof...(args) > 0) {
        return CombineHashCodes(combined, args...);
    } else {
        return combined;
    }
}

template <typename T>
inline uint64_t GetHashCode(const T& value)
    requires(std::is_fundamental_v<T> || std::is_pointer_v<T> || std::is_enum_v<T>)
{
    return FNV1AHash64(&value, sizeof(T));
}

template <typename T>
concept HasHashCodeMethod = requires(const T& a) {
    { a.GetHashCode() } -> std::convertible_to<uint64_t>;
};

template <typename T>
concept HasGetHashCodeFreeFunction = requires(const T& a) {
    { GetHashCode(a) } -> std::convertible_to<uint64_t>;
};

template <typename T>
concept IsHashable = HasHashCodeMethod<T> || HasGetHashCodeFreeFunction<T>;

template <typename T>
inline uint64_t GetHashCode(const T& value)
    requires(HasHashCodeMethod<T> && !HasGetHashCodeFreeFunction<T>)
{
    static_assert(std::is_same_v<decltype(value.GetHashCode()), uint64_t>,
                  "GetHashCode: GetHashCode method must return uint64_t.");
    static_assert(std::is_const_v<decltype(value)>, "GetHashCode: GetHashCode method must be const-qualified.");
    constexpr uint64_t typeSize = sizeof(value);
    static_assert(typeSize > 0, "GetHashCode: Cannot get hash code of an incomplete type.");
    return value.GetHashCode();
}

template <typename T>
inline uint64_t GetHashCode(const T& value)
    requires(HasGetHashCodeFreeFunction<T>)
{
    return GetPrimitiveHashCode(value);
}

template <typename... ArgsT>
inline uint64_t GetHashCode(const ArgsT&... args)
    requires(sizeof...(args) > 1 && (IsHashable<ArgsT> && ...))
{
    return CombineHashCodes(GetHashCode(args)...);
}

} // namespace Edvar::Utils