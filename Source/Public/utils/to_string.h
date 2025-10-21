#pragma once

#include <utility>
namespace edvar {
struct convertible_to_string {
public:
    virtual ~convertible_to_string() = default;
    /**
     * function to convert to string.
     */
    virtual edvar::string_utf16 to_string() const = 0;
    /**
     * function to convert to string with options. Default implementation ignores options.
     */
    virtual edvar::string_utf16 to_string(const edvar::string_utf16& options) const { return to_string(); }
};
// integer values
string_utf16 to_string(int8 value);
string_utf16 to_string(uint8 value);
string_utf16 to_string(int16 value);
string_utf16 to_string(uint16 value);
string_utf16 to_string(int32 value);
string_utf16 to_string(uint32 value);
string_utf16 to_string(int64 value);
string_utf16 to_string(uint64 value);

// boolean value
string_utf16 to_string(bool value);

// floating point values
string_utf16 to_string(float value);
string_utf16 to_string(double value);
// c-string values
string_utf16 to_string(const char_utf16* value);
string_utf16 to_string(const char_utf8* value);
string_utf16 to_string(const char_utf32* value);
// string values
string_utf16 to_string(const string_utf16& value);
string_utf16 to_string(const string_utf8& value);
string_utf16 to_string(const string_utf32& value);

// convertible to string values. calls without options.
string_utf16 to_string(const convertible_to_string& value);

template <typename in_type>
concept has_pure_to_string_method = requires(const in_type& value) {
    { value.to_string() } -> std::same_as<edvar::string_utf16>;
};

template <typename in_type>
concept has_optioned_to_string_method = requires(const in_type& value) {
    { value.to_string(std::declval<string_utf16>()) } -> std::same_as<edvar::string_utf16>;
};

template <typename in_type>
concept has_native_string_conversion =
    std::same_as<in_type, int8> || std::same_as<in_type, uint8> || std::same_as<in_type, int32> ||
    std::same_as<in_type, uint32> || std::same_as<in_type, float> || std::same_as<in_type, double> ||
    std::same_as<in_type, const char_utf16*> || std::same_as<in_type, const char_utf8*> ||
    std::same_as<in_type, const char_utf32*> || std::same_as<in_type, edvar::string_utf16> ||
    std::same_as<in_type, edvar::string_utf8> || std::same_as<in_type, edvar::string_utf32> ||
    std::same_as<in_type, bool> || std::same_as<in_type, convertible_to_string>;

template <typename in_type>
concept has_to_string_method = has_pure_to_string_method<in_type> || has_optioned_to_string_method<in_type>;

// No option is available but the function tries
template <typename in_type>
string_utf16 to_string(const in_type& value, const string_utf16& options)
    requires(!has_optioned_to_string_method<in_type>) && has_pure_to_string_method<in_type>
{
    return value.to_string();
}

template <typename in_type>
string_utf16 to_string(const in_type& value, const string_utf16& options)
    requires has_optioned_to_string_method<in_type>
{
    return value.to_string(options);
}

template <typename in_type>
string_utf16 to_string(const in_type& value, const string_utf16& options)
    requires(!has_to_string_method<in_type>) && has_native_string_conversion<in_type>
{
    if constexpr (std::is_same_v<in_type, convertible_to_string>) {
        return value.to_string(options);
    }
    return to_string(value);
}
} // namespace edvar