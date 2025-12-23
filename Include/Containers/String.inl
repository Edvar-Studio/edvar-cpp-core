#pragma once
#include "String.hpp"
#include "Platform/IPlatform.hpp"

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
bool StringBase<CharT>::operator==(const OtherCharT* other) const
    requires(Edvar::Utils::IsCharTypeV<OtherCharT>)
{
    StringBase<OtherCharT> otherString(other);
    return *this == otherString;
}
template <typename CharT>
template <typename OtherCharT>
bool StringBase<CharT>::operator!=(const StringBase<OtherCharT>& other) const {
    return !(*this == other);
}
template <typename CharT>
template <typename OtherCharT>
bool StringBase<CharT>::operator!=(const OtherCharT* other) const
    requires(Edvar::Utils::IsCharTypeV<OtherCharT>)
{
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
template <typename CharT> int32_t StringBase<CharT>::IndexOf(StringBase& toFind, int32_t startIndex) const {
    if (toFind.Length() <= 0) {
        return -1;
    }
    int32_t currentIndex = 0;
    for (currentIndex = IndexOf(toFind[0], currentIndex); currentIndex < Length() && currentIndex != -1;
         currentIndex = IndexOf(toFind[0], currentIndex + 1)) {
        if (SubString(currentIndex, toFind.Length() == toFind)) {
            return currentIndex; // if match found return the current index.
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
template <typename CharT> int32_t StringBase<CharT>::LastIndexOf(StringBase& toFind, int32_t startIndex) const {
    if (startIndex < 0 || startIndex >= Length()) {
        startIndex = Length() - 1;
    }
    int32_t currentIndex = 0;
    for (currentIndex = LastIndexOf(toFind[0], currentIndex); currentIndex >= 0 && currentIndex != -1;
         currentIndex = LastIndexOf(toFind[0], currentIndex - 1)) {
        if (SubString(currentIndex, toFind.Length() == toFind)) {
            return currentIndex; // if match found return the current index.
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
    StringBase copy(*this);
    copy += other;
    return copy;
}
template <typename CharT>
template <typename OtherCharT>
StringBase<CharT>& StringBase<CharT>::operator+=(const StringBase<OtherCharT>& other) { // copy self
    if constexpr (std::is_same_v<OtherCharT, CharT>) {
        uint32_t len = Length();
        uint32_t otherLen = other.Length();
        Buffer.Resize(len + otherLen + 1);
        Memory::CopyMemory(Buffer.Data() + len + 1, other.Data(), otherLen);
        Buffer.Get(len + otherLen + 1) = 0;
    } else {
        uint32_t len = Length();
        StringBase<CharT> convertedOther = other.template ConvertTo<CharT>();
        uint32_t convOtherLen = convertedOther.Length();
        Buffer.Resize(len + convOtherLen + 1);
        Memory::CopyMemory(Buffer.Data() + len + 1, convertedOther.Data(), convOtherLen);
        Buffer.Get(len + convOtherLen + 1) = 0;
    }
    return *this;
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
template <typename CharArrT>
StringBase<CharT>& StringBase<CharT>::operator+=(const CharArrT* other)
    requires(Edvar::Utils::IsCharTypeV<CharArrT>)
{
    *this += StringBase<CharT>(other);
    return *this;
}
template <typename CharT>
template <typename AppendT>
StringBase<CharT> StringBase<CharT>::operator+(const AppendT& other) const
    requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>)
{
    StringBase<CharT> returnString = *this;
    returnString.Buffer.Insert(Length() + 1, other);
    return returnString;
}
template <typename CharT>
template <typename AppendT>
StringBase<CharT>& StringBase<CharT>::operator+=(const AppendT& other)
    requires(!std::is_pointer_v<AppendT> && Edvar::Utils::IsCharTypeV<AppendT>)
{
    Buffer.Insert(Length(), other);
    return *this;
}

template <typename CharT> StringBase<CharT> StringBase<CharT>::ToString() const { return *this; }
template <typename CharT>
template <typename T>
StringBase<char16_t> StringBase<CharT>::NumberToString(const T& value, const NumberFormattingRule& FormattingRule,
                                                       int base)
    requires(std::is_arithmetic_v<T>)
{
    if (base != 10) {
        const int32_t bufferSize =
            Utils::CStrings::DataToHexString(static_cast<uint64_t>(value), nullptr, 0, FormattingRule.UpperCase,
                                             FormattingRule.UseAlternateForm, FormattingRule.MinimumIntegralDigits);
        if (bufferSize <= 0) {
            return {u"(number formatter error)"};
        }
        auto* buffer = new char16_t[bufferSize];
        Memory::ZeroMemory(buffer, bufferSize);
        Utils::CStrings::DataToHexString(static_cast<uint64_t>(value), buffer, bufferSize, FormattingRule.UpperCase,
                                         FormattingRule.UseAlternateForm, FormattingRule.MinimumIntegralDigits);
        StringBase<char16_t> result(buffer); // -1
        delete[] buffer;
        return result;
    }
    if constexpr (std::is_integral_v<T>) {
        const int32_t bufferLength =
            Utils::CStrings::NumberToString(static_cast<int64_t>(value), nullptr, 0, FormattingRule);
        if (bufferLength <= 0) {
            return {u"(number formatter error)"};
        }
        auto* buffer = new char16_t[bufferLength];
        Utils::CStrings::NumberToString(static_cast<int64_t>(value), buffer, bufferLength, FormattingRule);
        StringBase<char16_t> result(buffer);
        delete[] buffer;
        return result;
    } else if constexpr (std::is_floating_point_v<T>) {
        const int32_t bufferLength =
            Utils::CStrings::FloatToString(static_cast<double>(value), nullptr, 0, FormattingRule);
        if (bufferLength <= 0) {
            return {u"(number formatter error)"};
        }
        auto* buffer = new char16_t[bufferLength];
        Utils::CStrings::FloatToString(static_cast<double>(value), buffer, bufferLength, FormattingRule);
        StringBase<char16_t> result(buffer, bufferLength);
        delete[] buffer;
        return result;
    }

    return {};
}
template <typename CharT, typename... ArgsT> struct __Internal_String_Formatter {
    const CharT* formatString;
    int32_t formatStringCursor = 0;

    StringBase<CharT> outBuffer;
    int32_t outBufferCursor = 0;

    bool isParsingFormatPlaceholderInsides = false;
    static constexpr int32_t MaxFormatInnerLength = 255;
    CharT formatInner[MaxFormatInnerLength + 1];
    int32_t formatInnerCursor = 0;

    bool isIndexedPlaceholder = false;
    int32_t lastUsedIndexedPlaceholderIndex = 0;

    Utils::Function<StringBase<CharT>(const StringBase<CharT>&)> argumentFormatters[sizeof...(ArgsT)];

    template <uint32_t I>
    static Utils::Function<StringBase<CharT>(const StringBase<CharT>&)>
    MakeArgumentFormatter(const Containers::Tuple<std::decay_t<ArgsT>...>& arguments) {
        using ArgT = Containers::TupleElementT<I, std::decay_t<ArgsT>...>;
        const ArgT& argument = arguments.template Get<I>();
        return Utils::Function<StringBase<CharT>(const StringBase<CharT>&)>(
            [argument](const StringBase<CharT>& opts) -> StringBase<CharT> {
                if constexpr (Utils::HasOptionTakingToStringMethod<ArgT>) {
                    return argument.ToString(opts);
                } else if constexpr (Utils::HasNoArgumentToStringMethod<ArgT>) {
                    return argument.ToString();
                } else {
                    return StringFormatter<Containers::TupleElementT<I, std::decay_t<ArgsT>...>, CharT>::Format(
                        argument, opts);
                }
            });
    }

    template <uint32_t... Is>
    void SetArgumentFormatters(const Containers::Tuple<ArgsT...>& arguments, Utils::IndexSequence<Is...>) {
        ((argumentFormatters[Is] = MakeArgumentFormatter<Is>(arguments)), ...);
    }

    void AccumulateCharacter(CharT inCharacter) {
        if (isParsingFormatPlaceholderInsides && formatInnerCursor < MaxFormatInnerLength) {
            formatInner[formatInnerCursor++] = inCharacter;
            return;
        }
        outBuffer += inCharacter;
    }

    void PrintFormatToBuffer() {
#if _DEBUG
        if (formatInnerCursor != 0) {
            Platform::GetPlatform().OnFatalError(
                u"String::Format Internal Error: formatInnerCursor should be 0 at the start of PrintFormatToBuffer.");
            return;
        }
#endif
        const int32_t innerSize = Utils::CStrings::Length(formatInner);
        int32_t argumentIndex = lastUsedIndexedPlaceholderIndex++;
        // skip whitespace
        while (formatInnerCursor < innerSize && Utils::CStrings::IsWhitespace(formatInner[formatInnerCursor])) {
            formatInnerCursor++;
        }
        auto isDigit = [](CharT c) -> bool {
            if (c >= static_cast<CharT>('0') && c <= static_cast<CharT>('9')) {
                return true;
            }
            return false;
        };
        bool hasIndex = false;
        if (formatInnerCursor < innerSize && isDigit(formatInner[formatInnerCursor])) {
            argumentIndex = 0;
            while (formatInnerCursor < innerSize && isDigit(formatInner[formatInnerCursor])) {
                argumentIndex = argumentIndex * 10 + (formatInner[formatInnerCursor] - static_cast<CharT>('0'));
                formatInnerCursor++;
            }
            isIndexedPlaceholder = true;
            hasIndex = true;
        }
        if (isIndexedPlaceholder && !hasIndex) {
            Platform::GetPlatform().OnFatalError(
                u"String::Format Argument Error: mixing indexed placeholders are not allowed.");
            return;
        }

        bool hasFormatOptions = false;
        CharT formatOptions[MaxFormatInnerLength + 1] = {0};
        int32_t formatOptionsCursor = 0;
        // skip whitespace
        while (formatInnerCursor < innerSize && Utils::CStrings::IsWhitespace(formatInner[formatInnerCursor])) {
            formatInnerCursor++;
        }
        if (formatInnerCursor < innerSize && formatInner[formatInnerCursor] == static_cast<CharT>(':')) {
            formatInnerCursor++;
            // skip whitespace
            while (formatInnerCursor < innerSize && Utils::CStrings::IsWhitespace(formatInner[formatInnerCursor])) {
                formatInnerCursor++;
            }
            // check if any other character exist
            if (formatInnerCursor < innerSize) {
                hasFormatOptions = true;
                while (formatInnerCursor < innerSize) {
                    formatOptions[formatOptionsCursor++] = formatInner[formatInnerCursor++];
                }
            }
        }

        if (argumentIndex < 0 || argumentIndex >= sizeof...(ArgsT)) {
            Platform::GetPlatform().OnFatalError(u"String::Format Argument Error: argument index out of range.");
            return;
        }
        StringBase<CharT> formattedString = argumentFormatters[argumentIndex](
            hasFormatOptions ? StringBase<CharT>(formatOptions, formatOptionsCursor) : StringBase<CharT>());
        const bool oldP = isParsingFormatPlaceholderInsides;
        isParsingFormatPlaceholderInsides = false; // Disable parsing insides while printing
        for (int i = 0; i < formattedString.Length(); i++) {
            AccumulateCharacter(formattedString[i]);
        }
        isParsingFormatPlaceholderInsides = oldP;
        formatInnerCursor = 0;
    }
    template <typename... Us> StringBase<CharT> Format(const StringBase<CharT>& inFormatString, Us&&... args) {
        Containers::Tuple<ArgsT...> arguments = Containers::MakeTuple<ArgsT...>(args...);
        // Pre-allocate output buffer to double the format string length to reduce reallocations.
        outBuffer.EnsureCapacity(inFormatString.Length() * 2);
        SetArgumentFormatters(arguments, Utils::MakeIndexSequence<sizeof...(ArgsT)>());
        formatString = inFormatString.Data();
        for (; formatStringCursor < inFormatString.Length(); ++formatStringCursor) {
            switch (CharT currentChar = inFormatString[formatStringCursor]) {
            case static_cast<CharT>('{'): {
                if (inFormatString[formatStringCursor + 1] == static_cast<CharT>('{')) {
                    AccumulateCharacter(static_cast<CharT>('{'));
                    formatStringCursor++; // we handled two characters here. One will be incremented by the for loop.
                    continue;
                }
                isParsingFormatPlaceholderInsides = true;
                Memory::ZeroMemory(formatInner, MaxFormatInnerLength + 1);
                formatInnerCursor = 0;
                break;
            }
            case static_cast<CharT>('}'): {
                if (inFormatString[formatStringCursor + 1] == static_cast<CharT>('}')) {
                    AccumulateCharacter(static_cast<CharT>('}'));
                    formatStringCursor++; // we handled two characters here. One will be incremented by the for loop.
                    continue;
                }
                formatInnerCursor = 0;
                PrintFormatToBuffer();
                isParsingFormatPlaceholderInsides = false;
                break;
            }
            default: {
                AccumulateCharacter(currentChar);
                break;
            }
            }
        }
        return outBuffer;
    }
};
template <typename CharT>
template <typename... ArgsT>
StringBase<CharT> StringBase<CharT>::Format(const StringBase<CharT>& inFormatString, ArgsT&&... args) {
    return __Internal_String_Formatter<CharT, std::decay_t<ArgsT>...>().Format(inFormatString,
                                                                               std::forward<ArgsT>(args)...);
}
template <typename CharT>
List<StringBase<CharT>> StringBase<CharT>::Split(const CharT delimiter, const bool ignoreEmpty) const {
    List<StringBase<CharT>> result;
    int32_t startIndex = 0;
    while (startIndex < Length()) {
        int32_t delimiterIndex = IndexOf(delimiter, startIndex);

        if (delimiterIndex == -1) {
            if (StringBase<CharT> substring = SubString(startIndex, Length() - startIndex);
                !ignoreEmpty || !substring.IsEmpty()) {
                result.Add(substring);
            }
            break;
        } else {
            StringBase<CharT> substring = SubString(startIndex, delimiterIndex - startIndex);
            if (!ignoreEmpty || !substring.IsEmpty()) {
                result.Add(substring);
            }
            startIndex = delimiterIndex + 1;
        }
    }
    if (result.Length() == 0) {
        result.Add(*this); // Add the whole string if no delimiter found
    }
    return result;
}
template <typename CharT>
List<StringBase<CharT>> StringBase<CharT>::Split(const StringBase& delimiter, bool ignoreEmpty) const {
    List<StringBase<CharT>> result;
    int32_t startIndex = 0;
    while (startIndex < Length()) {
        int32_t delimiterIndex = IndexOf(delimiter.Data()[0], startIndex);
        auto foundDelimiter = SubString(delimiterIndex, delimiter.Length());
        if (foundDelimiter != delimiter) // Invalid match
            continue;
        if (delimiterIndex == -1) {
            if (StringBase<CharT> substring = SubString(startIndex, Length() - startIndex);
                !ignoreEmpty || !substring.IsEmpty()) {
                result.Add(substring);
            }
            break;
        } else {
            StringBase<CharT> substring = SubString(startIndex, delimiterIndex - startIndex);
            if (!ignoreEmpty || !substring.IsEmpty()) {
                result.Add(substring);
            }
            startIndex = delimiterIndex + delimiter.Length();
        }
    }
    if (result.Length() == 0) {
        result.Add(*this); // Add the whole string if no delimiter found
    }
    return result;
}
template <typename CharT>
StringBase<CharT> StringBase<CharT>::Replace(const StringBase& toFind, const StringBase& toReplaceWith,
                                             int32_t startIndex, int32_t* replaceEnd) const {
    StringBase<CharT> result;
    int32_t strIndex = IndexOf(toFind, startIndex);
    if (strIndex == -1) {
        if (replaceEnd)
            *replaceEnd = Length(); // go to the end of the string.
        return *this;
    }
    result += SubString(0, strIndex);
    result += toReplaceWith;
    const int32_t afterReplaceIndex = strIndex + toFind.Length();
    result += SubString(afterReplaceIndex, Length() - afterReplaceIndex);
    if (replaceEnd) {
        *replaceEnd = strIndex + toReplaceWith.Length();
    }
    return result;
}
template <typename CharT>
StringBase<CharT> StringBase<CharT>::ReplaceAll(const StringBase& toFind, const StringBase& toReplaceWith) const {
    const StringBase<CharT> result = *this;
    int32_t currentIndex = 0;
    do {
        result = result.Replace(toFind, toReplaceWith, currentIndex, &currentIndex);
        currentIndex++; // Move to the next part
    } while (currentIndex != result.Length()); // if we are at the end, stop.
    return result;
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::PadLeft(int32_t count) const {
    if (count > 0) {
        auto* buffer = new CharT[count + 1];
        for (int i = 0; i < count - Length(); i++) {
            buffer[i] = static_cast<CharT>(' ');
        }
        StringBase<CharT> result(buffer, count);
        delete[] buffer;
        result += *this;
        return result;
    } else if (count < 0) {
        // trim from left
        return SubString(-count, Length() + count);
    } else {
        return *this;
    }
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::PadRight(int32_t count) const {
    if (count > 0) {
        const CharT* buffer = new CharT[count + 1];
        for (int i = 0; i < count - Length(); i++) {
            buffer[i] = static_cast<CharT>(' ');
        }
        StringBase<CharT> result = *this;
        result += StringBase<CharT>(buffer, count);
        delete[] buffer;
        return result;
    } else if (count < 0) {
        // trim from right
        return SubString(0, Length() + count);
    } else {
        return *this;
    }
}

template <typename CharT>
template <typename FromT>
void StringBase<CharT>::SetDataFromRawString(const FromT* raw_str, const int32_t length) {

    Buffer.Resize(length + 1);
    Buffer.Get(length) = 0;
    if constexpr (std::is_same_v<FromT, CharT>) {
        Memory::CopyMemory(Buffer.Data(), raw_str, length);
    } else {
        auto* convertedBuffer = new CharT[length + 1];
        Utils::CStrings::ConvertString(raw_str, convertedBuffer, length + 1);
        Memory::CopyMemory(Buffer.Data(), convertedBuffer, (length + 1));
        delete[] convertedBuffer;
    }
}
template <typename CharT> StringBase<CharT> StringBase<CharT>::Upper(const I18N::Locale& locale) const {
    const int32_t size = Utils::CStrings::ToLower(Data(), nullptr, 0, locale);
    auto* buffer = new char16_t[size + 1];
    Utils::CStrings::ToUpper(Data(), buffer, size + 1, locale);
    StringBase result(buffer, size);
    delete[] buffer;
    return result;
}

} // namespace Edvar::Containers