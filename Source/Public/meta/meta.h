#pragma once

namespace edvar::meta {
#pragma region enable_if
namespace structs {
template <typename in_type> in_type declval() noexcept {}

template <bool expression, typename return_type = void> struct enable_if;

template <typename return_type> struct enable_if<true, return_type> {
    using type = return_type;
};
template <typename return_type> struct enable_if<false, return_type> {};
} // namespace structs

template <bool expression, typename return_type = void>
using enable_if = typename edvar::meta::structs::enable_if<expression, return_type>::type;
#pragma endregion enable_if
#pragma region is_builtin_type
namespace structs {
// Trait to detect built-in types (arithmetic, void, nullptr_t)
template <typename T> struct is_builtin_type {
    static constexpr bool value = false;
};
template <> struct is_builtin_type<bool> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<char> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<wchar_t> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<signed char> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<unsigned char> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<short> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<unsigned short> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<int> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<unsigned int> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<long> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<unsigned long> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<long long> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<unsigned long long> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<float> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<double> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<long double> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<void> {
    static constexpr bool value = true;
};
template <> struct is_builtin_type<std::nullptr_t> {
    static constexpr bool value = true;
};
} // namespace structs

template <typename in_type>
inline constexpr bool is_builtin_type = edvar::meta::structs::is_builtin_type<in_type>::value;
#pragma region is_move_assignable
namespace structs {
template <typename in_type> struct is_move_assignable {
private:
    // detect expression: std::declval<T&>() = std::declval<T&&>()
    template <typename U, typename = decltype(declval<U&>() = declval<U&&>())> static char test(int);
    template <typename> static int test(...);

public:
    static constexpr bool value = sizeof(test<in_type>(0)) == sizeof(char);
};
template <typename T> struct is_nothrow_move_assignable {
    static constexpr bool value = noexcept(declval<T&>() = declval<T&&>());
};
} // namespace structs
template <typename in_type>
inline constexpr bool is_move_assignable = edvar::meta::structs::is_move_assignable<in_type>::value;
template <typename in_type>
inline constexpr bool is_nothrow_move_assignable = edvar::meta::structs::is_nothrow_move_assignable<in_type>::value;
#pragma endregion is_move_assignable
#pragma region is_move_constructible
namespace structs {
template <typename in_type> struct is_move_constructible {
private:
    // detect expression: T(std::declval<T&&>())
    template <typename in_type_proxy, typename = decltype(in_type_proxy(declval<in_type_proxy&&>()))>
    static char test(int);
    template <typename> static int test(...);

public:
    static constexpr bool value = sizeof(test<in_type>(0)) == sizeof(char);
};
template <typename in_type> struct is_nothrow_move_constructible {
    static constexpr bool value = noexcept(in_type(declval<in_type&&>()));
};
} // namespace structs
template <typename in_type>
static constexpr bool is_move_constructible = edvar::meta::structs::is_move_constructible<in_type>::value;
template <typename in_type>
static constexpr bool is_nothrow_move_constructible =
    edvar::meta::structs::is_nothrow_move_constructible<in_type>::value;
#pragma endregion is_move_constructible
#pragma region is_copy_constructible
namespace structs {
template <typename in_type> struct is_copy_constructible {
private:
    // detect expression: T(std::declval<const T&>())
    template <typename in_type_proxy, typename = decltype(in_type_proxy(declval<const in_type_proxy&>()))>
    static char test(int);
    template <typename> static int test(...);

public:
    static constexpr bool value = sizeof(test<in_type>(0)) == sizeof(char);
};

template <typename in_type> struct is_nothrow_copy_constructible {
    static constexpr bool value = noexcept(in_type(declval<const in_type&>()));
};
} // namespace structs
template <typename in_type>
inline constexpr bool is_copy_constructible = edvar::meta::structs::is_copy_constructible<in_type>::value;
template <typename in_type>
inline constexpr bool is_nothrow_copy_constructible =
    edvar::meta::structs::is_nothrow_copy_constructible<in_type>::value;
#pragma endregion is_copy_constructible
#pragma region is_destructible
namespace structs {
template <typename in_type> struct is_destructible {
private:
    template <typename U, typename = decltype(declval<U&>().~U())> static char test(int);
    template <typename> static int test(...);

public:
    static constexpr bool value = is_builtin_type<in_type>::value ? true : (sizeof(test<in_type>(0)) == sizeof(char));
};
template <typename in_type> struct is_nothrow_destructible {
    static constexpr bool value = noexcept(declval<in_type&>().~in_type());
};
} // namespace structs
template <typename in_type>
inline constexpr bool is_destructible = edvar::meta::structs::is_destructible<in_type>::value;
template <typename in_type>
inline constexpr bool is_nothrow_destructible = edvar::meta::structs::is_nothrow_destructible<in_type>::value;
#pragma endregion is_destructible
#pragma region is_character
namespace structs {
template <typename in_type> struct is_character_template {
    static constexpr bool value = false;
};
template <> struct is_character_template<char> {
    static constexpr bool value = true;
};
template <> struct is_character_template<wchar_t> {
    static constexpr bool value = true;
};
template <> struct is_character_template<char_utf8> {
    static constexpr bool value = true;
};
template <> struct is_character_template<char_utf16> {
    static constexpr bool value = true;
};
template <> struct is_character_template<char_utf32> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type>
inline constexpr bool is_character = edvar::meta::structs::is_character_template<in_type>::value;
#pragma endregion is_character
#pragma region has_hash_function
namespace structs {
template <typename in_type> struct has_hash_function {
    // look if there is a function with signature "uint64 get_hash() const" in T
private:
    template <typename type_proxy, uint64 (type_proxy::*)() const> struct SFINAE {};
    // if U has such a function, this overload is chosen
    template <typename type_proxy> static char test(SFINAE<type_proxy, &type_proxy::get_hash>*);
    // otherwise this overload is chosen
    template <typename type_proxy> static int test(...);

public:
    static constexpr bool value = sizeof(test<in_type>(0)) == sizeof(char);
};
} // namespace structs
template <typename in_type>
inline constexpr bool has_hash_function = edvar::meta::structs::has_hash_function<in_type>::value;
#pragma endregion has_hash_function
#pragma region is_integral
namespace structs {
template <typename in_type> struct is_integral {
    static constexpr bool value = false;
};
template <> struct is_integral<int8> {
    static constexpr bool value = true;
};
template <> struct is_integral<uint8> {
    static constexpr bool value = true;
};
template <> struct is_integral<int16> {
    static constexpr bool value = true;
};
template <> struct is_integral<uint16> {
    static constexpr bool value = true;
};
template <> struct is_integral<int32> {
    static constexpr bool value = true;
};
template <> struct is_integral<uint32> {
    static constexpr bool value = true;
};
template <> struct is_integral<int64> {
    static constexpr bool value = true;
};
template <> struct is_integral<uint64> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type> inline constexpr bool is_integral = edvar::meta::structs::is_integral<in_type>::value;
#pragma endregion is_integral

#pragma region is_unsigned
namespace structs {
template <typename in_type> struct is_unsigned {
    static constexpr bool value = false;
};
template <> struct is_unsigned<uint8> {
    static constexpr bool value = true;
};
template <> struct is_unsigned<uint16> {
    static constexpr bool value = true;
};
template <> struct is_unsigned<uint32> {
    static constexpr bool value = true;
};
template <> struct is_unsigned<uint64> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type> inline constexpr bool is_unsigned = edvar::meta::structs::is_unsigned<in_type>::value;
#pragma endregion is_unsigned
#pragma region is_signed
namespace structs {
template <typename in_type> struct is_signed {
    static constexpr bool value = false;
};
template <> struct is_signed<int8> {
    static constexpr bool value = true;
};
template <> struct is_signed<int16> {
    static constexpr bool value = true;
};
template <> struct is_signed<int32> {
    static constexpr bool value = true;
};
template <> struct is_signed<int64> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type> inline constexpr bool is_signed = edvar::meta::structs::is_signed<in_type>::value;
#pragma endregion is_signed

#pragma region is_floating_point
namespace structs {
template <typename in_type> struct is_floating_point {
    static constexpr bool value = false;
};
template <> struct is_floating_point<float> {
    static constexpr bool value = true;
};
template <> struct is_floating_point<double> {
    static constexpr bool value = true;
};
template <> struct is_floating_point<long double> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type>
inline constexpr bool is_floating_point = edvar::meta::structs::is_floating_point<in_type>::value;
#pragma endregion is_floating_point

#pragma region is_same_type
namespace structs {
template <typename in_type, typename other> struct is_same_type {
    static constexpr bool value = false;
};
template <typename in_type> struct is_same_type<in_type, in_type> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type, typename other>
inline constexpr bool is_same_type = edvar::meta::structs::is_same_type<in_type, other>::value;
#pragma endregion is_same_type
#pragma region is_reference
namespace structs {
template <typename in_type> struct is_reference {
    static constexpr bool value = false;
};
template <typename in_type> struct is_reference<in_type&> {
    static constexpr bool value = true;
};
template <typename in_type> struct is_reference<in_type&&> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type> inline constexpr bool is_reference = edvar::meta::structs::is_reference<in_type>::value;
#pragma endregion is_reference

#pragma region remove_reference
namespace structs {
template <typename in_type> struct remove_reference {
    typedef in_type type;
};
template <typename in_type> struct remove_reference<in_type&> {
    typedef in_type type;
};
template <typename in_type> struct remove_reference<in_type&&> {
    typedef in_type type;
};
} // namespace structs
template <typename T> using remove_reference = typename edvar::meta::structs::remove_reference<T>::type;
#pragma endregion remove_reference

#pragma region is_pointer
namespace structs {
template <typename in_type> struct is_pointer {
    static constexpr bool value = false;
};
template <typename in_type> struct is_pointer<in_type*> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type> inline constexpr bool is_pointer = edvar::meta::structs::is_pointer<in_type>::value;
#pragma endregion is_pointer
#pragma region remove_pointer
namespace structs {
template <typename in_type> struct remove_pointer {
    typedef in_type type;
};
template <typename in_type> struct remove_pointer<in_type*> {
    typedef in_type type;
};
template <typename in_type> struct remove_pointer<in_type* const> {
    typedef in_type type;
};
template <typename in_type> struct remove_pointer<in_type* volatile> {
    typedef in_type type;
};
template <typename in_type> struct remove_pointer<in_type* const volatile> {
    typedef in_type type;
};
} // namespace structs
template <typename in_type> using remove_pointer = typename edvar::meta::structs::remove_pointer<in_type>::type;
#pragma endregion remove_pointer
#pragma region is_const
namespace structs {
template <typename in_type> struct is_const {
    static constexpr bool value = false;
};
template <typename in_type> struct is_const<const in_type> {
    static constexpr bool value = true;
};
template <typename in_type> struct is_const<const volatile in_type> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename T> inline constexpr bool is_const = edvar::meta::structs::is_const<T>::value;
#pragma endregion is_const
#pragma region remove_const
namespace structs {
template <typename in_type> struct remove_const {
    typedef in_type type;
};
template <typename in_type> struct remove_const<const in_type> {
    typedef in_type type;
};
template <typename in_type> struct remove_const<const volatile in_type> {
    typedef volatile in_type type;
};
} // namespace structs
template <typename in_type> using remove_const = typename edvar::meta::structs::remove_const<in_type>::type;
#pragma endregion remove_const
#pragma region is_volatile
namespace structs {
template <typename in_type> struct is_volatile {
    static constexpr bool value = false;
};
template <typename in_type> struct is_volatile<volatile in_type> {
    static constexpr bool value = true;
};
template <typename in_type> struct is_volatile<const volatile in_type> {
    static constexpr bool value = true;
};
} // namespace structs
template <typename in_type> inline constexpr bool is_volatile = edvar::meta::structs::is_volatile<in_type>::value;
#pragma endregion is_volatile
#pragma region remove_volatile
namespace structs {
template <typename in_type> struct remove_volatile {
    typedef in_type type;
};
template <typename in_type> struct remove_volatile<volatile in_type> {
    typedef in_type type;
};
template <typename in_type> struct remove_volatile<const volatile in_type> {
    typedef const in_type type;
};
} // namespace structs
template <typename in_type> using remove_volatile = typename edvar::meta::structs::remove_volatile<in_type>::type;
#pragma region remove_cv
template <typename in_type> using remove_cv = typename edvar::meta::remove_const<edvar::meta::remove_volatile<in_type>>;
#pragma endregion remove_cv
#pragma region decay
namespace structs {
template <typename in_type> struct decay_helper {
    using type = typename edvar::meta::remove_cv<in_type>::type;
};
template <typename in_type, uint32 length> struct decay_helper<in_type[length]> {
    using type = typename edvar::meta::remove_cv<in_type>::type*;
};
template <typename in_type> struct decay_helper<in_type[]> {
    using type = typename edvar::meta::remove_cv<in_type>::type*;
};
template <typename return_type, typename... args> struct decay_helper<return_type(args...)> {
    using type = return_type (*)(args...);
};
template <typename object_type, typename return_type, typename... args>
struct decay_helper<return_type (object_type::*)(args...)> {
    using type = return_type (object_type::*)(args...);
};

template <typename in_type> struct decay {
private:
    using cv_removed_type = typename remove_cv<in_type>::type;

public:
    using type = typename decay_helper<cv_removed_type>::type;
};
} // namespace structs
template <typename T> using decay = typename edvar::meta::structs::decay<T>::type;
#pragma endregion decay

#pragma region conditional_type
namespace structs {
template <bool condition, typename true_type, typename false_type> struct conditional_type {
    using type = true_type;
};
template <typename true_type, typename false_type> struct conditional_type<false, true_type, false_type> {
    using type = false_type;
};
} // namespace structs
template <bool condition, typename true_type, typename false_type>
using conditional_type = typename edvar::meta::structs::conditional_type<condition, true_type, false_type>::type;
#pragma endregion conditional_type
} // namespace edvar::meta