#pragma once

namespace edvar {
struct convertible_to_string {
public:
    virtual ~convertible_to_string() = default;
    /**
     * function to convert to string.
     */
    virtual edvar::string_base<char_utf16> to_string() const = 0;
    /**
     * function to convert to string with options. Default implementation ignores options.
     */
    virtual edvar::string_base<char_utf16>
    to_string(const edvar::string_base<char_utf16>& options) const;
};


// No option is available but the function tries to give us an options parameter.
template <typename in_type>
edvar::string_base<char_utf16> to_string(const in_type& value,
                                                        const edvar::string_base<char_utf16>& options)
    requires(!has_optioned_to_string_method<in_type> && has_pure_to_string_method<in_type>)
{
    return value.to_string();
}

template <typename in_type>
edvar::string_base<char_utf16> to_string(const in_type& value,
                                                        const edvar::string_base<char_utf16>& options)
    requires has_optioned_to_string_method<in_type>
{
    return value.to_string(options);
}


template <typename in_type>
edvar::string_base<char_utf16> to_string(const in_type& value,
                                                        const edvar::string_base<char_utf16>& options)
    requires(!has_to_string_method<in_type>) && has_native_string_conversion<in_type>
{
    if constexpr (std::is_same_v<in_type, convertible_to_string>) {
        return value.to_string(options);
    }
    return to_string(value);
}
}