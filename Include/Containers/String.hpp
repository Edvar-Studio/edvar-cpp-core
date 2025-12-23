#pragma once

#include "I18N/Locale.hpp"
#include "Utils/CString.hpp"
#include "Containers/List.hpp"

namespace Edvar::Containers {
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
    [[nodiscard]] int32_t Length() const { return Buffer.Length() - 1; }

    template <typename OtherCharT> StringBase<OtherCharT> ConvertTo() const;
    /**
     * @brief Compares two strings for content equality (case-sensitive), converting the other string if necessary.
     */
    template <typename OtherCharT> bool operator==(const StringBase<OtherCharT>& other) const;
    template <typename OtherCharT>
    bool operator==(const OtherCharT* other) const
        requires(Edvar::Utils::IsCharTypeV<OtherCharT>);

    template <typename OtherCharT> bool operator!=(const StringBase<OtherCharT>& other) const;
    template <typename OtherCharT>
    bool operator!=(const OtherCharT* other) const
        requires(Edvar::Utils::IsCharTypeV<OtherCharT>);

    StringBase Equals(const StringBase& other, bool ignoreCase = false,
                      const I18N::Locale& locale = I18N::Locale::Default()) const;

    StringBase Lower(const I18N::Locale& locale = I18N::Locale::Default()) const;

    StringBase Upper(const I18N::Locale& locale = I18N::Locale::Default()) const;

    StringBase SubString(int32_t startIndex, int32_t length = -1) const;

    StringBase TrimStart() const;

    StringBase TrimEnd() const;

    StringBase Trim() const;

    int32_t IndexOf(CharT toFind, int32_t startIndex = 0) const;
    int32_t IndexOf(StringBase& toFind, int32_t startIndex = 0) const;

    int32_t LastIndexOf(CharT toFind, int32_t startIndex = -1) const;
    int32_t LastIndexOf(StringBase& toFind, int32_t startIndex = -1) const;

    int32_t Count(CharT toFind) const;

    bool StartsWith(const StringBase& prefix) const;

    bool EndsWith(const StringBase& suffix) const;

    bool IsEmpty() const;

    bool IsWhitespace() const;

    CharT& operator[](int32_t index);
    const CharT& operator[](int32_t index) const;
    template <typename OtherCharT> StringBase operator+(const StringBase<OtherCharT>& other) const;
    template <typename OtherCharT> StringBase& operator+=(const StringBase<OtherCharT>& other);
    template <typename CharArrT>
    StringBase operator+(const CharArrT* other) const
        requires(Edvar::Utils::IsCharTypeV<CharArrT>);
    template <typename CharArrT>
    StringBase& operator+=(const CharArrT* other)
        requires(Edvar::Utils::IsCharTypeV<CharArrT>);
    template <typename AppendT>
    StringBase operator+(const AppendT& other) const
        requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>);
    template <typename AppendT>
    StringBase& operator+=(const AppendT& other)
        requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>);

    StringBase ToString() const;

    template <typename T>
    static StringBase<char16_t>
    NumberToString(const T& value, const NumberFormattingRule& FormattingRule = NumberFormattingRule(), int base = 10)
        requires(std::is_arithmetic_v<T>);

    template <typename... ArgsT> static StringBase Format(const StringBase& inFormatString, ArgsT&&... args);

    void EnsureCapacity(uint32_t newCapacity) { Buffer.EnsureCapacity(newCapacity); }

    Containers::List<StringBase> Split(CharT delimiter, bool ignoreEmpty = true) const;
    Containers::List<StringBase> Split(const StringBase& delimiter, bool ignoreEmpty = true) const;

    StringBase Replace(const StringBase& toFind, const StringBase& toReplaceWith, int32_t startIndex = 0,
                       int32_t* replaceEnd = nullptr) const;
    StringBase ReplaceAll(const StringBase& toFind, const StringBase& toReplaceWith) const;

    StringBase PadLeft(int32_t count) const;
    StringBase PadRight(int32_t count) const;

private:
    Containers::List<CharT> Buffer;

    template <typename FromT> void SetDataFromRawString(const FromT* raw_str, int32_t length);
};

template <typename T, typename CharT> struct StringFormatter {};

template <typename CharT> struct StringFormatter<StringBase<CharT>, CharT> {
    static StringBase<CharT> Format(const StringBase<CharT>& arg, const StringBase<CharT>& formatOptionString) {
        // we don't care about the format options for String. Just return the argument as is.
        return arg;
    }
};

template <typename CharT> struct StringFormatter<const CharT*, CharT> {
    static StringBase<CharT> Format(const CharT* arg, const StringBase<CharT>& formatOptionString) {
        // we don't care about the format options for raw strings. Just return the argument as is.
        return StringBase<CharT>(arg);
    }
};

template <typename CharT> struct StringFormatter<CharT, CharT> {
    static StringBase<CharT> Format(const CharT& arg, const StringBase<CharT>& formatOptionString) {
        StringBase<CharT> result;
        result += arg;
        return result;
    }
};

template <typename CharT> struct StringFormatter<double, CharT> {
    static StringBase<CharT> Format(double arg, const StringBase<CharT>& formatOptionString) {
        StringBase<CharT> result;
        NumberFormattingRule formattingRule;
        for (List<StringBase<CharT>> splitOptions = formatOptionString.Split(';');
             const StringBase<CharT>& option : splitOptions) {
            List<StringBase<CharT>> keyValue = option.Split('=');
            if (keyValue.Length() > 1) {
                if (keyValue[0].Trim() == "p") {
                    // setup precision
                    auto valueStr = keyValue[1].Trim().template ConvertTo<char16_t>();
                    formattingRule.MinimumFractionalDigits = Utils::CStrings::StringToInt64(*valueStr);
                    formattingRule.MaximumFractionalDigits = formattingRule.MinimumFractionalDigits;
                    continue;
                }
                if (keyValue[0].Trim() == "w") {
                    // setup width
                    // TODO: add support for width
                }
            }
        }
        result = StringBase<CharT>::NumberToString(arg, formattingRule);
        return result;
    }
};

template <typename CharT> struct StringFormatter<float, CharT> {
    static StringBase<CharT> Format(float arg, const StringBase<CharT>& formatOptionString) {
        return StringFormatter<double, CharT>::Format(static_cast<double>(arg), formatOptionString);
    }
};
// pointer types
template <typename T, typename CharT>
    requires(std::is_pointer_v<T> && !Utils::IsCharTypeV<T>)
struct StringFormatter<T, CharT> {
    static StringBase<CharT> Format(T arg, const StringBase<CharT>& formatOptionString) {
        if (arg == nullptr) {
            return StringBase<CharT>(u"nullptr");
        }
        auto address = reinterpret_cast<uintptr_t>(arg);
        int32_t bufferSize = Utils::CStrings::DataToHexString(address, nullptr, 0, true, true, 16);
        if (bufferSize <= 0) {
            return {u"(pointer formatter error)"};
        }
        auto* buffer = new char16_t[bufferSize];
        Utils::CStrings::DataToHexString(address, buffer, bufferSize, true, true, 16);
        StringBase<CharT> result(buffer);
        delete[] buffer;
        return result;
    }
};

template <typename T, typename CharT>
    requires(std::is_integral_v<T>)
struct StringFormatter<T, CharT> {
    static StringBase<CharT> Format(T arg, const StringBase<CharT>& formatOptionString) {
        NumberFormattingRule formattingRule;
        int base = 10;
        int32_t width = -1;
        bool zeroFill = false;
        for (List<StringBase<CharT>> splitOptions = formatOptionString.Split(';');
             const StringBase<CharT>& option : splitOptions) {
            if (option == "HEX") {
                // format hex
                base = 16;
                formattingRule.UpperCase = true;
            } else if (option == "hex") {
                base = 16;
                formattingRule.UpperCase = false;
            }
            if (option == "ZEROFILL") {
                zeroFill = true;
            }
            if (option == "#") {
                formattingRule.UseAlternateForm = true;
            }
            List<StringBase<CharT>> keyValue = option.Split('=');
            if (keyValue.Length() > 1) {
                if (keyValue[0].Trim() == "w") {
                    // setup width
                    auto valueStr = keyValue[1].Trim().template ConvertTo<char16_t>();
                    width = static_cast<int32_t>(Utils::CStrings::StringToInt64(*valueStr));
                    continue;
                }
            }
        }
        StringBase<CharT> result;
        if (width != -1 && zeroFill) {
            formattingRule.MinimumIntegralDigits = width;
            formattingRule.MaximumIntegralDigits = formattingRule.MinimumIntegralDigits;
        }
        if (base > 10) {
            result = StringBase<CharT>::NumberToString(static_cast<uint64_t>(arg), formattingRule, base);
        } else {
            result = StringBase<CharT>::NumberToString(arg, formattingRule, base);
        }
        // do padding
        if (width != -1 && !zeroFill) {
            // push to right
            if (width > result.Length()) {
                result = result.PadLeft(width - result.Length());
            }
        }

        return result;
    }
};
} // namespace Edvar::Containers