#pragma once

namespace edvar::memory {
/**
 * copy bytes of src to dest
 * @param dest destination pointer
 * @param src source pointer
 * @param count number of bytes to copy
 */
inline void copy_bytes(void* dest, const void* src, uint32_t count) {
    uint8_t* d = reinterpret_cast<uint8_t*>(dest);
    const uint8_t* s = reinterpret_cast<const uint8_t*>(src);
    for (uint32_t i = 0; i < count; ++i) {
        d[i] = s[i];
    }
}
/**
 * copy bytes of src to dest
 * @param dest destination pointer
 * @param src source pointer
 * @param count number of elements to copy (of type in_type)
 */
template <typename in_type = uint8_t> void copy(in_type* dest, const in_type* src, uint32_t count = 1) {
    copy_bytes(reinterpret_cast<void*>(dest), reinterpret_cast<const void*>(src), count * sizeof(in_type));
}
/**
 * set bytes of dest to value
 * @param dest destination pointer
 * @param value value to set (byte)
 * @param count number of bytes to set
 */
inline void set_bytes(void* dest, uint8_t value, uint32_t count) {
    uint8_t* d = reinterpret_cast<uint8_t*>(dest);
    for (uint32_t i = 0; i < count; ++i) {
        d[i] = value;
    }
}
/**
 * set bytes of dest to value
 * @param dest destination pointer
 * @param value value to set (of type in_type)
 * @param count number of elements to set (of type in_type)
 */
template <typename in_type = uint8_t> void set(in_type* dest, uint8_t value, uint32_t count = 1) {
    set_bytes(reinterpret_cast<void*>(dest), value, count * sizeof(in_type));
}
/**
 * set bytes of dest to zero
 * @param dest destination pointer
 * @param count number of bytes to set
 */
inline void zero_bytes(void* dest, uint32_t count) { set_bytes(dest, 0, count); }
/**
 * set bytes of dest to zero
 * @param dest destination pointer
 * @param count number of elements to set (of type in_type)
 */
template <typename in_type = uint8_t> void zero(in_type* dest, uint32_t count = 1) {
    zero_bytes(reinterpret_cast<void*>(dest), count * sizeof(in_type));
}

} // namespace edvar::memory