#pragma once

namespace edvar {
inline string_utf8 to_string(int8 value) {
    char_utf8 buf[5]; // enough for -128 and null terminator
    int pos = 4;
    buf[pos] = '\0';
    int8 abs_value = edvar::math::abs(value);
    do {
        buf[--pos] = '0' + (abs_value % 10);
        abs_value /= 10;
    } while (abs_value && pos > 0);
    if (value < 0 && pos > 0)
        buf[--pos] = '-';
    return string_utf8(buf + pos);
}
inline string_utf16 to_string_utf16(int8 value) {}
} // namespace edvar