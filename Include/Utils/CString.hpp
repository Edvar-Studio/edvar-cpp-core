#pragma once
namespace Edvar::I18N {
class Locale;
}
namespace Edvar::Utils::CStrings {
EDVAR_CPP_CORE_API int32_t Length(const char16_t* buffer);
EDVAR_CPP_CORE_API int32_t Length(const wchar_t* buffer);
EDVAR_CPP_CORE_API int32_t Length(const char* buffer);

EDVAR_CPP_CORE_API int32_t SPrintF(char16_t* buffer, uint32_t bufferLength, const char16_t* format, ...);
EDVAR_CPP_CORE_API char16_t* CreatePrintFString(const char16_t* format, ...);
EDVAR_CPP_CORE_API int32_t FPrintF(const void* file, const char16_t* format, ...);

EDVAR_CPP_CORE_API int32_t ToCharString(const wchar_t* inString, char* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToCharString(const char8_t* inString, char* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToCharString(const char16_t* inString, char* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToCharString(const char32_t* inString, char* buffer, int32_t bufferLength);

EDVAR_CPP_CORE_API int32_t ToWCharString(const char* inString, wchar_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToWCharString(const char8_t* inString, wchar_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToWCharString(const char16_t* inString, wchar_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToWCharString(const char32_t* inString, wchar_t* buffer, int32_t bufferLength);

EDVAR_CPP_CORE_API int32_t ToUtf8String(const char* inString, char8_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf8String(const wchar_t* inString, char8_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf8String(const char16_t* inString, char8_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf8String(const char32_t* inString, char8_t* buffer, int32_t bufferLength);

EDVAR_CPP_CORE_API int32_t ToUtf16String(const char* inString, char16_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf16String(const wchar_t* inString, char16_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf16String(const char8_t* inString, char16_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf16String(const char32_t* inString, char16_t* buffer, int32_t bufferLength);

EDVAR_CPP_CORE_API int32_t ToUtf32String(const char* inString, char32_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf32String(const wchar_t* inString, char32_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf32String(const char8_t* inString, char32_t* buffer, int32_t bufferLength);
EDVAR_CPP_CORE_API int32_t ToUtf32String(const char16_t* inString, char32_t* buffer, int32_t bufferLength);

template <typename FromT, typename ToT>
int32_t ConvertString(const FromT* inString, ToT* buffer, const int32_t bufferLength) {
    if constexpr (std::is_same_v<ToT, char>) {
        return ToCharString(inString, buffer, bufferLength);
    } else if constexpr (std::is_same_v<ToT, wchar_t>) {
        return ToWCharString(inString, buffer, bufferLength);
    } else if constexpr (std::is_same_v<ToT, char8_t>) {
        return ToUtf8String(inString, buffer, bufferLength);
    } else if constexpr (std::is_same_v<ToT, char16_t>) {
        return ToUtf16String(inString, buffer, bufferLength);
    } else if constexpr (std::is_same_v<ToT, char32_t>) {
        return ToUtf32String(inString, buffer, bufferLength);
    }
    // Unsupported target type
    return -1;
}
/**
 * Create a new converted string
 *
 * It is the caller's responsibility to delete the returned buffer
 * Should use `delete[]` to delete the returned buffer.
 */
template <typename ToT, typename FromT> inline ToT* CreateConvertedString(const FromT* inString) {
    const int32_t bufferLength = ConvertString<FromT, ToT>(inString, nullptr, 0);
    if (bufferLength <= 0) {
        return nullptr;
    }
    ToT* buffer = new ToT[bufferLength];
    ConvertString<FromT, ToT>(inString, buffer, bufferLength);
    return buffer;
}

EDVAR_CPP_CORE_API int32_t ToLower(char16_t* inString, char16_t* outString, int32_t bufferLength,
                                   const I18N::Locale* inLocale = nullptr);
EDVAR_CPP_CORE_API int32_t ToUpper(char16_t* inString, char16_t* outString, int32_t bufferLength,
                                   const I18N::Locale* inLocale = nullptr);

EDVAR_CPP_CORE_API uint64_t StringToUInt64(const char16_t* str, int32_t base = 10);
EDVAR_CPP_CORE_API int64_t StringToInt64(const char16_t* str, int32_t base = 10);

EDVAR_CPP_CORE_API bool IsWhitespace(char c);
EDVAR_CPP_CORE_API bool IsWhitespace(char8_t c);
EDVAR_CPP_CORE_API bool IsWhitespace(wchar_t c);
EDVAR_CPP_CORE_API bool IsWhitespace(char16_t c);
EDVAR_CPP_CORE_API bool IsWhitespace(char32_t c);
enum class NumberRoundingMode : uint8_t {
    HalfToEven,
    HalfFromZero,
    HalfToZero,
    FromZero,
    ToZero,
    ToNegativeInfinity,
    ToPositiveInfinity
};

struct NumberFormattingRule {
    // Only for base 10 numbers
    bool UseGrouping = true;
    bool AlwaysSign = false;
    NumberRoundingMode RoundingMode = NumberRoundingMode::HalfToEven;
    int32_t MinimumIntegralDigits = 1;
    int32_t MaximumIntegralDigits = INT32_MAX;
    // Only for floating point numbers
    int32_t MinimumFractionalDigits = 0;
    // Only for floating point numbers
    int32_t MaximumFractionalDigits = 3;
    // Show hex numbers in upper case (A) if true, lower case (a) if false. Applies only for `base` > 10.
    bool UpperCase = false;
    /**
     * Only for floating point numbers
     * Use scientific notation if the value is >= 10^Threshold or < 10^-Threshold.
     */
    int ScientificNotationThreshold = 10;
    // Locale to use for formatting.
    const I18N::Locale* Locale;
};

EDVAR_CPP_CORE_API int32_t NumberToString(int64_t value, char16_t* buffer, int32_t bufferLength, int32_t base = 10,
                                          NumberFormattingRule formattingRule = NumberFormattingRule());
EDVAR_CPP_CORE_API int32_t NumberToString(uint64_t value, char16_t* buffer, int32_t bufferLength, int32_t base = 10,
                                          NumberFormattingRule formattingRule = NumberFormattingRule());
EDVAR_CPP_CORE_API int32_t NumberToString(double value, char16_t* buffer, int32_t bufferLength, int32_t precision = 15,
                                          NumberFormattingRule formattingRule = NumberFormattingRule());
} // namespace Edvar::Utils::CStrings
namespace Edvar {
using NumberRoundingMode = Edvar::Utils::CStrings::NumberRoundingMode;
using NumberFormattingRule = Edvar::Utils::CStrings::NumberFormattingRule;
} // namespace Edvar
