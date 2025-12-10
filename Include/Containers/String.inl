#pragma once
#include "String.hpp"

namespace Edvar::Containers {
template <typename CharT> StringBase<CharT>::StringBase() { Buffer.AddZeroed(1); }
template <typename CharT> StringBase<CharT>::StringBase(const char16_t* raw_str) {
    const int32_t length = Utils::CStrings::Length(raw_str);
    SetDataFromRawString(raw_str, length);
}
template <typename CharT> StringBase<CharT>::StringBase(const char* raw_str) {
    const int32_t length = Utils::CStrings::Length(raw_str);
    SetDataFromRawString(raw_str, length);
}
template <typename CharT> StringBase<CharT>::StringBase(const wchar_t* raw_str) {
    const int32_t length = Utils::CStrings::Length(raw_str);
    SetDataFromRawString(raw_str, length);
}
template <typename CharT>
StringBase<CharT>::StringBase(const CharT* raw_str, const int32_t length)
    requires(Edvar::Utils::IsCharTypeV<CharT>)
{
    SetDataFromRawString(raw_str, length);
}
template <typename CharT> StringBase<CharT>::StringBase(const StringBase& other) noexcept {
    SetDataFromRawString(other.Data(), other.Length());
}
template <typename CharT> StringBase<CharT>::StringBase(StringBase&& other) noexcept {
    Buffer = std::move(other.Buffer);
}
template <typename CharT> StringBase<CharT>& StringBase<CharT>::operator=(const StringBase& other) noexcept {
    SetDataFromRawString(other.Data(), other.Length());
    return *this;
}
template <typename CharT> StringBase<CharT>& StringBase<CharT>::operator=(StringBase&& other) noexcept {
    Buffer = std::move(other.Buffer);
    return *this;
}
template <typename CharT>
template <typename... ArgsT>
StringBase<char16_t> StringBase<CharT>::PrintF(const CharT* format, ArgsT... args) {
    const int32_t size = Utils::CStrings::SPrintF(nullptr, 0, format, args...); // Get required size
    // SPrintF failed
    if (size <= 0) {
        return {};
    }
    auto* buffer = new CharT[size + 1];
    buffer[size] = 0;
    Utils::CStrings::SPrintF(buffer, size + 1, format, args...);
    StringBase<char16_t> result(buffer, size);
    delete[] buffer;
    return result;
}
template <typename CharT> template <typename OtherCharT> StringBase<OtherCharT> StringBase<CharT>::ConvertTo() const {
    auto* convertedBuffer = Utils::CStrings::CreateConvertedString<OtherCharT>(Data());
    StringBase<OtherCharT> result(convertedBuffer);
    delete[] convertedBuffer;
    return result;
}
template <typename CharT>
template <typename OtherCharT>
bool StringBase<CharT>::operator==(const StringBase<OtherCharT>& other) const {
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
template <typename CharT>
template <typename OtherCharT>
bool StringBase<CharT>::operator!=(const StringBase<OtherCharT>& other) const {
    return !(*this == other);
}
template <typename CharT>
StringBase<CharT> StringBase<CharT>::Equals(const StringBase& other, bool ignoreCase,
                                            const I18N::Locale& locale) const {
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
template <typename CharT> StringBase<CharT> StringBase<CharT>::Lower(const I18N::Locale& locale) const {
    const int32_t size = Utils::CStrings::ToLower(Data(), nullptr, 0, locale);
    auto* buffer = new char16_t[size + 1];
    Utils::CStrings::ToLower(Data(), buffer, size + 1, locale);
    StringBase result(buffer, size);
    delete[] buffer;
    return result;
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::SubString(int32_t startIndex, int32_t length) const {
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
template <typename CharT> StringBase<CharT> StringBase<CharT>::TrimStart() const {
    int32_t startIndex = 0;
    while (startIndex < Length() && Utils::CStrings::IsWhitespace(Data()[startIndex])) {
        ++startIndex;
    }
    return SubString(startIndex, Length() - startIndex);
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::TrimEnd() const {
    int32_t endIndex = Length() - 1;
    while (endIndex >= 0 && Utils::CStrings::IsWhitespace(Data()[endIndex])) {
        --endIndex;
    }
    return SubString(0, endIndex + 1);
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::Trim() const { return TrimStart().TrimEnd(); }
template <typename CharT> int32_t StringBase<CharT>::IndexOf(CharT toFind, int32_t startIndex) const {
    for (int32_t i = startIndex; i < Length(); i++) {
        if (Data()[i] == toFind) {
            return i;
        }
    }
    return -1;
}
template <typename CharT> int32_t StringBase<CharT>::LastIndexOf(CharT toFind, int32_t startIndex) const {
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
template <typename CharT> int32_t StringBase<CharT>::Count(CharT toFind) const {
    int32_t count = 0;
    for (int32_t i = 0; i < Length(); i++) {
        if (Data()[i] == toFind) {
            ++count;
        }
    }
    return count;
}
template <typename CharT> bool StringBase<CharT>::StartsWith(const StringBase& prefix) const {
    if (prefix.Length() > Length()) {
        return false;
    }
    for (int32_t i = 0; i < prefix.Length(); i++) {
        if (Data()[i] != prefix.Data()[i]) {
            return false;
        }
    }
    return true;
}
template <typename CharT> bool StringBase<CharT>::EndsWith(const StringBase& suffix) const {
    if (suffix.Length() > Length()) {
        return false;
    }
    int32_t startIndex = Length() - suffix.Length();
    for (int32_t i = 0; i < suffix.Length(); i++) {
        if (Data()[startIndex + i] != suffix.Data()[i]) {
            return false;
        }
    }
    return true;
}
template <typename CharT> bool StringBase<CharT>::IsEmpty() const { return Length() == 0 || IsWhitespace(); }
template <typename CharT> bool StringBase<CharT>::IsWhitespace() const {
    for (int32_t i = 0; i < Length(); i++) {
        if (!Utils::CStrings::IsWhitespace(Data()[i])) {
            return false;
        }
    }
    return true;
}
template <typename CharT> CharT& StringBase<CharT>::operator[](int32_t index) { return Buffer.Get(index); }
template <typename CharT> const CharT& StringBase<CharT>::operator[](int32_t index) const { return Buffer.Get(index); }
template <typename CharT>
template <typename OtherCharT>
StringBase<CharT> StringBase<CharT>::operator+(const StringBase<OtherCharT>& other) const {
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
template <typename CharT>
template <typename CharArrT>
StringBase<CharT> StringBase<CharT>::operator+(const CharArrT* other) const
    requires(Edvar::Utils::IsCharTypeV<CharArrT>)
{
    StringBase<CharT> otherString(other);
    return *this + otherString;
}
template <typename CharT>
template <typename AppendT>
StringBase<CharT> StringBase<CharT>::operator+(const AppendT& other) const
    requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>)
{
    StringBase<CharT> returnString = *this;
    returnString.Buffer.Resize(Length() + 1);
    returnString.Buffer.Get(Length()) = static_cast<CharT>(other);
    return returnString;
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::ToString() const { return *this; }
template <typename CharT>
template <typename T>
StringBase<char16_t> StringBase<CharT>::NumberToString(const T& value, const NumberFormattingRule& FormattingRule,
                                                       int base)
    requires(std::is_arithmetic_v<T>)
{
    if constexpr (std::is_integral_v<T>) {
        if (std::is_signed_v<T>) {
            const int32_t bufferLength =
                Utils::CStrings::NumberToString(static_cast<int64_t>(value), nullptr, 0, base, FormattingRule);
            char16_t* buffer = new char16_t[bufferLength];
            Utils::CStrings::NumberToString(static_cast<int64_t>(value), buffer, bufferLength, base, FormattingRule);
            StringBase<char16_t> result(buffer, bufferLength - 1); // -1
            delete[] buffer;
            return result;
        } else {
            const int32_t bufferLength =
                Utils::CStrings::NumberToString(static_cast<uint64_t>(value), nullptr, 0, base, FormattingRule);
            char16_t* buffer = new char16_t[bufferLength];
            Utils::CStrings::NumberToString(static_cast<uint64_t>(value), buffer, bufferLength, base, FormattingRule);
            StringBase<char16_t> result(buffer, bufferLength - 1); // -1
            delete[] buffer;
            return result;
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        const int32_t bufferLength =
            Utils::CStrings::FloatToString(static_cast<double>(value), nullptr, 0, FormattingRule);
        char16_t* buffer = new char16_t[bufferLength];
        Utils::CStrings::FloatToString(static_cast<double>(value), buffer, bufferLength, FormattingRule);
        StringBase<char16_t> result(buffer, bufferLength);
        delete[] buffer;
        return result;
    }
    return {};
}
template <typename CharT>
template <typename FromT>
void StringBase<CharT>::SetDataFromRawString(const FromT* raw_str, const int32_t length) {

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
template <typename CharT> StringBase<CharT> StringBase<CharT>::Upper(const I18N::Locale& locale) const {
    const int32_t size = Utils::CStrings::ToLower(Data(), nullptr, 0, locale);
    char16_t* buffer = new char16_t[size + 1];
    Utils::CStrings::ToUpper(Data(), buffer, size + 1, locale);
    StringBase result(buffer, size);
    delete[] buffer;
    return result;
}

} // namespace Edvar::Containers