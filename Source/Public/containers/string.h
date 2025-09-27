#pragma once
#include "./array.h" // IWYU pragma: keep
#include "platform/basic_types.h"
#include "utils/value_or_error.h"

namespace edvar {
namespace c_string {
template <typename character_type, typename = edvar::meta::enable_if<edvar::meta::is_character<character_type>>>
inline uint32 string_length(const character_type* str) {
    uint32 length = 0;
    while (str[length] != 0) {
        length++;
    }
    return length;
}

char_utf8* create_utf8(const char_utf16* in_string);
char_utf8* create_utf8(const char_utf32* in_string);
char_utf16* create_utf16(const char_utf8* in_string);
char_utf16* create_utf16(const char_utf32* in_string);
char_utf32* create_utf32(const char_utf8* in_string);
char_utf32* create_utf32(const char_utf16* in_string);

char_utf8* create_lowercase(const char_utf8* in_string, edvar::internationalization::locale* locale = nullptr);
char_utf16* create_lowercase(const char_utf16* in_string, edvar::internationalization::locale* locale = nullptr);
char_utf32* create_lowercase(const char_utf32* in_string, edvar::internationalization::locale* locale = nullptr);

char_utf8* create_uppercase(const char_utf8* in_string, edvar::internationalization::locale* locale = nullptr);
char_utf16* create_uppercase(const char_utf16* in_string, edvar::internationalization::locale* locale = nullptr);
char_utf32* create_uppercase(const char_utf32* in_string, edvar::internationalization::locale* locale = nullptr);

int compare(const char_utf8* str1, const char_utf8* str2, bool case_sensitive = true,
            const edvar::internationalization::locale* locale = nullptr);
int compare(const char_utf16* str1, const char_utf16* str2, bool case_sensitive = true,
            const edvar::internationalization::locale* locale = nullptr);
int compare(const char_utf32* str1, const char_utf32* str2, bool case_sensitive = true,
            const edvar::internationalization::locale* locale = nullptr);

} // namespace c_string

template <typename character_type = char_utf16> class string_view {
public:
    string_view() : _data(nullptr), _length(0) {}
    string_view(const character_type* str) : _data(str), _length(edvar::c_string::string_length(str)) {}
    string_view(const character_type* str, uint32 length) : _data(str), _length(length) {}
    const character_type* data() const { return _data; }
    character_type* mutable_data() { return const_cast<character_type*>(_data); }

    struct section {
        const string_view* parent_view;
        const edvar::memory::unique_pointer<string_view> result_view;

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
                    section{._parent_view = this,
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
    string_base() {
        _data.add(0); // null terminator
    }
    ~string_base() { _data.empty(); }

    string_base(const character_type* str) {
        if (str) {
            uint32 length = edvar::c_string::string_length(str);
            _data.resize(length + 1);
            _data = str;
        }
        _data.add(0); // null terminator
    }
    string_base(const character_type* str, uint32 length) {
        _data.resize(length + 1);
        for (uint32 i = 0; i < length; ++i) {
            _data[i] = str[i];
        }
        _data[length] = 0; // null terminator
    }
    string_base(const string_base& other) { _data = other._data; }
    string_base(string_base&& other) noexcept {
        _data = edvar::move(other._data);
        other._data.empty();
    }

    inline string_base<char_utf16> to_utf16() const {
        char_utf16* as_utf16 = c_string::create_utf16(_data.data());
        string_base<char_utf16> result(as_utf16);
        delete[] as_utf16;
        return result;
    }

    inline string_base<char_utf32> to_utf32() const {
        char_utf32* as_utf32 = c_string::create_utf32(_data.data());
        string_base<char_utf32> result(as_utf32);
        delete[] as_utf32;
        return result;
    }

    inline string_base<char_utf8> to_utf8() const {
        char_utf8* as_utf8 = c_string::create_utf8(_data.data());
        string_base<char_utf8> result(as_utf8);
        delete[] as_utf8;
        return result;
    }

    inline string_base to_lowercase(edvar::internationalization::locale* locale = nullptr) const {
        char_utf16* lowercased = c_string::create_lowercase(_data.data(), locale);
        string_base result(lowercased);
        delete[] lowercased;
        return result;
    }

    inline string_base to_uppercase(edvar::internationalization::locale* locale = nullptr) const {
        char_utf16* uppercased = c_string::create_uppercase(_data.data(), locale);
        string_base result(uppercased);
        delete[] uppercased;
        return result;
    }

    inline bool operator==(const string_base& other) const { return is_equal(other, true); }
    inline bool operator!=(const string_base& other) const { return !operator==(other); }

    inline bool is_equal(const character_type* str, bool case_sensitive = true,
                         const edvar::internationalization::locale* locale = nullptr) const {
        if (!str && _data.size() == 0)
            return true;
        if (!str || _data.size() == 0)
            return false;
        return c_string::compare(_data.data(), str, case_sensitive, locale) == 0;
    }

    inline bool is_equal(const string_base& other, bool case_sensitive = true,
                         const edvar::internationalization::locale* locale = nullptr) const {
        if (other._data.size() != _data.size())
            return false;
        return c_string::compare(_data.data(), other._data.data(), case_sensitive, locale) == 0;
    }

    inline string_base& operator=(const character_type* str) {
        if (_data.data() != str) {
            _data.clear();
            if (str) {
                uint32 length = edvar::c_string::string_length(str);
                _data.resize(length + 1);
                _data = str;
                // add null terminator
                _data.add(0);
            }
        }
        return *this;
    }

    inline string_base& operator=(const string_base& other) noexcept {
        if (this != &other) {
            _data = other._data;
        }
        return *this;
    }

    inline string_base& operator=(string_base&& other) noexcept {
        if (this != &other) {
            _data = edvar::move(other._data);
            other._data.empty();
        }
        return *this;
    }

    inline const character_type* data() const { return _data.data(); }
    inline character_type* data() { return _data.data(); }
    inline uint32 length() const { return _data.length() > 0 ? _data.length() - 1 : 0; }
    inline bool is_empty() const { return length() == 0; }
    inline void clear() {
        _data.clear();
        _data.add(0); // null terminator
    }

    inline void append(const character_type* str, uint32 length) {
        if (str && length > 0) {
            if (_data.length() > 0) {
                _data.remove_at(_data.length() - 1); // remove null terminator
            }
            uint32 current_length = _data.length();
            _data.resize(current_length + length + 1);
            for (uint32 i = 0; i < length; ++i) {
                _data[current_length + i] = str[i];
            }
            _data[current_length + length] = 0; // null terminator
        }
    }
    inline void append(const character_type* str) {
        if (str) {
            append(str, edvar::c_string::string_length(str));
        }
    }
    inline void append(const string_base& other) { append(other.data(), other.length()); }
    inline void append(const character_type ch) {
        if (_data.length() > 0) {
            _data.remove_at(_data.length() - 1); // remove null terminator
        }
        uint32 current_length = _data.length();
        _data.resize(current_length + 2);
        _data[current_length] = ch;
        _data[current_length + 1] = 0; // null terminator
    }
    inline void append(const string_view<character_type>& view) { append(view.data(), view.length()); }
    inline void append(const string_view<character_type>::section& section) { append(section.result_view); }

    string_base& operator+=(const character_type* str) {
        append(str);
        return *this;
    }
    string_base& operator+=(const string_base& other) {
        append(other);
        return *this;
    }
    string_base& operator+=(const character_type ch) {
        append(ch);
        return *this;
    }
    string_base& operator+=(const string_view<character_type>& view) {
        append(view);
        return *this;
    }
    string_base& operator+=(const string_view<character_type>::section& section) {
        append(section);
        return *this;
    }
    string_base& operator+(const character_type* str) const {
        string_base result(*this);
        result.append(str);
        return result;
    }
    string_base& operator+(const string_base& other) const {
        string_base result(*this);
        result.append(other);
        return result;
    }
    string_base& operator+(const character_type ch) const {
        string_base result(*this);
        result.append(ch);
        return result;
    }
    string_base& operator+(const string_view<character_type>& view) const {
        string_base result(*this);
        result.append(view);
        return result;
    }
    string_base& operator+(const string_view<character_type>::section& section) const {
        string_base result(*this);
        result.append(section);
        return result;
    }

private:
    array<character_type> _data;
};

using string_utf32 = string_base<char_utf32>;
using string_utf16 = string_base<char_utf16>;
using string_utf8 = string_base<char_utf8>;
using string = string_base<char_utf8>;

} // namespace edvar