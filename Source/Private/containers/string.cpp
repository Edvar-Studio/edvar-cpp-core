#include "containers/string.h"
#include "internationalization/locale.h"
#include "unicode/ustring.h" // IWYU pragma: keep used for u_str functions.
#include "unicode/coll.h"
#include "unicode/numfmt.h"
#include "unicode/unistr.h"
#include "unicode/parsepos.h"
#include <cstdlib>

namespace edvar::c_string {
char_utf8* create_utf8(const char_utf16* in_string) {
    if (!in_string)
        return nullptr;
    int32_t utf8Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    // Calculate required buffer size
    u_strToUTF8(nullptr, 0, &utf8Length, reinterpret_cast<const UChar*>(in_string), -1, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING) {
        return nullptr;
    }
    status = U_ZERO_ERROR;

    // Allocate buffer (+1 for null terminator)
    char_utf8* out_string = new char_utf8[utf8Length + 1];

    // Convert UTF-16 to UTF-8
    u_strToUTF8(reinterpret_cast<char*>(out_string), utf8Length + 1, nullptr, reinterpret_cast<const UChar*>(in_string),
                -1, &status);
    if (U_FAILURE(status)) {
        delete[] out_string;
        return nullptr;
    }
    return out_string;
}
char_utf8* create_utf8(const char_utf32* in_string) {
    // create char_utf16 from char_utf32
    char_utf16* intermediate = create_utf16(in_string);
    if (!intermediate)
        return nullptr;
    char_utf8* out_string = create_utf8(intermediate);
    delete[] intermediate;
    return out_string;
}
char_utf16* create_utf16(const char_utf8* in_string) {
    if (!in_string)
        return nullptr;

    int32_t utf16Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    // Calculate required buffer size
    u_strFromUTF8(nullptr, 0, &utf16Length, reinterpret_cast<const char*>(in_string), -1, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING) {
        return nullptr;
    }
    status = U_ZERO_ERROR;

    // Allocate buffer (+1 for null terminator)
    char_utf16* out_string = new char_utf16[utf16Length + 1];

    // Convert UTF-8 to UTF-16
    u_strFromUTF8(reinterpret_cast<UChar*>(out_string), utf16Length + 1, nullptr,
                  reinterpret_cast<const char*>(in_string), -1, &status);
    if (U_FAILURE(status)) {
        delete[] out_string;
        return nullptr;
    }
    return out_string;
}
char_utf16* create_utf16(const char_utf32* in_string) {
    if (!in_string)
        return nullptr;

    int32_t utf16Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    // Calculate required buffer size
    u_strFromUTF32(nullptr, 0, &utf16Length, reinterpret_cast<const UChar32*>(in_string), -1, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING) {
        return nullptr;
    }
    status = U_ZERO_ERROR;

    // Allocate buffer (+1 for null terminator)
    char_utf16* out_string = new char_utf16[utf16Length + 1];

    // Convert UTF-32 to UTF-16
    u_strFromUTF32(reinterpret_cast<UChar*>(out_string), utf16Length + 1, nullptr,
                   reinterpret_cast<const UChar32*>(in_string), -1, &status);
    if (U_FAILURE(status)) {
        delete[] out_string;
        return nullptr;
    }
    return out_string;
}
char_utf32* create_utf32(const char_utf8* in_string) {
    // create char_utf16 from char_utf8
    char_utf16* intermediate = create_utf16(in_string);
    if (!intermediate)
        return nullptr;
    char_utf32* out_string = create_utf32(intermediate);
    delete[] intermediate;
    return out_string;
}
char_utf32* create_utf32(const char_utf16* in_string) {
    if (!in_string)
        return nullptr;

    int32_t utf32Length = 0;
    UErrorCode status = U_ZERO_ERROR;

    // Calculate required buffer size
    u_strToUTF32(nullptr, 0, &utf32Length, reinterpret_cast<const UChar*>(in_string), -1, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING) {
        return nullptr;
    }
    status = U_ZERO_ERROR;

    // Allocate buffer (+1 for null terminator)
    char_utf32* out_string = new char_utf32[utf32Length + 1];

    // Convert UTF-16 to UTF-32
    u_strToUTF32(reinterpret_cast<UChar32*>(out_string), utf32Length + 1, nullptr,
                 reinterpret_cast<const UChar*>(in_string), -1, &status);
    if (U_FAILURE(status)) {
        delete[] out_string;
        return nullptr;
    }
    return out_string;
}

char_utf8* create_lowercase(const char_utf8* in_string, edvar::internationalization::locale* locale) {
    char_utf16* intermediate = create_utf16(in_string);
    if (!intermediate)
        return nullptr;
    char_utf16* lowercased = create_lowercase(intermediate, locale);
    delete[] intermediate;
    if (!lowercased)
        return nullptr;
    char_utf8* out_string = create_utf8(lowercased);
    delete[] lowercased;
    return out_string;
}
char_utf16* create_lowercase(const char_utf16* in_string, edvar::internationalization::locale* locale) {
    if (!in_string)
        return nullptr;
    UErrorCode status = U_ZERO_ERROR;
    int32_t length = static_cast<int32_t>(edvar::c_string::string_length(in_string));

    // Calculate required buffer size
    int32_t lowerLength = u_strToLower(nullptr, 0, reinterpret_cast<const UChar*>(in_string), length,
                                       locale ? locale->name() : nullptr, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING) {
        return nullptr;
    }
    status = U_ZERO_ERROR;

    // Allocate buffer (+1 for null terminator)
    char_utf16* out_string = new char_utf16[lowerLength + 1];

    // Convert to lowercase
    u_strToLower(reinterpret_cast<UChar*>(out_string), lowerLength + 1, reinterpret_cast<const UChar*>(in_string),
                 length, locale ? locale->name() : nullptr, &status);
    if (U_FAILURE(status)) {
        delete[] out_string;
        return nullptr;
    }
    return out_string;
}
char_utf32* create_lowercase(const char_utf32* in_string, edvar::internationalization::locale* locale) {
    char_utf16* intermediate = create_utf16(in_string);
    if (!intermediate)
        return nullptr;
    char_utf16* lowercased = create_lowercase(intermediate, locale);
    delete[] intermediate;
    if (!lowercased)
        return nullptr;
    char_utf32* out_string = create_utf32(lowercased);
    delete[] lowercased;
    return out_string;
}

char_utf8* create_uppercase(const char_utf8* in_string, edvar::internationalization::locale* locale) {
    char_utf16* intermediate = create_utf16(in_string);
    if (!intermediate)
        return nullptr;
    char_utf16* uppercased = create_uppercase(intermediate, locale);
    delete[] intermediate;
    if (!uppercased)
        return nullptr;
    char_utf8* out_string = create_utf8(uppercased);
    delete[] uppercased;
    return out_string;
}
char_utf16* create_uppercase(const char_utf16* in_string, edvar::internationalization::locale* locale) {
    if (!in_string)
        return nullptr;
    UErrorCode status = U_ZERO_ERROR;
    int32_t length = static_cast<int32_t>(edvar::c_string::string_length(in_string));

    // Calculate required buffer size
    int32_t upperLength = u_strToUpper(nullptr, 0, reinterpret_cast<const UChar*>(in_string), length,
                                       locale ? locale->name() : nullptr, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING) {
        return nullptr;
    }
    status = U_ZERO_ERROR;

    // Allocate buffer (+1 for null terminator)
    char_utf16* out_string = new char_utf16[upperLength + 1];

    // Convert to uppercase
    u_strToUpper(reinterpret_cast<UChar*>(out_string), upperLength + 1, reinterpret_cast<const UChar*>(in_string),
                 length, locale ? locale->name() : nullptr, &status);
    if (U_FAILURE(status)) {
        delete[] out_string;
        return nullptr;
    }
    return out_string;
}
char_utf32* create_uppercase(const char_utf32* in_string, edvar::internationalization::locale* locale) {
    char_utf16* intermediate = create_utf16(in_string);
    if (!intermediate)
        return nullptr;
    char_utf16* uppercased = create_uppercase(intermediate, locale);
    delete[] intermediate;
    if (!uppercased)
        return nullptr;
    char_utf32* out_string = create_utf32(uppercased);
    delete[] uppercased;
    return out_string;
}

int compare(const char_utf8* str1, const char_utf8* str2, bool case_sensitive,
            const edvar::internationalization::locale* locale) {
    char_utf16* intermediate1 = create_utf16(str1);
    char_utf16* intermediate2 = create_utf16(str2);
    if (!intermediate1 || !intermediate2) {
        delete[] intermediate1;
        delete[] intermediate2;
        return 0; // indicate error in conversion
    }
    int result = compare(intermediate1, intermediate2, case_sensitive, locale);
    delete[] intermediate1;
    delete[] intermediate2;
    return result;
}
int compare(const char_utf16* str1, const char_utf16* str2, bool case_sensitive,
            const edvar::internationalization::locale* locale) {
    if (!str1 || !str2) {
        if (str1 == str2)
            return 0;
    } else if (str1 == nullptr) {
        return -1;
    } else if (str2 == nullptr) {
        return 1;
    }
    UErrorCode status = U_ZERO_ERROR;
    int32_t length1 = static_cast<int32_t>(edvar::c_string::string_length(str1));
    int32_t length2 = static_cast<int32_t>(edvar::c_string::string_length(str2));
    if (length1 != length2) {
        return (length1 < length2) ? -1 : 1;
    }
    if (locale == nullptr) {
        locale = &edvar::internationalization::locale::current();
    }
    icu::Collator* coll =
        icu::Collator::createInstance(*reinterpret_cast<const icu::Locale*>(locale->get_icu_locale_data()), status);
    if (U_FAILURE(status) || !coll) {
        delete coll;
        return 0; // error opening collator
    }
    coll->setStrength(case_sensitive ? icu::Collator::TERTIARY : icu::Collator::SECONDARY);
    int cmp =
        coll->compare(reinterpret_cast<const UChar*>(str1), length1, reinterpret_cast<const UChar*>(str2), length2);
    delete coll;
    return cmp;
}
int compare(const char_utf32* str1, const char_utf32* str2, bool case_sensitive,
            const edvar::internationalization::locale* locale) {
    char_utf16* intermediate1 = create_utf16(str1);
    char_utf16* intermediate2 = create_utf16(str2);
    if (!intermediate1 || !intermediate2) {
        delete[] intermediate1;
        delete[] intermediate2;
        return 0; // indicate error in conversion
    }
    int result = compare(intermediate1, intermediate2, case_sensitive, locale);
    delete[] intermediate1;
    delete[] intermediate2;
    return result;
}

bool is_whitespace(char_utf8 ch) { return is_whitespace(static_cast<char_utf16>(ch)); }
bool is_whitespace(char_utf16 ch) { return u_isUWhiteSpace(static_cast<UChar32>(ch)); }
bool is_whitespace(char_utf32 ch) { return is_whitespace(static_cast<char_utf16>(ch)); }

// Parsing/wrapper implementations declared in the public header.
value_or_error_code<int64_t> parse_signed(const char_utf16* string, int base,
                                          const edvar::internationalization::locale* locale) {
    if (base != 10) {
        return value_or_error_code<int64_t>::from_value(
            std::move(std::strtoll(reinterpret_cast<const char*>(string), nullptr, base)));
    }
    if (!locale)
        locale = &edvar::internationalization::locale::current();
    // Use ICU number parsing with the provided locale.
    UErrorCode status = U_ZERO_ERROR;
    const icu::Locale* icu_loc = reinterpret_cast<const icu::Locale*>(locale->get_icu_locale_data());
    icu::UnicodeString ustr = icu::UnicodeString(string);
    icu::NumberFormat* nf = icu::NumberFormat::createInstance(*icu_loc, status);
    if (U_FAILURE(status) || !nf) {
        delete nf;
        return value_or_error_code<int64_t>::from_error(1);
    }
    icu::Formattable fmt;
    icu::ParsePosition parsePos(0);
    nf->parse(ustr, fmt, parsePos);
    int32_t parsedCount = parsePos.getIndex();
    if (parsedCount == 0) {
        delete nf;
        return value_or_error_code<int64_t>::from_error(2);
    }
    int64_t d = fmt.getInt64();
    delete nf;
    return value_or_error_code<int64_t>::from_value(std::move(d));
}

value_or_error_code<double> parse_double(const char_utf16* string, const edvar::internationalization::locale* locale) {
    if (!locale)
        locale = &edvar::internationalization::locale::current();
    // Use ICU number parsing with the provided locale.
    UErrorCode status = U_ZERO_ERROR;
    const icu::Locale* icu_loc = reinterpret_cast<const icu::Locale*>(locale->get_icu_locale_data());
    icu::UnicodeString ustr = icu::UnicodeString(string);
    icu::NumberFormat* nf = icu::NumberFormat::createInstance(*icu_loc, status);
    if (U_FAILURE(status) || !nf) {
        delete nf;
        return value_or_error_code<double>::from_error(1);
    }
    icu::Formattable fmt;
    icu::ParsePosition parsePos(0);
    nf->parse(ustr, fmt, parsePos);
    int32_t parsedCount = parsePos.getIndex();
    if (parsedCount == 0) {
        delete nf;
        return value_or_error_code<double>::from_error(1);
    }
    double d = fmt.getDouble();
    delete nf;
    return value_or_error_code<double>::from_value(d);
}
} // namespace edvar::c_string