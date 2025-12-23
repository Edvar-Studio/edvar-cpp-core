#pragma once

namespace Edvar::Utils {
// Exposed index sequence helper (moved out of Tuple internals)
template <uint32_t... Is> struct IndexSequence {};

template <uint32_t N, uint32_t... Is> struct MakeIndexSequenceImpl {
    using type = typename MakeIndexSequenceImpl<N - 1, N - 1, Is...>::type;
};

template <uint32_t... Is> struct MakeIndexSequenceImpl<0, Is...> {
    using type = IndexSequence<Is...>;
};

template <uint32_t N> using MakeIndexSequence = typename MakeIndexSequenceImpl<N>::type;

template <typename... Args> class BiggerType {
public:
    using Type = void;
};
template <typename T> class BiggerType<T> {
public:
    using Type = T;
};
template <typename T1, typename T2, typename... Args> class BiggerType<T1, T2, Args...> {
public:
    using Type =
        typename BiggerType<typename std::conditional<(sizeof(T1) >= sizeof(T2)), T1, T2>::type, Args...>::Type;
};

template <typename... Args> using BiggerTypeT = typename BiggerType<Args...>::Type;

template <typename... Args> class SmallerType {
public:
    using Type = void;
};
template <typename T> class SmallerType<T> {
public:
    using Type = T;
};
template <typename T1, typename T2, typename... Args> class SmallerType<T1, T2, Args...> {
public:
    using Type =
        typename SmallerType<typename std::conditional<(sizeof(T1) <= sizeof(T2)), T1, T2>::type, Args...>::Type;
};

template <typename... Args> using SmallerTypeT = typename SmallerType<Args...>::Type;

template <typename T>
static constexpr bool IsCharTypeV =
    std::is_same_v<std::remove_cv_t<T>, char> || std::is_same_v<std::remove_cv_t<T>, wchar_t> ||
    std::is_same_v<std::remove_cv_t<T>, char8_t> || std::is_same_v<std::remove_cv_t<T>, char16_t> ||
    std::is_same_v<std::remove_cv_t<T>, char32_t>;

template <typename T>
concept HasNoArgumentToStringMethod = requires(const T& a) {
    { a.ToString() } -> std::convertible_to<Containers::String>;
};

template <typename T>
concept HasOptionTakingToStringMethod = requires(const T& a, const Containers::String& opts) {
    { a.ToString(opts) } -> std::convertible_to<Containers::String>;
};

} // namespace Edvar::Utils