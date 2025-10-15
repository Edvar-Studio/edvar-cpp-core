#pragma once
#include "./array.h" // IWYU pragma: keep
#include "platform/basic_types.h"

namespace edvar {
namespace c_string {
template <typename character_type, typename = std::enable_if_t<edvar::meta::is_character_v<character_type>>>
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

bool is_whitespace(char_utf8 ch);
bool is_whitespace(char_utf16 ch);
bool is_whitespace(char_utf32 ch);

template <typename character_type = char_utf16, bool is_const = false> class iterator {
public:
    using data_type = std::conditional_t<is_const, const character_type*, character_type*>;
    using data_ref_type = std::conditional_t<is_const, const character_type&, character_type&>;

    iterator(character_type* start, uint32 length) : _data(start), _length(length), _current_index(0) {}
    iterator(character_type* start) : _data(start), _length(edvar::c_string::string_length(start)), _current_index(0) {}
    iterator(const iterator& other)
        : _data(other._data), _length(other._length), _current_index(other._current_index) {}
    iterator(iterator&& other) noexcept
        : _data(other._data), _length(other._length), _current_index(other._current_index) {
        other._data = nullptr;
        other._length = 0;
        other._current_index = 0;
    }
    ~iterator() = default;

    bool has_next() const { return _current_index < _length; }
    data_ref_type next() { return _data[_current_index++]; }
    data_ref_type current() const { return _data[_current_index]; }
    bool has_previous() const { return _current_index > 0; }
    data_ref_type previous() { return _data[--_current_index]; }
    iterator& reset() {
        _current_index = 0;
        return *this;
    }
    iterator& seek(uint32 index) {
        if (index < _length) {
            _current_index = index;
        } else {
            _current_index = _length;
        }
        return *this;
    }
    data_ref_type peek_next() {
        if (has_next()) {
            return _data[_current_index + 1];
        }
    }
    data_ref_type peek_previous() {
        if (has_previous()) {
            return _data[_current_index - 1];
        }
    }

    std::enable_if_t<!is_const, iterator&> replace(const character_type& value) {
        _data[_current_index] = value;
        return *this;
    }

    // utility
    bool is_whitespace() const { return c_string::is_whitespace(_data[_current_index]); }
    void skip_whitespace() {
        while (has_next() && is_whitespace()) {
            ++_current_index;
        }
    }
    void skip_non_whitespace() {
        while (has_next() && !is_whitespace()) {
            ++_current_index;
        }
    }

    iterator& operator++() {
        if (has_next())
            ++_current_index;
        return *this;
    }
    iterator operator++(int) {
        iterator temp = *this;
        ++(*this);
        return temp;
    }
    iterator& operator--() {
        if (has_previous())
            --_current_index;
        return *this;
    }
    iterator operator--(int) {
        iterator temp = *this;
        --(*this);
        return temp;
    }
    data_ref_type operator*() { return current(); }
    data_type operator->() { return &current(); }
    bool operator==(const iterator& other) const {
        return _data == other._data && _length == other._length && _current_index == other._current_index;
    }
    bool operator!=(const iterator& other) const { return !(*this == other); }

private:
    data_type _data;
    uint32 _length;
    uint32 _current_index;
};
// swap for iterator
} // namespace c_string

template <typename character_type> class string_base; // forward declare string_base

template <typename character_type = char_utf16> class string_view {
public:
    string_view() : _data(nullptr), _length(0) {}
    string_view(const character_type* str) : _data(str), _length(edvar::c_string::string_length(str)) {}
    string_view(const character_type* str, uint32 length) : _data(str), _length(length) {}
    inline string_view(const string_base<character_type>& in_string_base);
    const character_type* data() const { return _data; }
    character_type* mutable_data() { return const_cast<character_type*>(_data); }
    uint32 length() const { return _length; }

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

    container::array<value_or_error_code<section>> find_all(const character_type* in_str,
                                                            uint32 start_offset = 0) const {
        container::array<value_or_error_code<section>> return_arr;
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

    bool operator!=(const character_type* other_string) const { return !(*this == other_string); }
    bool operator!=(const string_view& other) const { return !(*this == other); }

    const character_type& operator[](uint32 index) const { return _data[index]; }

    using iterator = c_string::iterator<character_type, false>;
    using const_iterator = c_string::iterator<character_type, true>;

    iterator begin() { return iterator(const_cast<char_utf16*>(_data), _length); }
    iterator end() { return iterator(const_cast<char_utf16*>(_data), _length).seek(_length); }
    const_iterator begin() const { return const_iterator(_data, _length); }
    const_iterator end() const { return const_iterator(_data, _length).seek(_length); }
    const_iterator cbegin() const { return const_iterator(_data, _length); }
    const_iterator cend() const { return const_iterator(_data, _length).seek(_length); }

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

    string_base(nullptr_t) {
        _data.add(0); // null terminator
    }
    string_base(const char* c_str) {
        if (c_str) {
            char_utf16* as_utf16 = c_string::create_utf16(reinterpret_cast<const char_utf8*>(c_str));
            if (as_utf16) {
                uint32 length = edvar::c_string::string_length(as_utf16);
                _data.ensure_capacity(length + 1);
                _data.add_uninitialized(length + 1);
                edvar::memory::copy<character_type>(_data.data(), as_utf16, length);
                _data.data()[length] = 0; // null terminator
                delete[] as_utf16;
            } else {
                _data.add(0); // null terminator
            }
        } else {
            _data.add(0); // null terminator
        }
    }

    string_base(const char_utf8* utf8_str) {
        if (utf8_str) {
            char_utf16* as_utf16 = c_string::create_utf16(utf8_str);
            if (as_utf16) {
                uint32 length = edvar::c_string::string_length(as_utf16);
                _data.ensure_capacity(length + 1);
                _data.add_uninitialized(length + 1);
                edvar::memory::copy<character_type>(_data.data(), as_utf16, length);
                _data.data()[length] = 0; // null terminator
                delete[] as_utf16;
            } else {
                _data.add(0); // null terminator
            }
        } else {
            _data.add(0); // null terminator
        }
    }

    string_base(const char_utf16* utf16_str) {
        if (utf16_str) {
            uint32 length = edvar::c_string::string_length(utf16_str);
            _data.ensure_capacity(length + 1);
            _data.add_uninitialized(length + 1);
            edvar::memory::copy<character_type>(_data.data(), utf16_str, length);
            _data.data()[length] = 0; // null terminator
        } else {
            _data.add(0); // null terminator
        }
    }

    string_base(const char_utf32* utf32_str) {
        if (utf32_str) {
            char_utf16* as_utf16 = c_string::create_utf16(utf32_str);
            if (as_utf16) {
                uint32 length = edvar::c_string::string_length(as_utf16);
                _data.ensure_capacity(length + 1);
                _data.add_uninitialized(length + 1);
                edvar::memory::copy<character_type>(_data.data(), as_utf16, length);
                _data.data()[length] = 0; // null terminator
                delete[] as_utf16;
            } else {
                _data.add(0); // null terminator
            }
        } else {
            _data.add(0); // null terminator
        }
    }

    string_base(const wchar_t* in_str) {
        static_assert(sizeof(wchar_t) == sizeof(char_utf16) || sizeof(wchar_t) == sizeof(char_utf32) ||
                          sizeof(wchar_t) == sizeof(char_utf8),
                      "wchar_t must be either 16-bit or 32-bit or 8-bit");
        // convert wchar_t* to char_utf16*
        // for this if sizeof(wchar_t) == sizeof(char_utf16) we can directly use it
        if constexpr (sizeof(wchar_t) == sizeof(char_utf16)) {
            _data.add(reinterpret_cast<const char_utf16*>(in_str), edvar::c_string::string_length(in_str));
        } else if constexpr (sizeof(wchar_t) == sizeof(char_utf32)) {
            char_utf16* as_utf16 = c_string::create_utf16(reinterpret_cast<const char_utf32*>(in_str));
            if (as_utf16) {
                uint32 length = edvar::c_string::string_length(as_utf16);
                _data.ensure_capacity(length + 1);
                _data.add_uninitialized(length + 1);
                edvar::memory::copy<character_type>(_data.data(), as_utf16, length);
                _data.data()[length] = 0; // null terminator
                delete[] as_utf16;
            } else {
                _data.add(0); // null terminator
            }
        } else if constexpr (sizeof(wchar_t) == sizeof(char_utf8)) {
            // this doesn't make any sense, why would someone use this but ok.
            char_utf16* as_utf16 = c_string::create_utf16(reinterpret_cast<const char_utf8*>(in_str));
            if (as_utf16) {
                uint32 length = edvar::c_string::string_length(as_utf16);
                _data.ensure_capacity(length + 1);
                _data.add_uninitialized(length + 1);
                edvar::memory::copy<character_type>(_data.data(), as_utf16, length);
                _data.data()[length] = 0; // null terminator
                delete[] as_utf16;
            } else {
                _data.add(0); // null terminator
            }
        }
    }

    string_base(const character_type* str, uint32 length) {
        if (str == nullptr) {
            _data.add(0); // null terminator
            return;
        }

        _data.ensure_capacity(length + 1);
        _data.add_uninitialized(length + 1);
        edvar::memory::copy<character_type>(_data.data(), str, length);
        _data.data()[length] = 0; // null terminator
    }
    string_base(const string_base& other) {
        _data = other._data; // copies by default
    }
    string_base(string_base&& other) noexcept { _data = edvar::move(other._data); }

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
    inline void append(const string_base& other) {
        if (other.length() > 0)
            append(other.data(), other.length());
    }
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
    inline void append(const typename string_view<character_type>::section& section) { append(section.result_view); }

    template <typename... Args> static string_base format(const char* format, const Args&... args) {
        return format(reinterpret_cast<const character_type*>(format), args...);
    }
    template<typename... Args> static string_base format(const wchar_t* format, const Args&... args) {
        return format(reinterpret_cast<const character_type*>(format), args...);
    }
    template <typename... Args> static string_base format(const character_type* format, const Args&... args) {
        string_view<character_type> format_view(format);
        string_base result;

        // arguments to string
        array<string_base> arg_strings;
        constexpr int num_args = sizeof...(Args);
        arg_strings.add_uninitialized(num_args);

        int arg_index = 0;
        ((string_base::to_string_helper(args, arg_strings[arg_index++])) , ...);
        // parse format string
        for (auto iterator = format_view.begin(); iterator.has_next(); ++iterator) {
            character_type ch = *iterator;
            // skip the whitespace before processing
            if (iterator.has_next()) {
                iterator.skip_whitespace();
            } else {
                result.append(ch);
                break;
            }
            string_base braces_inner;
            switch (ch) {
            case '{': {
                // if next is also {, then it is escaped
                break;
            }
            case '}': {
                break;
            }
            default: {

                break;
            }
            }
        }
        return result;
    }

    void resize(uint32 new_length) {
        if (new_length == 0) {
            clear();
            return;
        }
        if (_data.length() > 0) {
            _data.remove_at(_data.length() - 1); // remove null terminator
        }
        _data.resize(new_length + 1);
        _data[new_length] = 0; // null terminator
    }

    character_type& operator[](uint32 index) { return _data[index]; }
    const character_type& operator[](uint32 index) const { return _data[index]; }

    string_view<character_type> to_view() const { return string_view<character_type>(_data.data(), length()); }
    string_view<character_type>::iterator begin() { return to_view().begin(); }
    string_view<character_type>::iterator end() { return to_view().end(); }
    string_view<character_type>::const_iterator begin() const { return to_view().begin(); }
    string_view<character_type>::const_iterator end() const { return to_view().end(); }
    string_view<character_type>::const_iterator cbegin() const { return to_view().cbegin(); }
    string_view<character_type>::const_iterator cend() const { return to_view().cend(); }

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
    string_base& operator+=(const typename string_view<character_type>::section& section) {
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
    string_base& operator+(const typename string_view<character_type>::section& section) const {
        string_base result(*this);
        result.append(section);
        return result;
    }

private:
    array<character_type> _data;
};

template <typename character_type>
string_view<character_type>::string_view(const string_base<character_type>& in_string_base) {
    _data = in_string_base.data();
    _length = in_string_base.length();
}

} // namespace edvar

#pragma region iterator_requirements
// Requirements for iterators
template <typename character_type>
inline void swap(edvar::c_string::iterator<character_type>& a, edvar::c_string::iterator<character_type>& b) noexcept {
    using edvar::swap;
    swap(a._data, b._data);
    swap(a._length, b._length);
    swap(a._current_index, b._current_index);
}

#pragma endregion iterator_requirements
