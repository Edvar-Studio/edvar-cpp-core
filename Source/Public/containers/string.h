#pragma once
#include "./array.h" // IWYU pragma: keep
#include "utils/value_or_error.h"

namespace edvar {
namespace c_string {
uint32 string_length(const wchar_t* buffer);
uint32 string_length(const char* buffer);
} // namespace c_string

template <typename character_type = wchar_t> class string_view {
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
    enum class is_equal_comparson_type : uint8 {
        current_culture,
        current_culture_ignore_case,

        invariant_culture,
        invariant_culture_ignore_case,

        ordinal,
        ordinal_ignore_case
    };
    bool is_equal(const character_type* other_string,
                  is_equal_comparson_type comparision_rule = is_equal_comparson_type::current_culture) {
                    
                  }

    bool operator==(const character_type* other_string) const {}

private:
    const character_type* _data;
    uint32 _length;
};

class string {};
} // namespace edvar