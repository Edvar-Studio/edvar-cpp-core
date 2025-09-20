#pragma once

#include "internationalization/locale.h"
#define U_HIDE_DEPRECATED_API
#define U_HIDE_INTERNAL_API
#include <unicode/unistr.h>
#include <unicode/locid.h>
#undef U_HIDE_INTERNAL_API
#undef U_HIDE_DEPRECATED_API
namespace edvar::internationalization {
array<locale*> locale::_locale_registry;

static struct _locale_initializer {
    _locale_initializer() {
        int count = 0;
        const icu::Locale* elements = icu::Locale::getAvailableLocales(count);
        for (int i = 0; i < count; i++) {
            const icu::Locale& element = elements[i];
            locale::register_from_icu_locale(&element);
        }
    }
} _locale_init_instance;

const char* locale::language() const{
    return static_cast<const icu::Locale*>(_icu_locale_data)->getLanguage();
}

const char* locale::script() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getScript();
}
const char* locale::country() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getCountry();
}
const char* locale::variant() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getVariant();
}
const char* locale::name() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getName();
}
const char* locale::language_iso3() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getISO3Language();
}
const char* locale::country_iso3() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getISO3Country();
}
uint32 locale::lcid() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getLCID();
}
bool locale::is_right_to_left() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->isRightToLeft();
}
bool locale::is_valid() const {
    return _icu_locale_data != nullptr && !static_cast<const icu::Locale*>(_icu_locale_data)->isBogus();
}
edvar::string locale::display_language(const locale* in_locale) const {
    icu::UnicodeString result;
    if (in_locale) {
        result = static_cast<const icu::Locale*>(_icu_locale_data)
                     ->getDisplayLanguage(*static_cast<const icu::Locale*>(in_locale->_icu_locale_data),
                                         result);
    } else {
        result = static_cast<const icu::Locale*>(_icu_locale_data)->getDisplayLanguage(result);
    }
    edvar::string return_value(result.getBuffer(), result.length());
    return return_value;
}


locale::~locale() { delete static_cast<const icu::Locale*>(_icu_locale_data); }

void locale::register_from_icu_locale(const void* icu_locale) {
    const icu::Locale* loc = static_cast<const icu::Locale*>(icu_locale);
    locale* new_locale = new locale();
    new_locale->_icu_locale_data = loc;
    _locale_registry.add(new_locale);
}

} // namespace edvar::internationalization