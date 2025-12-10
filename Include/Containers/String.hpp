#pragma once

#include "I18N/Locale.hpp"
#include "Utils/CString.hpp"

namespace Edvar {
struct FormatOpts {
    const char16_t* FormatOptionString;
    int32_t FormatOptionStringLength;

    bool IsValid() const { return FormatOptionStringLength > 0 && FormatOptionString != nullptr; }
};
} // namespace Edvar
namespace Edvar::Containers {

template <typename CharT, typename T>
concept HasMemberToStringMethod = requires(T t) {
    { t.ToString() } -> std::convertible_to<Containers::StringBase<CharT>>;
};
template <typename CharT, typename T>
concept HasMemberToStringMethodWithFormatOpts = requires(T t, Edvar::FormatOpts opts) {
    { t.ToString(opts) } -> std::convertible_to<Containers::StringBase<CharT>>;
};
template <typename CharT, typename T>
concept HasFreeToStringFunction = requires(T t) {
    { ToString(t) } -> std::convertible_to<Containers::StringBase<CharT>>;
};
template <typename CharT, typename T>
concept HasFreeToStringFunctionWithFormatOpts = requires(T t, Edvar::FormatOpts opts) {
    { ToString(t, opts) } -> std::convertible_to<Containers::StringBase<CharT>>;
};
template <typename CharT, typename T>
concept IsConvertibleToString = HasMemberToStringMethod<CharT, T> || HasMemberToStringMethodWithFormatOpts<CharT, T> ||
                                HasFreeToStringFunction<CharT, T> || HasFreeToStringFunctionWithFormatOpts<CharT, T>;

template <typename CharT> struct StringBase {
    StringBase();
    StringBase(const char16_t* raw_str);
    StringBase(const char* raw_str);
    StringBase(const wchar_t* raw_str);

    StringBase(const CharT* raw_str, int32_t length)
        requires(Edvar::Utils::IsCharTypeV<CharT>);

    StringBase(const StringBase& other) noexcept;
    StringBase(StringBase&& other) noexcept;

    StringBase& operator=(const StringBase& other) noexcept;
    StringBase& operator=(StringBase&& other) noexcept;

    const CharT* operator*() const { return Data(); }

    ~StringBase() = default;

    [[nodiscard]] const CharT* Data() const { return Buffer.Data(); }
    [[nodiscard]] CharT* Data() { return Buffer.Data(); }
    [[nodiscard]] int32_t Length() const { return Buffer.Length(); }

    template <typename... ArgsT> static StringBase<char16_t> PrintF(const CharT* format, ArgsT... args);

    template <typename OtherCharT> StringBase<OtherCharT> ConvertTo() const;
    /**
     * @brief Compares two strings for content equality (case-sensitive), converting the other string if necessary.
     */
    template <typename OtherCharT> bool operator==(const StringBase<OtherCharT>& other) const;

    template <typename OtherCharT> bool operator!=(const StringBase<OtherCharT>& other) const;

    StringBase Equals(const StringBase& other, bool ignoreCase = false,
                      const I18N::Locale& locale = I18N::Locale::Default()) const;

    StringBase Lower(const I18N::Locale& locale = I18N::Locale::Default()) const;

    StringBase Upper(const I18N::Locale& locale = I18N::Locale::Default()) const;

    StringBase SubString(int32_t startIndex, int32_t length = -1) const;

    StringBase TrimStart() const;

    StringBase TrimEnd() const;

    StringBase Trim() const;

    int32_t IndexOf(CharT toFind, int32_t startIndex = 0) const;

    int32_t LastIndexOf(CharT toFind, int32_t startIndex = -1) const;

    int32_t Count(CharT toFind) const;

    bool StartsWith(const StringBase& prefix) const;

    bool EndsWith(const StringBase& suffix) const;

    bool IsEmpty() const;

    bool IsWhitespace() const;

    CharT& operator[](int32_t index);
    const CharT& operator[](int32_t index) const;
    template <typename OtherCharT> StringBase operator+(const StringBase<OtherCharT>& other) const;
    template <typename CharArrT>
    StringBase operator+(const CharArrT* other) const
        requires(Edvar::Utils::IsCharTypeV<CharArrT>);

    template <typename AppendT>
    StringBase operator+(const AppendT& other) const
        requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>);

    StringBase ToString() const;

    template <typename T>
    static StringBase<char16_t>
    NumberToString(const T& value, const NumberFormattingRule& FormattingRule = NumberFormattingRule(), int base = 10)
        requires(std::is_arithmetic_v<T>);

private:
    Containers::List<CharT> Buffer;

    template <typename FromT> void SetDataFromRawString(const FromT* raw_str, int32_t length);
};

} // namespace Edvar::Containers