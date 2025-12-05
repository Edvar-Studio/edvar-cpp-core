#pragma once

#include "I18N/Locale.hpp"
#include "Memory/Ops.hpp"
#include "Utils/CString.hpp"
#include "Utils/Meta.hpp"
#include <type_traits>
namespace Edvar::Containers {

template <typename CharT> struct StringBase {
    StringBase() { Buffer.AddZeroed(1); };
    StringBase(const char16_t* raw_str) {
        const int32_t length = Utils::CStrings::Length(raw_str);
        SetDataFromRawString(raw_str, length);
    }
    StringBase(const char* raw_str) {
        const int32_t length = Utils::CStrings::Length(raw_str);
        SetDataFromRawString(raw_str, length);
    }
    StringBase(const wchar_t* raw_str) {
        const int32_t length = Utils::CStrings::Length(raw_str);
        SetDataFromRawString(raw_str, length);
    }

    StringBase(const CharT* raw_str, const int32_t length)
        requires(Edvar::Utils::IsCharTypeV<CharT>)
    {
        SetDataFromRawString(raw_str, length);
    }

    StringBase(const StringBase& other) noexcept { SetDataFromRawString(other.Data(), other.Length()); }
    StringBase(const StringBase&& other) noexcept { Buffer = std::move(other.Buffer); }

    StringBase& operator=(const StringBase& other) noexcept {
        SetDataFromRawString(other.Data(), other.Length());
        return *this;
    }
    StringBase& operator=(const StringBase&& other) noexcept {
        Buffer = std::move(other.Buffer);
        return *this;
    }

    ~StringBase() = default;

    [[nodiscard]] const CharT* Data() const { return Buffer.Data(); }
    [[nodiscard]] CharT* Data() { return Buffer.Data(); }
    [[nodiscard]] int32_t Length() const { return Buffer.Length(); }

    template <typename... ArgsT> static StringBase<char16_t> PrintF(const CharT* format, ArgsT... args) {
        const int32_t size = Utils::CStrings::SPrintF(nullptr, 0, format, args...); // Get required size
        // SPrintF failed
        if (size <= 0) {
            return {};
        }
        CharT* buffer = new CharT[size + 1];
        buffer[size] = 0;
        Utils::CStrings::SPrintF(buffer, size + 1, format, args...);
        StringBase<char16_t> result(buffer, size);
        delete[] buffer;
        return result;
    }

    template <typename OtherCharT> StringBase<OtherCharT> ConvertTo() const {
        auto* convertedBuffer = Utils::CStrings::CreateConvertedString<OtherCharT>(Data());
        StringBase<OtherCharT> result(convertedBuffer);
        delete[] convertedBuffer;
        return result;
    }
    /**
     * @brief Compares two strings for content equality (case sensitive), converting the other string if necessary.
     */
    template <typename OtherCharT> bool operator==(const StringBase<OtherCharT>& other) const {
        if (Length() != other.Length()) {
            return false;
        }
        if constexpr (std::is_same_v<CharT, OtherCharT>) {
            for (int32_t i = 0; i < Length(); i++) {
                if (Data()[i] != other.Data()[i]) {
                    return false;
                }
            }
            return true;
        } else {
            StringBase<CharT> convertedOther = other.template ConvertTo<CharT>();
            for (int32_t i = 0; i < Length(); i++) {
                CharT thisChar = Data()[i];
                CharT otherChar = convertedOther.Data()[i];
                if (thisChar != otherChar) {
                    return false;
                }
            }
            return true;
        }
    }

    template <typename OtherCharT> bool operator!=(const StringBase<OtherCharT>& other) const {
        return !(*this == other);
    }

    StringBase Equals(const StringBase& other, bool ignoreCase = false,
                      const I18N::Locale& locale = I18N::Locale::Default()) const {
        if (Length() != other.Length()) {
            return false;
        }
        if (!ignoreCase) {
            return *this == other;
        } else {
            StringBase thisLower = Lower(locale);
            StringBase otherLower = other.Lower(locale);
            return thisLower == otherLower;
        }
    }

    StringBase Lower(const I18N::Locale& locale = I18N::Locale::Default()) const {
        const int32_t size = Utils::CStrings::ToLower(Data(), nullptr, 0, locale);
        char16_t* buffer = new char16_t[size + 1];
        Utils::CStrings::ToLower(Data(), buffer, size + 1, locale);
        StringBase result(buffer, size);
        delete[] buffer;
        return result;
    }

    StringBase Upper(const I18N::Locale& locale = I18N::Locale::Default()) const {
        const int32_t size = Utils::CStrings::ToLower(Data(), nullptr, 0, locale);
        char16_t* buffer = new char16_t[size + 1];
        Utils::CStrings::ToUpper(Data(), buffer, size + 1, locale);
        StringBase result(buffer, size);
        delete[] buffer;
        return result;
    }

    StringBase SubString(int32_t startIndex, int32_t length = -1) const {
        // if startIndex is less than 0 set start as 0
        if (startIndex < 0) {
            startIndex = 0;
        }
        // if startIndex exceeds string length, return empty string
        if (startIndex >= Length()) {
            return StringBase();
        }
        // if length is < 0 take until end of string
        if (length < 0) {
            length = Length() - startIndex;
        }
        // if startIndex + length exceeds string length, adjust length
        if (startIndex + length > Length()) {
            length = Length() - startIndex;
        }
        return StringBase(Data() + startIndex, length);
    }

    StringBase TrimStart() const {
        int32_t startIndex = 0;
        while (startIndex < Length() && Utils::CStrings::IsWhitespace(Data()[startIndex])) {
            ++startIndex;
        }
        return SubString(startIndex, Length() - startIndex);
    }

    StringBase TrimEnd() const {
        int32_t endIndex = Length() - 1;
        while (endIndex >= 0 && Utils::CStrings::IsWhitespace(Data()[endIndex])) {
            --endIndex;
        }
        return SubString(0, endIndex + 1);
    }

    StringBase Trim() const { return TrimStart().TrimEnd(); }

    int32_t IndexOf(CharT toFind, int32_t startIndex = 0) const {
        for (int32_t i = startIndex; i < Length(); i++) {
            if (Data()[i] == toFind) {
                return i;
            }
        }
        return -1;
    }

    int32_t LastIndexOf(CharT toFind, int32_t startIndex = -1) const {
        if (startIndex < 0 || startIndex >= Length()) {
            startIndex = Length() - 1;
        }
        for (int32_t i = startIndex; i >= 0; i--) {
            if (Data()[i] == toFind) {
                return i;
            }
        }
        return -1;
    }

    int32_t Count(CharT toFind) const {
        int32_t count = 0;
        for (int32_t i = 0; i < Length(); i++) {
            if (Data()[i] == toFind) {
                ++count;
            }
        }
        return count;
    }

    bool IsEmpty() const { return Length() == 0 || IsWhitespace(); }

    bool IsWhitespace() const {
        for (int32_t i = 0; i < Length(); i++) {
            if (!Utils::CStrings::IsWhitespace(Data()[i])) {
                return false;
            }
        }
        return true;
    }

    CharT& operator[](int32_t index) { return Buffer.Get(index); }
    const CharT& operator[](int32_t index) const { return Buffer.Get(index); }
    template <typename OtherCharT> StringBase operator+(const StringBase<OtherCharT>& other) const {
        StringBase result;
        if constexpr (std::is_same_v<OtherCharT, CharT>) {
            result.Buffer.Resize(Length() + other.Length() + 1);
            Memory::CopyMemory(result.Buffer.Data() + Length(), other.Data(), other.Length());
            result.Buffer.Get(Length() + other.Length()) = 0;
        } else {
            StringBase<CharT> convertedOther = other.template ConvertTo<CharT>();
            result.Buffer.Resize(Length() + convertedOther.Length() + 1);
            Memory::CopyMemory(result.Buffer.Data() + Length(), convertedOther.Data(), convertedOther.Length());
            result.Buffer.Get(Length() + convertedOther.Length()) = 0;
        }
        return result;
    }
    template <typename CharArrT>
    StringBase operator+(const CharArrT* other) const
        requires(Edvar::Utils::IsCharTypeV<CharArrT>)
    {
        StringBase<CharT> otherString(other);
        return *this + otherString;
    }

    template <typename AppendT>
    StringBase operator+(const AppendT& other) const
        requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>)
    {
        StringBase<CharT> returnString = *this;
        returnString.Buffer.Resize(Length() + 1);
        returnString.Buffer.Get(Length()) = static_cast<CharT>(other);
        return returnString;
    }

    StringBase ToString() const { return *this; }

    template <typename T>
    static StringBase<char16_t> NumberToString(const T& value,
                                               const NumberFormattingRule& FormattingRule = NumberFormattingRule())
        requires(std::is_arithmetic_v<T>)
    {
        if constexpr (std::is_integral_v<T>) {
            if (std::is_signed_v<T>) {
                const int32_t bufferLength =
                    Utils::CStrings::NumberToString(static_cast<int64_t>(value), nullptr, 0, 10, FormattingRule);
                char16_t* buffer = new char16_t[bufferLength];
                Utils::CStrings::NumberToString(static_cast<int64_t>(value), buffer, bufferLength, 10, FormattingRule);
                StringBase<char16_t> result(buffer, bufferLength - 1); // -1
                delete[] buffer;
                return result;
            } else {
                const int32_t bufferLength =
                    Utils::CStrings::NumberToString(static_cast<uint64_t>(value), nullptr, 0, 10, FormattingRule);
                char16_t* buffer = new char16_t[bufferLength];
                Utils::CStrings::NumberToString(static_cast<uint64_t>(value), buffer, bufferLength, 10, FormattingRule);
                StringBase<char16_t> result(buffer, bufferLength - 1); // -1
                delete[] buffer;
                return result;
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            const int32_t bufferLength =
                Utils::CStrings::NumberToString(static_cast<double>(value), nullptr, 0, 15, FormattingRule);
            char16_t* buffer = new char16_t[bufferLength];
            Utils::CStrings::NumberToString(static_cast<double>(value), buffer, bufferLength, 15, FormattingRule);
            StringBase<char16_t> result(buffer, bufferLength);
            delete[] buffer;
            return result;
        }
        return {};
    }

private:
    Containers::List<CharT> Buffer;

    template <typename ConvertT>
    static StringBase ConvertObjectToString(const ConvertT& obj)
        requires(Edvar::Utils::HasToStringMethod<ConvertT>)
    {
        return Containers::StringBase<char16_t>(obj.ToString());
    }

    template <typename ConvertT>
    static StringBase ConvertObjectToString(const ConvertT& obj)
        requires(!Edvar::Utils::HasToStringMethod<ConvertT>)
    {
        return Containers::StringBase<char16_t>();
    }

    template <typename FromT> void SetDataFromRawString(const FromT* raw_str, const int32_t length) {

        Buffer.Resize(length + 1);
        Buffer.Get(length) = 0;
        if constexpr (std::is_same_v<FromT, CharT>) {
            Memory::CopyMemory(Buffer.Data(), raw_str, length);
        } else {
            CharT* convertedBuffer = new CharT[length + 1];
            Utils::CStrings::ConvertString(raw_str, convertedBuffer, length + 1);
            Memory::CopyMemory(Buffer.Data(), convertedBuffer, (length + 1));
            delete[] convertedBuffer;
        }
    }
};

} // namespace Edvar::Containers