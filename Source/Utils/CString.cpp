#include "unicode/utypes.h"   // IWYU pragma: keep
#include "unicode/ustring.h"  // IWYU pragma: keep
#include "unicode/locid.h"    // IWYU pragma: keep
#include "unicode/uchar.h"    // IWYU pragma: keep
#include "unicode/dcfmtsym.h" // IWYU pragma: keep
#include <cstdio>             // IWYU pragma: keep
#include <cstdlib>            // IWYU pragma: keep
#include <cstdarg>            // IWYU pragma: keep
#include <cmath>              // IWYU pragma: keep

#include "Utils/CString.hpp" // IWYU pragma: keep
#include "I18N/Locale.hpp"   // IWYU pragma: keep

namespace Edvar::Utils::CStrings {
int32_t Length(const char16_t* buffer) { return u_strlen(buffer); }
int32_t Length(const wchar_t* buffer) {
    int32_t length = 0;
    while (*buffer) {
        ++length;
    }
    return length;
}
int32_t Length(const char* buffer) {
    int32_t length = 0;
    while (*buffer) {
        ++length;
    }
    return length;
}

int32_t SPrintF(char16_t* buffer, uint32_t bufferLength, const char16_t* format, ...) {
    if (!format) {
        return 0;
    }

    const int32_t formatAsWCharLength = ToWCharString(format, nullptr, 0);
    if (formatAsWCharLength <= 0) {
        return 0;
    }
    auto* formatAsWChar = new wchar_t[formatAsWCharLength];
    ToWCharString(format, formatAsWChar, formatAsWCharLength);

    va_list arg;
    va_start(arg, format);
    const int32_t resultSize = vswprintf(nullptr, 0, formatAsWChar, arg);
    if (resultSize <= 0) {
        va_end(arg);
        delete[] formatAsWChar;
        return resultSize;
    }
    auto* resultBuffer = new wchar_t[resultSize];
    vswprintf(resultBuffer, resultSize, formatAsWChar, arg);
    va_end(arg);
    delete[] formatAsWChar;

    const int32_t returnSize = ToUtf16String(resultBuffer, buffer, bufferLength);
    delete[] formatAsWChar;
    return returnSize;
}
char16_t* CreatePrintFString(const char16_t* format, ...) {
    va_list arg;
    va_start(arg, format);
    const int32_t length = SPrintF(nullptr, 0, format, arg);
    auto* returnBuffer = new char16_t[length];
    SPrintF(returnBuffer, length, format, arg);
    va_end(arg);
    return returnBuffer;
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

int32_t NumberToString(int64_t value, char16_t* buffer, int32_t bufferLength, int32_t base,
                       NumberFormattingRule formattingRule) {
    if (base < 2 || base > 36) {
        return 0; // Invalid base
    }

    const bool isNegative = value < 0;
    const uint64_t absValue = isNegative ? static_cast<uint64_t>(-value) : static_cast<uint64_t>(value);

    // Convert the absolute value to string in the given base
    char16_t tempBuffer[256]; // Temporary buffer for conversion
    int32_t digitCount = 0;
    uint64_t temp = absValue;

    // Extract digits in reverse order
    do {
        const int32_t digit = temp % base;
        const char16_t digitChar = digit < 10 ? u'0' + digit : (formattingRule.UpperCase ? u'A' : u'a') + (digit - 10);
        tempBuffer[digitCount++] = digitChar;
        temp /= base;
    } while (temp > 0 && digitCount < 256);

    // Apply minimum integral digits
    while (digitCount < formattingRule.MinimumIntegralDigits && digitCount < 256) {
        tempBuffer[digitCount++] = u'0';
    }

    // Apply maximum integral digits
    if (digitCount > formattingRule.MaximumIntegralDigits) {
        digitCount = formattingRule.MaximumIntegralDigits;
    }

    // Calculate output size including sign and grouping separators
    int32_t outputSize = 0;

    // Add sign
    if (isNegative || formattingRule.AlwaysSign) {
        outputSize++; // For + or -
    }
    // Get grouping separator from locale
    Containers::StringBase<char16_t> groupingSeparator = u",";
    if (base == 10 && formattingRule.UseGrouping) {
        UErrorCode errorCode = U_ZERO_ERROR;
        const I18N::Locale& locale = formattingRule.Locale ? *formattingRule.Locale : I18N::Locale::Default();
        icu::DecimalFormatSymbols dfs(*static_cast<const icu::Locale*>(locale.GetData()), errorCode);
        if (U_SUCCESS(errorCode)) {
            groupingSeparator =
                dfs.getSymbol(icu::DecimalFormatSymbols::kGroupingSeparatorSymbol).getTerminatedBuffer();
        }
    }
    // Add digits and grouping separators
    outputSize += digitCount;
    if (formattingRule.UseGrouping && base == 10) {
        // Add grouping separators (every 3 digits for decimal)
        const int32_t groupingCount = (digitCount - 1) / 3;
        outputSize += groupingCount * groupingSeparator.Length();
    }

    // If buffer is null, return required size
    if (buffer == nullptr) {
        return outputSize + 1; // +1 for null terminator
    }

    // Check if buffer is large enough
    if (bufferLength < outputSize + 1) {
        return 0; // Buffer too small
    }

    // Build the output string
    int32_t writePos = 0;

    // Write sign
    if (isNegative) {
        buffer[writePos++] = u'-';
    } else if (formattingRule.AlwaysSign) {
        buffer[writePos++] = u'+';
    }

    // Write digits (reverse order from tempBuffer) with grouping
    for (int32_t i = digitCount - 1; i >= 0; --i) {
        buffer[writePos++] = tempBuffer[i];

        // Add grouping separator if needed (every 3 digits from right, but not at the end)
        if (formattingRule.UseGrouping && base == 10 && i > 0 && i % 3 == 0) {
            for (int32_t gsIdx = 0; gsIdx < groupingSeparator.Length(); ++gsIdx) {
                buffer[writePos++] = groupingSeparator[gsIdx];
            }
        }
    }

    // Null terminate
    buffer[writePos] = u'\0';

    return writePos;
}

int32_t NumberToString(uint64_t value, char16_t* buffer, int32_t bufferLength, int32_t base,
                       NumberFormattingRule formattingRule) {
    if (base < 2 || base > 36) {
        return 0; // Invalid base
    }

    const bool isNegative = value < 0;
    const uint64_t absValue = isNegative ? static_cast<uint64_t>(-value) : static_cast<uint64_t>(value);

    // Convert the absolute value to string in the given base
    char16_t tempBuffer[256]; // Temporary buffer for conversion
    int32_t digitCount = 0;
    uint64_t temp = absValue;

    // Extract digits in reverse order
    do {
        const int32_t digit = temp % base;
        const char16_t digitChar = digit < 10 ? u'0' + digit : (formattingRule.UpperCase ? u'A' : u'a') + (digit - 10);
        tempBuffer[digitCount++] = digitChar;
        temp /= base;
    } while (temp > 0 && digitCount < 256);

    // Apply minimum integral digits
    while (digitCount < formattingRule.MinimumIntegralDigits && digitCount < 256) {
        tempBuffer[digitCount++] = u'0';
    }

    // Apply maximum integral digits
    if (digitCount > formattingRule.MaximumIntegralDigits) {
        digitCount = formattingRule.MaximumIntegralDigits;
    }

    // Calculate output size including sign and grouping separators
    int32_t outputSize = 0;

    // Add sign
    if (formattingRule.AlwaysSign) {
        outputSize++; // For +
    }
    // Get grouping separator from locale
    Containers::StringBase<char16_t> groupingSeparator = u",";
    if (base == 10 && formattingRule.UseGrouping) {
        UErrorCode errorCode = U_ZERO_ERROR;
        const I18N::Locale& locale = formattingRule.Locale ? *formattingRule.Locale : I18N::Locale::Default();
        icu::DecimalFormatSymbols dfs(*static_cast<const icu::Locale*>(locale.GetData()), errorCode);
        if (U_SUCCESS(errorCode)) {
            groupingSeparator =
                dfs.getSymbol(icu::DecimalFormatSymbols::kGroupingSeparatorSymbol).getTerminatedBuffer();
        }
    }
    // Add digits and grouping separators
    outputSize += digitCount;
    if (formattingRule.UseGrouping && base == 10) {
        // Add grouping separators (every 3 digits for decimal)
        const int32_t groupingCount = (digitCount - 1) / 3;
        outputSize += groupingCount * groupingSeparator.Length();
    }

    // If buffer is null, return required size
    if (buffer == nullptr) {
        return outputSize + 1; // +1 for null terminator
    }

    // Check if buffer is large enough
    if (bufferLength < outputSize + 1) {
        return 0; // Buffer too small
    }

    // Build the output string
    int32_t writePos = 0;

    // Write sign
    if (formattingRule.AlwaysSign) {
        buffer[writePos++] = u'+';
    }

    // Write digits (reverse order from tempBuffer) with grouping
    for (int32_t i = digitCount - 1; i >= 0; --i) {
        buffer[writePos++] = tempBuffer[i];

        // Add grouping separator if needed (every 3 digits from right, but not at the end)
        if (formattingRule.UseGrouping && base == 10 && i > 0 && i % 3 == 0) {
            for (int32_t gsIdx = 0; gsIdx < groupingSeparator.Length(); ++gsIdx) {
                buffer[writePos++] = groupingSeparator[gsIdx];
            }
        }
    }

    // Null terminate
    buffer[writePos] = u'\0';

    return writePos;
}

int32_t NumberToString(double value, char16_t* buffer, int32_t bufferLength, int32_t precision,
                       NumberFormattingRule formattingRule) {
    // Handle special values
    if (std::isnan(value)) {
        const char16_t* nanStr = u"NaN";
        const int32_t nanLen = 3;
        if (buffer == nullptr) {
            return nanLen + 1;
        }
        if (bufferLength < nanLen + 1) {
            return 0;
        }
        u_strcpy(buffer, nanStr);
        return nanLen;
    }

    if (std::isinf(value)) {
        const char16_t* infStr = value > 0 ? u"Infinity" : u"-Infinity";
        const int32_t infLen = value > 0 ? 8 : 9;
        if (buffer == nullptr) {
            return infLen + 1;
        }
        if (bufferLength < infLen + 1) {
            return 0;
        }
        u_strcpy(buffer, infStr);
        return infLen;
    }

    const bool isNegative = value < 0;
    const double absValue = isNegative ? -value : value;

    // Clamp precision to reasonable range
    if (precision < 0) {
        precision = 15;
    }
    if (precision > 100) {
        precision = 100;
    }

    // Determine if we should use scientific notation
    bool useScientific = false;
    int32_t exponent = 0;

    if (absValue > 0) {
        exponent = static_cast<int32_t>(std::floor(std::log10(absValue)));
        useScientific = (absValue >= std::pow(10.0, formattingRule.ScientificNotationThreshold) ||
                         absValue < std::pow(10.0, -formattingRule.ScientificNotationThreshold));
    }

    // Get locale-specific symbols
    Containers::StringBase<char16_t> decimalSeparator = u".";
    Containers::StringBase<char16_t> groupingSeparator = u",";

    UErrorCode errorCode = U_ZERO_ERROR;
    const I18N::Locale& locale = formattingRule.Locale ? *formattingRule.Locale : I18N::Locale::Default();
    icu::DecimalFormatSymbols dfs(*static_cast<const icu::Locale*>(locale.GetData()), errorCode);
    if (U_SUCCESS(errorCode)) {
        decimalSeparator = dfs.getSymbol(icu::DecimalFormatSymbols::kDecimalSeparatorSymbol).getTerminatedBuffer();
        groupingSeparator =
            dfs.getSymbol(icu::DecimalFormatSymbols::kGroupingSeparatorSymbol).getTerminatedBuffer();
    }

    // Format the number
    char16_t tempBuffer[512];
    int32_t tempPos = 0;

    // Add sign
    if (isNegative) {
        tempBuffer[tempPos++] = u'-';
    } else if (formattingRule.AlwaysSign) {
        tempBuffer[tempPos++] = u'+';
    }

    double scaledValue = absValue;
    int32_t integralDigits = 0;

    if (useScientific) {
        // Shift value to be between 1.0 and 10.0
        scaledValue = absValue / std::pow(10.0, exponent);
    }

    // Extract integral part
    int64_t integralPart = static_cast<int64_t>(scaledValue);
    double fractionalPart = scaledValue - integralPart;

    // Convert integral part to string
    if (integralPart == 0) {
        tempBuffer[tempPos++] = u'0';
        integralDigits = 1;
    } else {
        char16_t integralBuffer[64];
        int32_t integralPos = 0;
        int64_t temp = integralPart;
        while (temp > 0) {
            integralBuffer[integralPos++] = u'0' + (temp % 10);
            temp /= 10;
        }
        // Reverse and copy
        for (int32_t i = integralPos - 1; i >= 0; --i) {
            tempBuffer[tempPos++] = integralBuffer[i];
        }
        integralDigits = integralPos;
    }

    // Apply minimum integral digits
    if (integralDigits < formattingRule.MinimumIntegralDigits) {
        // Insert leading zeros
        int32_t zerosToAdd = formattingRule.MinimumIntegralDigits - integralDigits;
        for (int32_t i = tempPos - integralDigits - (isNegative || formattingRule.AlwaysSign ? 1 : 0);
             i < zerosToAdd; ++i) {
            // Shift existing content
            for (int32_t j = tempPos; j > (isNegative || formattingRule.AlwaysSign ? 1 : 0); --j) {
                tempBuffer[j] = tempBuffer[j - 1];
            }
            tempBuffer[(isNegative || formattingRule.AlwaysSign ? 1 : 0)] = u'0';
            tempPos++;
        }
        integralDigits = formattingRule.MinimumIntegralDigits;
    }

    // Add grouping separators for integral part
    if (formattingRule.UseGrouping && integralDigits > 3) {
        // We need to insert grouping separators
        // Build the integral part with separators
        int32_t startPos = (isNegative || formattingRule.AlwaysSign ? 1 : 0);
        int32_t endPos = tempPos;
        int32_t digitCount = endPos - startPos;

        // Rebuild with grouping separators
        char16_t withGrouping[512];
        int32_t groupPos = 0;

        // Copy sign if present
        for (int32_t i = 0; i < startPos; ++i) {
            withGrouping[groupPos++] = tempBuffer[i];
        }

        // Copy digits with grouping separators
        for (int32_t i = 0; i < digitCount; ++i) {
            if (i > 0 && (digitCount - i) % 3 == 0) {
                for (int32_t gsIdx = 0; gsIdx < groupingSeparator.Length(); ++gsIdx) {
                    withGrouping[groupPos++] = groupingSeparator[gsIdx];
                }
            }
            withGrouping[groupPos++] = tempBuffer[startPos + i];
        }

        tempPos = groupPos;
        // Copy back
        for (int32_t i = 0; i < tempPos; ++i) {
            tempBuffer[i] = withGrouping[i];
        }
    }

    // Add decimal separator and fractional digits
    int32_t fractionalDigits = formattingRule.MaximumFractionalDigits;
    if (fractionalDigits > 0 || formattingRule.MinimumFractionalDigits > 0) {
        for (int32_t dsIdx = 0; dsIdx < decimalSeparator.Length(); ++dsIdx) {
            tempBuffer[tempPos++] = decimalSeparator[dsIdx];
        }

        // Round the fractional part based on rounding mode
        double roundFactor = std::pow(10.0, fractionalDigits);
        double roundedFractional = fractionalPart * roundFactor;

        // Apply rounding mode
        int64_t fractionalInt = 0;
        switch (formattingRule.RoundingMode) {
            case NumberRoundingMode::HalfToEven: {
                double rounded = std::round(roundedFractional);
                if (rounded == std::floor(roundedFractional) + 0.5) {
                    // Banker's rounding: round to nearest even
                    int64_t lower = static_cast<int64_t>(std::floor(roundedFractional));
                    fractionalInt = (lower % 2 == 0) ? lower : lower + 1;
                } else {
                    fractionalInt = static_cast<int64_t>(std::round(roundedFractional));
                }
                break;
            }
            case NumberRoundingMode::HalfFromZero:
                fractionalInt = static_cast<int64_t>(std::floor(roundedFractional + 0.5));
                break;
            case NumberRoundingMode::HalfToZero:
                fractionalInt = static_cast<int64_t>(std::trunc(roundedFractional));
                break;
            case NumberRoundingMode::FromZero:
                fractionalInt = static_cast<int64_t>(std::ceil(std::abs(roundedFractional)));
                break;
            case NumberRoundingMode::ToZero:
                fractionalInt = static_cast<int64_t>(std::trunc(roundedFractional));
                break;
            case NumberRoundingMode::ToNegativeInfinity:
                fractionalInt = static_cast<int64_t>(std::floor(roundedFractional));
                break;
            case NumberRoundingMode::ToPositiveInfinity:
                fractionalInt = static_cast<int64_t>(std::ceil(roundedFractional));
                break;
        }

        // Convert fractional part to string with leading zeros
        char16_t fractionalBuffer[256];
        int32_t fracLen = 0;

        for (int32_t i = 0; i < fractionalDigits; ++i) {
            fractionalBuffer[fracLen++] = u'0' + ((fractionalInt / static_cast<int64_t>(std::pow(10.0, fractionalDigits - i - 1))) % 10);
        }

        // Apply minimum fractional digits
        int32_t digitsToWrite =
            std::max(formattingRule.MinimumFractionalDigits, formattingRule.MaximumFractionalDigits);
        for (int32_t i = 0; i < digitsToWrite; ++i) {
            tempBuffer[tempPos++] = i < fracLen ? fractionalBuffer[i] : u'0';
        }
    }

    // Add scientific notation exponent if needed
    if (useScientific) {
        tempBuffer[tempPos++] = u'e';
        if (exponent >= 0) {
            tempBuffer[tempPos++] = u'+';
        } else {
            tempBuffer[tempPos++] = u'-';
            exponent = -exponent;
        }
        // Add exponent digits
        if (exponent < 10) {
            tempBuffer[tempPos++] = u'0';
        }
        char16_t expBuffer[16];
        int32_t expLen = 0;
        int32_t temp = exponent;
        if (temp == 0) {
            expBuffer[expLen++] = u'0';
        } else {
            while (temp > 0) {
                expBuffer[expLen++] = u'0' + (temp % 10);
                temp /= 10;
            }
        }
        // Reverse and copy exponent
        for (int32_t i = expLen - 1; i >= 0; --i) {
            tempBuffer[tempPos++] = expBuffer[i];
        }
    }

    // Check if buffer is large enough
    if (buffer == nullptr) {
        return tempPos + 1; // +1 for null terminator
    }

    if (bufferLength < tempPos + 1) {
        return 0; // Buffer too small
    }

    // Copy to output buffer and null terminate
    for (int32_t i = 0; i < tempPos; ++i) {
        buffer[i] = tempBuffer[i];
    }
    buffer[tempPos] = u'\0';

    return tempPos;
}
} // namespace Edvar::Utils::CStrings