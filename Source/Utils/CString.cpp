#include "Utils/CString.hpp" // IWYU pragma: keep
#include "unicode/utypes.h"  // IWYU pragma: keep
#include "unicode/ustring.h" // IWYU pragma: keep

#include <cstdio>  // IWYU pragma: keep
#include <cstdlib> // IWYU pragma: keep
#include <cstdarg> // IWYU pragma: keep

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

} // namespace Edvar::Utils::CStrings