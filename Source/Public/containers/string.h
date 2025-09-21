#pragma once
#include "./array.h" // IWYU pragma: keep
#include "platform/basic_types.h"
#include "utils/value_or_error.h"

namespace edvar {
namespace internationalization {
class locale;
}
namespace c_string {
template <typename character_type, typename = edvar::meta::enable_if<edvar::meta::is_character<character_type>>>
inline uint32 string_length(const character_type* str) {
    uint32 length = 0;
    while (str[length] != 0) {
        length++;
    }
    return length;
}

} // namespace c_string

template <typename character_type = char_utf16> class string_view {
public:
    string_view() : _data(nullptr), _length(0) {}
    string_view(const character_type* str) : _data(str), _length(edvar::c_string::string_length(str)) {}
    string_view(const character_type* str, uint32 length) : _data(str), _length(length) {}
    const character_type* data() const { return _data; }
    character_type* mutable_data() { return const_cast<character_type*>(_data); }

    struct section {
        const string_view parent_view;
        const string_view result_view;

        const character_type* section_start;
        const character_type* section_end;
    };

    value_or_error_code<section> find(const character_type* in_str, uint32 start_offset = 0) const {
        uint32 other_length = edvar::c_string::string_length(in_str);
        if (other_length > (_length + start_offset)) {
            return value_or_error_code<section>::from_error(1);
        }
        for (uint32 i = 0; i < _length; ++i) {
            character_type* current_comp_start = _data + i + start_offset;
            bool matches = true;
            for (uint32 j = 0; j < other_length; ++j) {
                if (current_comp_start[j] != in_str[j]) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                return value_or_error_code<section>::from_value(
                    section{._parent_view = *this,
                            ._section_start = current_comp_start,
                            ._section_end = current_comp_start + other_length,
                            ._result_view = string_view(current_comp_start, other_length)});
            }
        }
        return value_or_error_code<section>::from_error(1);
    }

    array<value_or_error_code<section>> find_all(const character_type* in_str, uint32 start_offset = 0) const {
        array<value_or_error_code<section>> return_arr;
        value_or_error_code<section> latest;
        uint32 latest_offset = start_offset;
        do {
            latest = find(in_str, latest_offset);
            if (latest.is_error()) {
                break;
            }
            section latest_section = latest.get_value();
            latest_offset += latest_section.result_view._length;
        } while (latest.is_error());
        return return_arr;
    }

    bool operator==(const character_type* other_string) const {
        uint32 other_length = edvar::c_string::string_length(other_string);
        if (other_length != _length) {
            return false;
        }
        for (uint32 i = 0; i < _length; ++i) {
            if (_data[i] != other_string[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator==(const string_view& other) const {
        if (other._length != _length) {
            return false;
        }
        for (uint32 i = 0; i < _length; ++i) {
            if (_data[i] != other._data[i]) {
                return false;
            }
        }
        return true;
    }

private:
    const character_type* _data;
    uint32 _length;
};

template <typename character_type = char_utf16> class string_base {
public:
    string_base();
    ~string_base();

    string_base(const character_type* str);
    string_base(const character_type* str, uint32 length);
    string_base(const string_base& other);
    string_base(string_base&& other) noexcept;

    template <typename other_character_type>
    inline void convert_string_type(const string_base<other_character_type>& other);
    inline string_base& operator=(const string_base& other);
    inline string_base& operator=(string_base&& other) noexcept;
    inline bool operator==(const string_base& other) const;
    bool operator!=(const string_base& other) const { return !operator==(other); }

private:
    array<character_type> _data;
};

using string_utf32 = string_base<char_utf32>;
using string_utf16 = string_base<char_utf16>;
using string = string_base<char_utf8>;

} // namespace edvar