#include "unicode/utypes.h"   // IWYU pragma: keep
#include "unicode/ustring.h"  // IWYU pragma: keep
#include "unicode/locid.h"    // IWYU pragma: keep
#include "unicode/uchar.h"    // IWYU pragma: keep
#include "unicode/dcfmtsym.h" // IWYU pragma: keep

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

void vsnprintf_WriteToBuffer(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex,
                             const char16_t charToWrite) {
    if (buffer == nullptr || bufferLength <= 0) {
        currentIndex++;
        return;
    }
    if (currentIndex < static_cast<int32_t>(bufferLength) - 1) {
        buffer[currentIndex++] = charToWrite;
    }
}
struct vsnprintf_ArgState {
    struct {
        uint8_t leftJustify : 1 = false;
        uint8_t forceSign : 1 = false;
        uint8_t spaceForSign : 1 = false;
        uint8_t alternateForm : 1 = false;
        uint8_t leftPadWithZeros : 1 = false;
    } flags;

    int argNumber = -1; // -1 means next argument in order
    void parseArgumentNumber(const char16_t*& stream) { _parseArgumentNumberHelper(stream); }
    bool _parseArgumentNumberHelper(const char16_t*& stream) {
        const char16_t* originalPos = stream;
        if (*stream >= u'0' && *stream <= u'9') {
            if (argNumber == -1) {
                argNumber = 0;
            }
            argNumber = argNumber * 10 + (*stream - u'0');
            stream++;
            if (!_parseArgumentNumberHelper(stream)) {
                stream = originalPos;
            }
            return true;
        } else if (*stream == u'$') {
            stream++;
            return false;
        } else {
            // No argument number specified, reset to -1
            argNumber = -1;
            stream = originalPos;
            return false;
        }
    }

    void parseFlags(const char16_t*& stream) {
        bool flagFound = true;
        switch (*stream) {
        case u'-':
            flags.leftJustify = true;
            break;
        case u'+':
            flags.forceSign = true;
            break;
        case u' ':
            flags.spaceForSign = true;
            break;
        case u'#':
            flags.alternateForm = true;
            break;
        case u'0':
            flags.leftPadWithZeros = true;
            break;
        default:
            flagFound = false;
        }
        if (flagFound) {
            stream++;
            parseFlags(stream);
        }
    }

    struct {
        int32_t width = -1;
        uint8_t widthAsNextArgs : 1 = false;
    } width;
    void parseWidth(const char16_t*& stream) {
        // check to see if it is '*'
        if (*stream == u'*') {
            width.widthAsNextArgs = true;
            stream++;
            return;
        } else if (*stream >= u'0' && *stream <= u'9') {
            if (width.width == -1) {
                width.width = 0;
            }
            width.width = width.width * 10 + (*stream - u'0');
            stream++;
            parseWidth(stream);
        }
    }

    struct {
        int32_t number = -1;
        uint8_t precisionAsNextArgs : 1 = false;
    } precision;
    void parsePrecision(const char16_t*& stream) {
        if (*stream == u'.') {
            stream++;
            // check to see if it is '*'
            if (*stream == u'*') {
                precision.precisionAsNextArgs = true;
                stream++;
                return;
            } else if (*stream >= u'0' && *stream <= u'9') {
                if (precision.number == -1) {
                    precision.number = 0;
                }
                precision.number = precision.number * 10 + (*stream - u'0');
                stream++;
                parsePrecision(stream);
            } else {
                precision.number = 0; // Precision specified as . without number means 0
            }
        }
    }

    enum class LengthModifier : uint8_t { None, hh, h, l, ll, j, z, t, L } lengthModifier = LengthModifier::None;

    void parseLengthModifier(const char16_t*& stream) {
        switch (*stream) {
        case u'h':
            stream++;
            if (*stream == u'h') {
                lengthModifier = LengthModifier::hh;
                stream++;
            } else {
                lengthModifier = LengthModifier::h;
            }
            break;
        case u'l':
            stream++;
            if (*stream == u'l') {
                lengthModifier = LengthModifier::ll;
                stream++;
            } else {
                lengthModifier = LengthModifier::l;
            }
            break;
        case u'j':
            lengthModifier = LengthModifier::j;
            stream++;
            break;
        case u'z':
            lengthModifier = LengthModifier::z;
            stream++;
            break;
        case u't':
            lengthModifier = LengthModifier::t;
            stream++;
            break;
        case u'L':
            lengthModifier = LengthModifier::L;
            stream++;
            break;
        default:
            break;
        }
    }

    enum class Specifier : uint8_t {
        Unknown,
        d,
        i = d,
        u,
        o,
        x,
        X,
        f,
        F,
        e,
        E,
        g,
        G,
        a,
        A,
        c,
        s,
        p,
        n
    } specifier = Specifier::Unknown;

    void parseSpecifier(const char16_t*& stream) {
        switch (*stream) {
        case u'd':
            specifier = Specifier::d;
            break;
        case u'i':
            specifier = Specifier::i;
            break;
        case u'u':
            specifier = Specifier::u;
            break;
        case u'o':
            specifier = Specifier::o;
            break;
        case u'x':
            specifier = Specifier::x;
            break;
        case u'X':
            specifier = Specifier::X;
            break;
        case u'f':
            specifier = Specifier::f;
            break;
        case u'F':
            specifier = Specifier::F;
            break;
        case u'e':
            specifier = Specifier::e;
            break;
        case u'E':
            specifier = Specifier::E;
            break;
        case u'g':
            specifier = Specifier::g;
            break;
        case u'G':
            specifier = Specifier::G;
            break;
        case u'a':
            specifier = Specifier::a;
            break;
        case u'A':
            specifier = Specifier::A;
            break;
        case u'c':
            specifier = Specifier::c;
            break;
        case u's':
            specifier = Specifier::s;
            break;
        case u'p':
            specifier = Specifier::p;
            break;
        case u'n':
            specifier = Specifier::n;
            break;
        default:
            specifier = Specifier::Unknown;
            return;
        }
        stream++;
    }

    void Parse(const char16_t*& stream) {
        parseArgumentNumber(stream);
        parseFlags(stream);
        parseWidth(stream);
        parsePrecision(stream);
        parseLengthModifier(stream);
        parseSpecifier(stream);
    }

    int32_t getWidth(va_list args) const {
        if (width.widthAsNextArgs) {
            return va_arg(args, int32_t);
        }
        return width.width;
    }

    int32_t getPrecision(va_list args) const {
        if (precision.precisionAsNextArgs) {
            return va_arg(args, int32_t);
        }
        return precision.number;
    }

    void Write(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex, va_list args) const {
        if (specifier == Specifier::Unknown) {
            // Invalid specifier or percent symbol, write as is
            vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u'%');
            return;
        }
        const int32_t w = getWidth(args);
        const int32_t p = getPrecision(args);
        switch (specifier) {
        case Specifier::d: {
            writeDecimalInteger(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case Specifier::u: {
            writeUnsignedDecimalInteger(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case Specifier::o: {
            writeNonDecimalInteger<8>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case Specifier::X:
        case Specifier::x: {
            writeNonDecimalInteger<16>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case Specifier::f:
        case Specifier::F:
        case Specifier::e:
        case Specifier::E:
        case Specifier::g:
        case Specifier::G: {
            switch (lengthModifier) {
            case LengthModifier::L:
                writeFloatingPoint<double, long double>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            default:
                writeFloatingPoint<double, double>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            }
            break;
        }
        case Specifier::c: {
            switch (lengthModifier) {
            default:
                writeCharacter<char16_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            case LengthModifier::hh:
                writeCharacter<char8_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            case LengthModifier::h:
                writeCharacter<char16_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            case LengthModifier::l:
                writeCharacter<char32_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            }
            break;
        }
        case Specifier::s: {
            switch (lengthModifier) {
            case LengthModifier::hh:
                writeString<char8_t, char8_t*>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            default:
            case LengthModifier::h:
                writeString<char16_t, char16_t*>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            case LengthModifier::l:
                writeString<char32_t, char32_t*>(buffer, bufferLength, currentIndex, args, w, p);
                break;
            }
            break;
        }
        case Specifier::p: {
            writePointer(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case Specifier::n: {
            storeWrittenCount(args, currentIndex);
            break;
        }
        default: {
            // Not supported yet.
            Platform::Get().OnFatalError(u"vsnprintf: Unsupported format specifier encountered.");
            break;
        }
        }
    }

    void storeWrittenCount(va_list args, int32_t currentIndex) const {
        switch (lengthModifier) {
        case LengthModifier::hh: {
            auto* ptr = va_arg(args, signed char*);
            *ptr = static_cast<signed char>(currentIndex);
            break;
        }
        case LengthModifier::h: {
            auto* ptr = va_arg(args, short*);
            *ptr = static_cast<short>(currentIndex);
            break;
        }
        case LengthModifier::l: {
            auto* ptr = va_arg(args, long*);
            *ptr = static_cast<long>(currentIndex);
            break;
        }
        case LengthModifier::ll: {
            auto* ptr = va_arg(args, long long*);
            *ptr = static_cast<long long>(currentIndex);
            break;
        }
        case LengthModifier::j: {
            auto* ptr = va_arg(args, intmax_t*);
            *ptr = static_cast<intmax_t>(currentIndex);
            break;
        }
        case LengthModifier::z: {
            auto* ptr = va_arg(args, size_t*);
            *ptr = static_cast<size_t>(currentIndex);
            break;
        }
        case LengthModifier::t: {
            auto* ptr = va_arg(args, ptrdiff_t*);
            *ptr = static_cast<ptrdiff_t>(currentIndex);
            break;
        }
        default:
        case LengthModifier::None: {
            auto* ptr = va_arg(args, int*);
            *ptr = static_cast<int>(currentIndex);
            break;
        }
        }
    }

    void writePointer(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex, va_list args, int32_t w,
                      int32_t p) const {
        const auto value = va_arg(args, void*);
        NumberFormattingRule rule;
        rule.AlwaysSign = false; // always sign then delete the sign if not needed and modify the array.

        rule.Locale = nullptr;
        rule.MinimumFractionalDigits = p == -1 ? 0 : p;
        rule.UseGrouping = false;
        const int32_t size = NumberToString(reinterpret_cast<uint64_t>(value), nullptr, 0, 10, rule);
        auto* tempBuffer = new char16_t[size];
        NumberToString(reinterpret_cast<uint64_t>(value), tempBuffer, size, 10, rule);
        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u'b');
        if (w < size - 1) {
            w = -1; // no truncation is allowed
        }
        if (w == -1) {
            // no padding applies when the width is -1
            for (int i = 0; i < size - 1; ++i) {
                vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
            }
        } else {
            if (w != 0) {
                const int32_t paddingNeeded = w - (size - 1);

                if (!flags.leftJustify) {
                    // pad on the left
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex,
                                                flags.leftPadWithZeros ? u'0' : u' ');
                    }
                }
                // write the actual number
                for (int i = 0; i < size - 1; ++i) {
                    vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
                }
                if (flags.leftJustify) {
                    // pad on the right
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u' ');
                    }
                }
            }
        }
        delete[] tempBuffer;
    }

    template <typename T, typename ReadT>
    void writeCharacter(char16_t* buffer, const uint32_t bufferLength, int32_t& currentIndex, va_list args, int32_t w,
                        const int32_t p) const {
        const auto value = static_cast<T>(va_arg(args, ReadT));
        T buff[2];
        buff[0] = value;
        buff[1] = 0;
        auto* tempBuffer = CreateConvertedString<char16_t, T>(buff);

        const int32_t size = Length(tempBuffer) + 1;
        if (w < size - 1) {
            w = -1; // no truncation is allowed
        }
        if (w == -1) {
            // no padding applies when the width is -1
            for (int i = 0; i < size - 1; ++i) {
                vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
            }
        } else {
            if (w != 0) {
                const int32_t paddingNeeded = w - (size - 1);

                if (!flags.leftJustify) {
                    // pad on the left
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex,
                                                flags.leftPadWithZeros ? u'0' : u' ');
                    }
                }
                // write the actual number
                for (int i = 0; i < size - 1; ++i) {
                    vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
                }
                if (flags.leftJustify) {
                    // pad on the right
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u' ');
                    }
                }
            }
        }
        delete[] tempBuffer;
    }

    template <typename T, typename ReadT>
    void writeString(char16_t* buffer, const uint32_t bufferLength, int32_t& currentIndex, va_list args, int32_t w,
                     const int32_t p) const {
        const auto value = static_cast<T*>(va_arg(args, ReadT));
        auto* tempBuffer = CreateConvertedString<char16_t, T>(value);

        int32_t size = Length(tempBuffer) + 1;
        size = Math::Min(size, p == -1 ? size : p + 1);
        if (w < size - 1) {
            w = -1; // no truncation is allowed
        }
        if (w == -1) {
            // no padding applies when the width is -1
            for (int i = 0; i < size - 1; ++i) {
                vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
            }
        } else {
            if (w != 0) {
                const int32_t paddingNeeded = w - (size - 1);

                if (!flags.leftJustify) {
                    // pad on the left
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex,
                                                flags.leftPadWithZeros ? u'0' : u' ');
                    }
                }
                // write the actual number
                for (int i = 0; i < size - 1; ++i) {
                    vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
                }
                if (flags.leftJustify) {
                    // pad on the right
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u' ');
                    }
                }
            }
        }
        delete[] tempBuffer;
    }

    template <typename T, typename ReadT>
    void writeDecimalIntegerType(char16_t* buffer, const uint32_t bufferLength, int32_t& currentIndex, va_list args,
                                 int32_t w, const int32_t p) const {
        const auto value = static_cast<T>(va_arg(args, ReadT));
        if (value == 0 && p == 0) {
            return;
        }
        NumberFormattingRule rule;
        rule.AlwaysSign = true; // always sign then delete the sign if not needed and modify the array.
        rule.Locale = nullptr;
        rule.MinimumFractionalDigits = p == -1 ? 0 : p;
        rule.UseGrouping = false;
        using ToStringType = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
        int32_t size = NumberToString(static_cast<ToStringType>(value), nullptr, 0, 10, rule);
        auto* tempBuffer = new char16_t[size];
        NumberToString(static_cast<ToStringType>(value), tempBuffer, size, 10, rule);
        if (w < size - 1) {
            w = -1; // no truncation is allowed
        }
        if (w == -1) {
            // no padding applies when the width is -1
            for (int i = 0; i < size - 1; ++i) {
                vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
            }
        } else {
            if (w != 0) {
                const int32_t paddingNeeded = w - (size - 1);
                char16_t sign = tempBuffer[0];
                const bool signRequired = sign == u'-' || (sign == u'+' && (flags.forceSign || flags.spaceForSign));
                if (sign == u'+' && flags.spaceForSign) {
                    sign = u' ';
                }
                if (tempBuffer[0] == u'+') {
                    // remove the sign by shifting all characters to the left by one
                    for (int j = 1; j < size - 1; ++j) {
                        tempBuffer[j - 1] = tempBuffer[j];
                    }
                    --size;
                }
                if (!flags.leftJustify) {
                    if (flags.leftPadWithZeros && signRequired) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, sign);
                        // write sign first if required
                    }
                    // pad on the left
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex,
                                                flags.leftPadWithZeros ? u'0' : u' ');
                    }
                }
                // write the actual number
                for (int i = 0; i < size - 1; ++i) {
                    if (i == 0) {
                        if (!flags.leftPadWithZeros && signRequired) {
                            vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, sign);
                        }
                    }
                    vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
                }
                if (flags.leftJustify) {
                    // pad on the right
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u' ');
                    }
                }
            }
        }
        delete[] tempBuffer;
    }

    template <typename T, typename ReadT, int Base = 16>
    void writeNonDecimalUnsignedIntegerType(char16_t* buffer, const uint32_t bufferLength, int32_t& currentIndex,
                                            va_list args, int32_t w, const int32_t p) const {
        const auto value = static_cast<T>(va_arg(args, ReadT));
        if (value == 0 && p == 0) {
            return;
        }
        NumberFormattingRule rule;
        rule.AlwaysSign = false;
        rule.Locale = nullptr;
        rule.MinimumFractionalDigits = 0;
        rule.UseGrouping = false;
        rule.UpperCase = specifier == Specifier::X;
        using ToStringType = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
        int32_t size = NumberToString(static_cast<ToStringType>(value), nullptr, 0, Base, rule);
        constexpr int alternateFormExtraSize = (Base > 10) ? 2 : 1;
        if (flags.alternateForm) {
            size += alternateFormExtraSize; // for 0x or 0X or 0
        }
        auto* tempBuffer = new char16_t[size];
        NumberToString(static_cast<ToStringType>(value), tempBuffer, size, 10, rule);
        if (flags.alternateForm) {
            // shift the string to the right by alternateFormExtraSize
            for (int i = 0; i < size - alternateFormExtraSize - 1; ++i) {
                tempBuffer[i + alternateFormExtraSize] = tempBuffer[i];
            }
            tempBuffer[0] = u'0';
            if constexpr (Base > 10) {
                tempBuffer[1] = specifier == Specifier::X ? u'X' : u'x';
            }
        }
        if (w < size - 1) {
            w = -1; // no truncation is allowed
        }
        if (w == -1) {
            // no padding applies when the width is -1
            for (int i = 0; i < size - 1; ++i) {
                vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
            }
        } else {
            if (w != 0) {
                const int32_t paddingNeeded = w - (size - 1);
                if (!flags.leftJustify) {
                    // pad on the left
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex,
                                                flags.leftPadWithZeros ? u'0' : u' ');
                    }
                }
                // write the actual number
                for (int i = 0; i < size - 1; ++i) {
                    vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
                }
                if (flags.leftJustify) {
                    // pad on the right
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u' ');
                    }
                }
            }
        }
        delete[] tempBuffer;
    }

    void writeDecimalInteger(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex, va_list args, int32_t w,
                             int32_t p) const {
        switch (lengthModifier) {
        case LengthModifier::hh: {
            writeDecimalIntegerType<int8_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::h: {
            writeDecimalIntegerType<int16_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        default: // default to int32_t
        case LengthModifier::l: {
            writeDecimalIntegerType<int32_t, int32_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::ll: {
            writeDecimalIntegerType<int64_t, int64_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::j: {
            writeDecimalIntegerType<intmax_t, intmax_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::z: {
            writeDecimalIntegerType<size_t, size_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::t: {
            writeDecimalIntegerType<ptrdiff_t, ptrdiff_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        }
    }

    void writeUnsignedDecimalInteger(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex, va_list args,
                                     int32_t w, int32_t p) const {
        switch (lengthModifier) {
        case LengthModifier::hh: {
            writeDecimalIntegerType<uint8_t, uint32_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::h: {
            writeDecimalIntegerType<uint16_t, uint32_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        default: // default to int32_t
        case LengthModifier::l: {
            writeDecimalIntegerType<uint32_t, uint32_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::ll: {
            writeDecimalIntegerType<uint64_t, uint64_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::j: {
            writeDecimalIntegerType<uintmax_t, uintmax_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::z: {
            writeDecimalIntegerType<size_t, size_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::t: {
            writeDecimalIntegerType<ptrdiff_t, ptrdiff_t>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        }
    }
    template <int Base>
    void writeNonDecimalInteger(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex, va_list args, int32_t w,
                                int32_t p) const {
        switch (lengthModifier) {
        case LengthModifier::hh: {
            writeNonDecimalUnsignedIntegerType<uint8_t, uint32_t, Base>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::h: {
            writeNonDecimalUnsignedIntegerType<uint16_t, uint32_t, Base>(buffer, bufferLength, currentIndex, args, w,
                                                                         p);
            break;
        }
        default: // default to int32_t
        case LengthModifier::l: {
            writeNonDecimalUnsignedIntegerType<uint32_t, uint32_t, Base>(buffer, bufferLength, currentIndex, args, w,
                                                                         p);
            break;
        }
        case LengthModifier::ll: {
            writeNonDecimalUnsignedIntegerType<uint64_t, uint64_t, Base>(buffer, bufferLength, currentIndex, args, w,
                                                                         p);
            break;
        }
        case LengthModifier::j: {
            writeNonDecimalUnsignedIntegerType<uintmax_t, uintmax_t, Base>(buffer, bufferLength, currentIndex, args, w,
                                                                           p);
            break;
        }
        case LengthModifier::z: {
            writeNonDecimalUnsignedIntegerType<size_t, size_t, Base>(buffer, bufferLength, currentIndex, args, w, p);
            break;
        }
        case LengthModifier::t: {
            writeNonDecimalUnsignedIntegerType<ptrdiff_t, ptrdiff_t, Base>(buffer, bufferLength, currentIndex, args, w,
                                                                           p);
            break;
        }
        }
    }
    template <typename T, typename ReadT>
    void writeFloatingPoint(char16_t* buffer, uint32_t bufferLength, int32_t& currentIndex, va_list args, int32_t w,
                            int32_t p) const {
        const auto value = static_cast<T>(va_arg(args, ReadT));
        NumberFormattingRule rule;
        rule.AlwaysSign = true; // always sign then delete the sign if not needed and modify the array.
        rule.Locale = nullptr;
        rule.MinimumFractionalDigits = p == -1 ? 6 : p;
        rule.UseGrouping = false;
        rule.ScientificNotationThreshold = specifier == Specifier::e || specifier == Specifier::E ? 0 : 10;
        bool writtenDecimalPoint = false;
        int32_t size = FloatToString(static_cast<double>(value), nullptr, 0, rule, &writtenDecimalPoint);
        auto* tempBuffer = new char16_t[size];
        FloatToString(static_cast<double>(value), tempBuffer, size, rule, &writtenDecimalPoint);
        if (specifier == Specifier::G || specifier == Specifier::g) {
            const int32_t prevSize = size;
            rule.ScientificNotationThreshold = 0;
            size = FloatToString(static_cast<double>(value), nullptr, 0, rule, &writtenDecimalPoint);
            if (size > prevSize) {
                // restore previous.
                rule.ScientificNotationThreshold = 10;
                size = prevSize;
            }
        }
        if (w < size - 1) {
            w = -1; // no truncation is allowed
        }
        if (w == -1) {
            // no padding applies when the width is -1
            for (int i = 0; i < size - 1; ++i) {
                vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
            }
        } else {
            if (w != 0) {
                const int32_t paddingNeeded = w - (size - 1);
                char16_t sign = tempBuffer[0];
                const bool signRequired = sign == u'-' || (sign == u'+' && (flags.forceSign || flags.spaceForSign));
                if (sign == u'+' && flags.spaceForSign) {
                    sign = u' ';
                }
                // remove the sign by shifting all characters to the left by one
                for (int j = 1; j < size - 1; ++j) {
                    tempBuffer[j - 1] = tempBuffer[j];
                }
                --size;
                if (!flags.leftJustify) {
                    if (flags.leftPadWithZeros && signRequired) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, sign);
                        // write sign first if required
                    }
                    // pad on the left
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex,
                                                flags.leftPadWithZeros ? u'0' : u' ');
                    }
                }
                for (int i = 0; i < size - 1; ++i) {
                    if (i == 0) {
                        if (!flags.leftPadWithZeros && signRequired) {
                            vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, sign);
                        }
                    }
                    vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, tempBuffer[i]);
                    if (rule.MinimumFractionalDigits == 0 && flags.alternateForm) {
                        // write a decimal point if alternate form is specified and no fractional digits
                        if (tempBuffer[i] == u'0') {
                            vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u'.');
                        }
                    }
                }
                if (flags.leftJustify) {
                    // pad on the right
                    for (int i = 0; i < paddingNeeded; ++i) {
                        vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, u' ');
                    }
                }
            }
        }
        delete[] tempBuffer;
    }
};
int32_t VSNPrintF(char16_t* buffer, uint32_t bufferLength, const char16_t* format, va_list args) {
    if (!buffer) {
        return 0;
    }
    const char16_t* cur = format;
    int currentIndex = 0;
    while (cur != nullptr && *cur != 0) {
        if (*cur != u'%') {
            vsnprintf_WriteToBuffer(buffer, bufferLength, currentIndex, *cur);
            while (cur != nullptr && *cur != 0 && *cur != u'%') {
                ++cur;
            }
            continue;
        }
        // skip the first %
        ++cur;
        vsnprintf_ArgState argState;
        argState.Parse(format);
        argState.Write(buffer, bufferLength, currentIndex, args);
    }
    return currentIndex;
}

int32_t SPrintF(char16_t* buffer, uint32_t bufferLength, const char16_t* format, ...) {
    va_list ap;
    va_start(ap, format);
    const int32_t result = VSNPrintF(buffer, bufferLength, format, ap);
    va_end(ap);
    return result;
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
void PrintF(const char16_t* format, ...) {
    if (!format) {
        return;
    }
    const int32_t formatAsWCharLength = ToWCharString(format, nullptr, 0);
    if (formatAsWCharLength <= 0) {
        return;
    }
    auto* formatAsWChar = new wchar_t[formatAsWCharLength];
    ToWCharString(format, formatAsWChar, formatAsWCharLength);

    va_list arg;
    va_start(arg, format);
    vwprintf(formatAsWChar, arg);
    va_end(arg);
    delete[] formatAsWChar;
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
    // Apply minimum fractional digits. Just pad 0s at the end starting with decimal point.
    if (formattingRule.MinimumFractionalDigits > 0) {
        tempBuffer[digitCount++] = u'.';
        for (int32_t i = 0; i < formattingRule.MinimumFractionalDigits; ++i) {
            tempBuffer[digitCount++] = u'0';
        }
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

    if (base <= 10) {
        // Write sign
        if (isNegative) {
            buffer[writePos++] = u'-';
        } else if (formattingRule.AlwaysSign) {
            buffer[writePos++] = u'+';
        }
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

    // Convert the absolute value to string in the given base
    char16_t tempBuffer[256]; // Temporary buffer for conversion
    int32_t digitCount = 0;
    uint64_t temp = value;

    // Extract digits in reverse order
    do {
        const uint64_t digit = temp % base;
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

    // Apply minimum fractional digits. Just pad 0s at the end starting with decimal point.
    if (formattingRule.MinimumFractionalDigits > 0) {
        tempBuffer[digitCount++] = u'.';
        for (int32_t i = 0; i < formattingRule.MinimumFractionalDigits; ++i) {
            tempBuffer[digitCount++] = u'0';
        }
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

int32_t FloatToString(double value, char16_t* buffer, int32_t bufferLength, NumberFormattingRule formattingRule,
                      bool* writtenDecimalPoint) {
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

    // Determine if we should use scientific notation
    bool useScientific = false;
    int32_t exponent = 0;

    if (absValue > 0) {
        exponent = static_cast<int32_t>(std::floor(std::log10(absValue)));
        useScientific = (absValue >= std::pow(10.0, formattingRule.ScientificNotationThreshold) ||
                         absValue < std::pow(10.0, -formattingRule.ScientificNotationThreshold)) ||
                        formattingRule.ScientificNotationThreshold == 0;
    }

    // Get locale-specific symbols
    Containers::StringBase<char16_t> decimalSeparator = u".";
    Containers::StringBase<char16_t> groupingSeparator = u",";

    UErrorCode errorCode = U_ZERO_ERROR;
    const I18N::Locale& locale = formattingRule.Locale ? *formattingRule.Locale : I18N::Locale::Default();
    icu::DecimalFormatSymbols dfs(*static_cast<const icu::Locale*>(locale.GetData()), errorCode);
    if (U_SUCCESS(errorCode)) {
        decimalSeparator = dfs.getSymbol(icu::DecimalFormatSymbols::kDecimalSeparatorSymbol).getTerminatedBuffer();
        groupingSeparator = dfs.getSymbol(icu::DecimalFormatSymbols::kGroupingSeparatorSymbol).getTerminatedBuffer();
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
    auto integralPart = static_cast<int64_t>(scaledValue);
    double fractionalPart = scaledValue - static_cast<double>(integralPart);

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
        for (int32_t i = tempPos - integralDigits - (isNegative || formattingRule.AlwaysSign ? 1 : 0); i < zerosToAdd;
             ++i) {
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
    // add fractional digits if the abs(fractionalPart) > epsilon or minimum fractional digits > 0
    const double epsilon = 0.5 / std::pow(10.0, formattingRule.MaximumFractionalDigits);
    if (Math::Abs(fractionalPart) > epsilon || formattingRule.MinimumFractionalDigits > 0) {
        for (int32_t dsIdx = 0; dsIdx < decimalSeparator.Length(); ++dsIdx) {
            tempBuffer[tempPos++] = decimalSeparator[dsIdx];
        }

        // Round the fractional part based on rounding mode
        double roundFactor = std::pow(10.0, formattingRule.MaximumFractionalDigits);
        double roundedFractional = fractionalPart * roundFactor;

        // Apply rounding mode
        int64_t fractionalInt = 0;
        switch (formattingRule.RoundingMode) {
        case NumberRoundingMode::HalfToEven: {
            double rounded = std::round(roundedFractional);
            if (rounded == std::floor(roundedFractional) + 0.5) {
                // Banker's rounding: round to nearest even
                auto lower = static_cast<int64_t>(std::floor(roundedFractional));
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
        int32_t fractionalPos = 0;
        int32_t fracLen = 0;
        int32_t zeroes = 0;
        for (int32_t i = 0; i < formattingRule.MaximumFractionalDigits; ++i) {
            char16_t written =
                u'0' + ((fractionalInt /
                         static_cast<int64_t>(std::pow(10.0, formattingRule.MaximumFractionalDigits - i - 1))) %
                        10);
            fractionalBuffer[fractionalPos++] = written;
            if (written == u'0') {
                zeroes++;
            } else {
                fracLen += zeroes;
                fracLen++;
                zeroes = 0;
            }
        }

        // Apply minimum fractional digits
        int32_t digitsToWrite =
            Math::Clamp(fracLen, formattingRule.MinimumFractionalDigits, formattingRule.MaximumFractionalDigits);
        if (writtenDecimalPoint != nullptr) {
            (*writtenDecimalPoint) = digitsToWrite > 0;
        }
        for (int32_t i = 0; i < digitsToWrite; ++i) {
            tempBuffer[tempPos++] = i < fracLen ? fractionalBuffer[i] : u'0';
        }
    } else {
        if (writtenDecimalPoint != nullptr) {
            *writtenDecimalPoint = false;
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