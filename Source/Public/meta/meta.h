#pragma once

namespace edvar::meta{
template<typename type> struct is_character : std::false_type{};
template<> struct is_character<char_utf8> : std::true_type{};
template<> struct is_character<char_utf16> : std::true_type{};
template<> struct is_character<char_utf32> : std::true_type{};

template<typename type> constexpr bool is_character_v = is_character<type>::value;
}