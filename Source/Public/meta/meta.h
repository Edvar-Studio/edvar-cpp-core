#pragma once

namespace edvar::meta {
#pragma region enable_if
template <bool expression, typename return_type = void> struct enable_if_template;

template <typename return_type> struct enable_if_template<true, return_type> {
    using type = return_type;
};
template <typename return_type> struct enable_if_template<false, return_type> {};

template <bool expression, typename return_type = void>
using enable_if = typename enable_if_template<expression, return_type>::type;
#pragma endregion enable_if

#pragma region is_character
template <typename T> struct is_character {
    static constexpr bool value = false;
};
template <> struct is_character<char> {
    static constexpr bool value = true;
};

template <> struct is_character<char_utf8> {
    static constexpr bool value = true;
};
template <> struct is_character<char_utf16> {
    static constexpr bool value = true;
};
template <> struct is_character<char_utf32> {
    static constexpr bool value = true;
};
#pragma endregion is_character
} // namespace edvar::meta