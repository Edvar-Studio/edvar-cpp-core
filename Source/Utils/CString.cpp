#include "unicode/utypes.h"          // IWYU pragma: keep
#include "unicode/ustring.h"         // IWYU pragma: keep
#include "unicode/locid.h"           // IWYU pragma: keep
#include "unicode/uchar.h"           // IWYU pragma: keep
#include "unicode/dcfmtsym.h"        // IWYU pragma: keep
#include "unicode/numberformatter.h" // IWYU pragma: keep

#include "Utils/CString.hpp" // IWYU pragma: keep
#include "I18N/Locale.hpp"   // IWYU pragma: keep

#include <cstdio>  // IWYU pragma: keep
#include <cstdlib> // IWYU pragma: keep
#include <cstdarg> // IWYU pragma: keep
#include <cmath>   // IWYU pragma: keep

namespace Edvar::Utils::CStrings {
int32_t Length(const char16_t* buffer) { return u_strlen(buffer); }
int32_t Length(const wchar_t* buffer) {
    int32_t length = 0;
    while (*buffer) {
        ++length;
        ++buffer;
    }
    return length;
}
int32_t Length(const char* buffer) {
    int32_t length = 0;
    while (*buffer) {
        ++length;
        ++buffer;
    }
    return length;
}

void vsnprintf_WriteToBuffer(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex,
                             const char16_t charToWrite) {
    // Always advance the written-character count so callers can know how many
    // characters would have been produced (like snprintf semantics). Only
    // write into the buffer when there is space for a character plus the
    // null-terminator.
    if (buffer == nullptr || bufferLength == 0) {
        ++currentIndex;
        return;
    }
    if (currentIndex < static_cast<int32_t>(bufferLength) - 1) {
        buffer[currentIndex] = charToWrite;
    }
    ++currentIndex;
}
int32_t ToCharString(const wchar_t* inString, char* buffer, const int32_t bufferLength) {
    const int32_t utf16Size = ToUtf16String(inString, nullptr, bufferLength);
    auto* utf16Buffer = new char16_t[utf16Size];
    ToUtf16String(inString, utf16Buffer, utf16Size);
    const int32_t resultSize = ToCharString(utf16Buffer, buffer, bufferLength);
    delete[] utf16Buffer;
    return resultSize;
}
int32_t ToCharString(const char8_t* inString, char* buffer, const int32_t bufferLength) {
    if (auto* bufferUtf8 = reinterpret_cast<char8_t*>(buffer); bufferUtf8 != inString) {
        Memory::CopyMemory(bufferUtf8, inString, bufferLength);
        return bufferLength;
    }
    return bufferLength;
}
int32_t ToCharString(const char16_t* inString, char* buffer, const int32_t bufferLength) {
    int32_t writeSize = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8(buffer, bufferLength, &writeSize, inString, -1, &errorCode);
    return writeSize;
}
int32_t ToCharString(const char32_t* inString, char* buffer, const int32_t bufferLength) {
    const int32_t requiredBufferLength = ToUtf16String(inString, nullptr, 0);
    const auto* bufferUtf16 = new wchar_t[requiredBufferLength];
    const int32_t resultStrLength = ToCharString(bufferUtf16, buffer, bufferLength);
    delete[] bufferUtf16;
    return resultStrLength;
}
int32_t ToWCharString(const char* inString, wchar_t* buffer, const int32_t bufferLength) {
    return static_cast<int32_t>(mbstowcs(buffer, inString, bufferLength));
}
int32_t ToWCharString(const char8_t* inString, wchar_t* buffer, const int32_t bufferLength) {
    return ToWCharString(reinterpret_cast<const char*>(inString), buffer, bufferLength);
}
int32_t ToWCharString(const char16_t* inString, wchar_t* buffer, const int32_t bufferLength) {
    int32_t resultLength = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToWCS(buffer, bufferLength, &resultLength, inString, -1, &errorCode);
    return resultLength;
}
int32_t ToWCharString(const char32_t* inString, wchar_t* buffer, const int32_t bufferLength) {
    const int32_t utf16Size = ToUtf16String(inString, nullptr, 0);
    auto* utf16Buffer = new char16_t[utf16Size];
    ToUtf16String(inString, utf16Buffer, utf16Size);
    const int32_t resultSize = ToWCharString(utf16Buffer, buffer, bufferLength);
    delete[] utf16Buffer;
    return resultSize;
}
int32_t ToUtf8String(const char* inString, char8_t* buffer, const int32_t bufferLength) {
    if (const auto* utf8InString = reinterpret_cast<const char8_t*>(inString); utf8InString != buffer) {
        Memory::CopyMemory(buffer, utf8InString, bufferLength);
    }
    return bufferLength;
}
int32_t ToUtf8String(const wchar_t* inString, char8_t* buffer, const int32_t bufferLength) {
    return ToCharString(inString, reinterpret_cast<char*>(buffer), bufferLength);
}
int32_t ToUtf8String(const char16_t* inString, char8_t* buffer, const int32_t bufferLength) {
    return ToCharString(inString, reinterpret_cast<char*>(buffer), bufferLength);
}
int32_t ToUtf8String(const char32_t* inString, char8_t* buffer, const int32_t bufferLength) {
    return ToCharString(inString, reinterpret_cast<char*>(buffer), bufferLength);
}
int32_t ToUtf16String(const char* inString, char16_t* buffer, const int32_t bufferLength) {
    int32_t resultSize = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF8(buffer, bufferLength, &resultSize, inString, -1, &errorCode);
    return resultSize;
}
int32_t ToUtf16String(const wchar_t* inString, char16_t* buffer, const int32_t bufferLength) {
    int32_t resultSize = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromWCS(buffer, bufferLength, &resultSize, inString, -1, &errorCode);
    return resultSize;
}
int32_t ToUtf16String(const char8_t* inString, char16_t* buffer, const int32_t bufferLength) {
    return ToUtf16String(reinterpret_cast<const char*>(inString), buffer, bufferLength);
}
int32_t ToUtf16String(const char32_t* inString, char16_t* buffer, const int32_t bufferLength) {
    int32_t resultSize = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32(buffer, bufferLength, &resultSize, reinterpret_cast<const UChar32*>(inString), -1, &errorCode);
    return resultSize;
}
int32_t ToUtf32String(const char* inString, char32_t* buffer, const int32_t bufferLength) {
    const int32_t utf16Length = ToUtf16String(inString, nullptr, 0);
    auto* utf16Buffer = new char16_t[utf16Length];
    ToUtf16String(inString, utf16Buffer, utf16Length);
    const int32_t resultSize = ToUtf32String(utf16Buffer, buffer, bufferLength);
    delete[] utf16Buffer;
    return resultSize;
}
int32_t ToUtf32String(const wchar_t* inString, char32_t* buffer, const int32_t bufferLength) {
    const int32_t utf16Length = ToUtf16String(inString, nullptr, 0);
    auto* utf16Buffer = new char16_t[utf16Length];
    ToUtf16String(inString, utf16Buffer, utf16Length);
    const int32_t resultSize = ToUtf32String(utf16Buffer, buffer, bufferLength);
    delete[] utf16Buffer;
    return resultSize;
}
int32_t ToUtf32String(const char8_t* inString, char32_t* buffer, const int32_t bufferLength) {
    return ToUtf32String(reinterpret_cast<const char*>(inString), buffer, bufferLength);
}
int32_t ToUtf32String(const char16_t* inString, char32_t* buffer, const int32_t bufferLength) {
    int32_t resultSize = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF32(reinterpret_cast<UChar32*>(buffer), bufferLength, &resultSize, inString, -1, &errorCode);
    return resultSize;
}

int32_t ToLower(char16_t* inString, char16_t* outString, int32_t bufferLength, const I18N::Locale* inLocale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    const I18N::Locale& locale = inLocale ? *inLocale : I18N::Locale::Default();
    return u_strToLower(outString, bufferLength, inString, -1,
                        static_cast<const icu::Locale*>(locale.GetData())->getName(), &errorCode);
}

int32_t ToUpper(char16_t* inString, char16_t* outString, int32_t bufferLength, const I18N::Locale* inLocale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    const I18N::Locale& locale = inLocale ? *inLocale : I18N::Locale::Default();
    return u_strToUpper(outString, bufferLength, inString, -1,
                        static_cast<const icu::Locale*>(locale.GetData())->getName(), &errorCode);
}

bool IsWhitespace(char c) { return u_isWhitespace(static_cast<UChar32>(c)); }
bool IsWhitespace(char8_t c) { return u_isWhitespace(static_cast<UChar32>(c)); }
bool IsWhitespace(wchar_t c) { return u_isWhitespace(static_cast<UChar32>(c)); }
bool IsWhitespace(char16_t c) { return u_isWhitespace(static_cast<UChar32>(c)); }
bool IsWhitespace(char32_t c) { return u_isWhitespace(static_cast<UChar32>(c)); }
icu::number::LocalizedNumberFormatter getNumberFormatter(NumberFormattingRule formattingRule) {
    if (formattingRule.Locale == nullptr) {
        formattingRule.Locale = &I18N::Locale::Invariant();
    }
    UNumberFormatRoundingMode roundingMode;
    switch (formattingRule.RoundingMode) {
    default:
    case NumberRoundingMode::HalfToEven:
        roundingMode = UNUM_ROUND_HALFEVEN;
        break;
    case NumberRoundingMode::HalfFromZero:
        roundingMode = UNUM_ROUND_HALFUP;
        break;
    case NumberRoundingMode::HalfToZero:
        roundingMode = UNUM_ROUND_HALFDOWN;
        break;
    case NumberRoundingMode::FromZero:
        roundingMode = UNUM_ROUND_UP;
        break;
    case NumberRoundingMode::ToZero:
        roundingMode = UNUM_ROUND_DOWN;
        break;
    case NumberRoundingMode::ToNegativeInfinity:
        roundingMode = UNUM_ROUND_FLOOR;
        break;
    case NumberRoundingMode::ToPositiveInfinity:
        roundingMode = UNUM_ROUND_CEILING;
        break;
    }
    return icu::number::NumberFormatter::withLocale(*static_cast<const icu::Locale*>(formattingRule.Locale->GetData()))
        .notation(formattingRule.UseScientificNotation ? icu::number::Notation::scientific()
                                                       : icu::number::Notation::simple())
        .precision(icu::number::Precision::minMaxFraction(formattingRule.MinimumFractionalDigits,
                                                          formattingRule.MaximumFractionalDigits))
        .grouping(formattingRule.UseGrouping ? UNUM_GROUPING_AUTO : UNUM_GROUPING_OFF)
        .sign(formattingRule.AlwaysSign ? UNUM_SIGN_EXCEPT_ZERO : UNUM_SIGN_NEGATIVE)
        .integerWidth(icu::number::IntegerWidth::zeroFillTo(formattingRule.MinimumIntegralDigits)
                          .truncateAt(formattingRule.MaximumIntegralDigits))
        .roundingMode(roundingMode);
}

int32_t NumberToString(const int64_t value, char16_t* buffer, const int32_t bufferLength,
                       const NumberFormattingRule& formattingRule) {
    UErrorCode errorCode = U_ZERO_ERROR;
    const auto result = getNumberFormatter(formattingRule).formatInt(value, errorCode);
    if (U_FAILURE(errorCode)) {
        return 0;
    }
    auto str = result.toString(errorCode);
    if (bufferLength > 0 && buffer != nullptr) {
        // Check if buffer is large enough
        if (bufferLength >= str.length() + 1) {
            // Copy the string to the buffer
            u_strcpy(buffer, str.getTerminatedBuffer());
        }
    }
    return str.length() + 1;
}

int32_t FloatToString(const double value, char16_t* buffer, const int32_t bufferLength,
                      const NumberFormattingRule& formattingRule) {
    getNumberFormatter(formattingRule);
    UErrorCode errorCode = U_ZERO_ERROR;
    const auto result = getNumberFormatter(formattingRule).formatDouble(value, errorCode);
    if (U_FAILURE(errorCode)) {
        return 0;
    }
    auto str = result.toString(errorCode);
    if (bufferLength > 0 && buffer != nullptr) {
        // Check if buffer is large enough
        if (bufferLength >= str.length() + 1) {
            // Copy the string to the buffer
            u_strcpy(buffer, str.getTerminatedBuffer());
        }
    }
    return str.length() + 1;
}
int32_t DataToHexString(const uint64_t value, char16_t* buffer, const int32_t bufferLength, const bool upperCase,
                        const bool prefix0x, const int32_t minWidth, const char16_t paddingChar) {
    // parse the data into 4-bits and store each of them as bytes.
    char16_t hexDigits[16];
    for (int32_t i = 0; i < 16; ++i) {
        if (const auto asByte = static_cast<uint8_t>((value >> ((15 - i) * 4)) & 0x0F); asByte < 10) {
            hexDigits[i] = u'0' + asByte;
        } else {
            hexDigits[i] = (upperCase ? u'A' : u'a') + (asByte - 10);
        }
    }
    int32_t startIndex = 0;
    // Skip leading zeros
    while (startIndex < 15 && hexDigits[startIndex] == u'0') {
        ++startIndex;
    }
    int32_t hexLength = 16 - startIndex;
    if (hexLength < minWidth) {
        hexLength = minWidth;
    }
    const int32_t totalLength = hexLength + (prefix0x ? 2 : 0);
    if (buffer == nullptr) {
        return totalLength + 1; // +1 for null terminator
    }
    if (bufferLength < totalLength + 1) {
        return 0; // Buffer too small
    }
    int32_t writePos = 0;
    // Write prefix if needed
    if (prefix0x) {
        buffer[writePos++] = u'0';
        buffer[writePos++] = upperCase ? u'X' : u'x';
    }
    // Write padding characters
    for (int32_t i = hexLength - (16 - startIndex); i > 0; --i) {
        buffer[writePos++] = paddingChar;
    }
    // Write hex digits
    for (int32_t i = startIndex; i < 16; ++i) {
        buffer[writePos++] = hexDigits[i];
    }
    // Null terminate
    buffer[writePos] = u'\0';
    return writePos;
}
uint64_t StringToUInt64(const char16_t* str, const int32_t base) {
    if (base < 2 || base > 36) {
        return 0; // Invalid base
    }

    uint64_t result = 0;
    int32_t index = 0;

    // Skip leading whitespace
    while (IsWhitespace(str[index])) {
        ++index;
    }

    // Skip 0x or 0X for bigger-than-10-base if present
    // This can't exist in bases that include '0' and 'x' as valid digits
    if (base > 10 && base <= 33 && str[index] == u'0' && (str[index + 1] == u'x' || str[index + 1] == u'X')) {
        index += 2;
    }

    // Process each character
    while (str[index] != u'\0') {
        char16_t ch = str[index];
        int32_t digitValue = -1;

        if (ch >= u'0' && ch <= u'9') {
            digitValue = ch - u'0';
        } else if (ch >= u'A' && ch <= u'Z') {
            digitValue = ch - u'A' + 10;
        } else if (ch >= u'a' && ch <= u'z') {
            digitValue = ch - u'a' + 10;
        } else {
            break; // Invalid character
        }

        if (digitValue < 0 || digitValue >= base) {
            break; // Invalid digit for the base
        }

        result = result * base + static_cast<uint64_t>(digitValue);
        ++index;
    }

    return result;
}
int64_t StringToInt64(const char16_t* str, int32_t base) {
    if (base < 2 || base > 36) {
        return 0; // Invalid base
    }

    int64_t result = 0;
    int32_t index = 0;
    bool isNegative = false;

    // Skip leading whitespace
    while (IsWhitespace(str[index])) {
        ++index;
    }

    // Check for sign
    if (str[index] == u'-') {
        isNegative = true;
        ++index;
    } else if (str[index] == u'+') {
        ++index;
    }

    // Process each character
    while (str[index] != u'\0') {
        char16_t ch = str[index];
        int32_t digitValue = -1;

        if (ch >= u'0' && ch <= u'9') {
            digitValue = ch - u'0';
        } else if (ch >= u'A' && ch <= u'Z') {
            digitValue = ch - u'A' + 10;
        } else if (ch >= u'a' && ch <= u'z') {
            digitValue = ch - u'a' + 10;
        } else {
            break; // Invalid character
        }

        if (digitValue < 0 || digitValue >= base) {
            break; // Invalid digit for the base
        }

        result = result * base + static_cast<int64_t>(digitValue);
        ++index;
    }

    return isNegative ? -result : result;
}
} // namespace Edvar::Utils::CStrings