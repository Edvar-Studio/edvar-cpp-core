#pragma once

namespace edvar {
// integer values
edvar::string_base<char_utf16> to_string(int8_t value);
edvar::string_base<char_utf16> to_string(uint8_t value);
edvar::string_base<char_utf16> to_string(int16_t value);
edvar::string_base<char_utf16> to_string(uint16_t value);
edvar::string_base<char_utf16> to_string(int32_t value);
edvar::string_base<char_utf16> to_string(uint32_t value);
edvar::string_base<char_utf16> to_string(int64_t value);
edvar::string_base<char_utf16> to_string(uint64_t value);

// boolean value
edvar::string_base<char_utf16> to_string(bool value);

// floating point values
edvar::string_base<char_utf16> to_string(float value);
edvar::string_base<char_utf16> to_string(double value);
// c-string values
edvar::string_base<char_utf16> to_string(const char_utf16* value);
edvar::string_base<char_utf16> to_string(const char_utf8* value);
edvar::string_base<char_utf16> to_string(const char_utf32* value);
// string values
edvar::string_base<char_utf16> to_string(const edvar::string_base<char_utf16>& value);
edvar::string_base<char_utf16> to_string(const string_utf8& value);
edvar::string_base<char_utf16> to_string(const string_utf32& value);

// convertible to string values. calls without options.
edvar::string_base<char_utf16> to_string(const convertible_to_string& value);

template <typename in_type>
concept has_pure_to_string_method = requires(const in_type& value) {
    { value.to_string() } -> std::same_as<edvar::string_base<char_utf16>>;
};

template <typename in_type>
concept has_optioned_to_string_method = requires(const in_type& value) {
    { value.to_string(std::declval<edvar::string_base<char_utf16>>()) } -> std::same_as<edvar::string_base<char_utf16>>;
};

template <typename in_type>
concept has_native_string_conversion =
    std::same_as<in_type, int8_t> || std::same_as<in_type, uint8_t> || std::same_as<in_type, int32_t> ||
    std::same_as<in_type, uint32_t> || std::same_as<in_type, float> || std::same_as<in_type, double> ||
    std::same_as<in_type, const char_utf16*> || std::same_as<in_type, const char_utf8*> ||
    std::same_as<in_type, const char_utf32*> || std::same_as<in_type, edvar::string_base<char_utf16>> ||
    std::same_as<in_type, edvar::string_utf8> || std::same_as<in_type, edvar::string_utf32> ||
    std::same_as<in_type, bool> || std::same_as<in_type, convertible_to_string>;

template <typename in_type>
concept has_to_string_method = has_pure_to_string_method<in_type> || has_optioned_to_string_method<in_type>;

// No option is available but the function tries to give us an options parameter.
template <typename in_type>
edvar::string_base<char_utf16> to_string(const in_type& value, const edvar::string_base<char_utf16>& options)
    requires(!has_optioned_to_string_method<in_type> && has_pure_to_string_method<in_type>);

template <typename in_type>
edvar::string_base<char_utf16> to_string(const in_type& value, const edvar::string_base<char_utf16>& options)
    requires has_optioned_to_string_method<in_type>;

template <typename in_type>
edvar::string_base<char_utf16> to_string(const in_type& value, const edvar::string_base<char_utf16>& options)
    requires(!has_to_string_method<in_type>) && has_native_string_conversion<in_type>;
} // namespace edvar