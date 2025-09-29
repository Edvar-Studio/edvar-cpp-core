#include "internationalization/locale.h"
#include "containers/string.h"
#include "unicode/utypes.h"
#include <cstring>
#include "containers/string.h"
#define U_HIDE_DEPRECATED_API
#define U_HIDE_INTERNAL_API
#include "unicode/unistr.h"
#include "unicode/locid.h"
#undef U_HIDE_INTERNAL_API
#undef U_HIDE_DEPRECATED_API
namespace edvar::internationalization {
array<locale*> locale::_locale_registry;
locale* locale::_root_locale = nullptr;

static struct _locale_initializer {
    _locale_initializer() {
        int count = 0;
        const icu::Locale* elements = icu::Locale::getAvailableLocales(count);

        for (int i = 0; i < count; i++) {
            const icu::Locale& element = elements[i];
            locale::register_from_icu_locale(&element);
        }
    }
} _locale_initializer_instance;

const locale& locale::invariant() {
    if (_root_locale == nullptr) {
        locale::register_from_icu_locale(&icu::Locale::getRoot());
    }
    return *locale::_root_locale;
}
const locale& locale::current() {
    const icu::Locale& current_locale = icu::Locale::getDefault();
    for (uint32 i = 0; i < static_cast<uint32>(locale::_locale_registry.length()); i++) {
        if (static_cast<const icu::Locale*>(locale::_locale_registry[i]->_icu_locale_data) == &current_locale) {
            return *locale::_locale_registry[i];
        }
    }
    // not found, register it
    locale::register_from_icu_locale(&current_locale);
    return *locale::_locale_registry[locale::_locale_registry.length() - 1];
}

const locale* locale::get(const char* language, const char* country, const char* variant,
                          const char* keywords_and_values) {
    if (!language)
        return nullptr;
    for (uint32 i = 0; i < static_cast<uint32>(locale::_locale_registry.length()); i++) {
        const locale* loc = locale::_locale_registry[i];
        if (strcmp(loc->language(), language) != 0) {
            continue;
        }
        if (country && strcmp(loc->country(), country) != 0) {
            continue;
        }
        if (variant && strcmp(loc->variant(), variant) != 0) {
            continue;
        }
        // TODO: keywords_and_values comparison
        return loc;
    }
    return nullptr;
}

const char* locale::language() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getLanguage(); }
const char* locale::script() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getScript(); }
const char* locale::country() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getCountry(); }
const char* locale::variant() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getVariant(); }
const char* locale::name() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getName(); }
const char* locale::language_iso3() const {
    return static_cast<const icu::Locale*>(_icu_locale_data)->getISO3Language();
}
const char* locale::country_iso3() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getISO3Country(); }
uint32 locale::lcid() const { return static_cast<const icu::Locale*>(_icu_locale_data)->getLCID(); }
bool locale::is_right_to_left() const { return static_cast<const icu::Locale*>(_icu_locale_data)->isRightToLeft(); }
bool locale::is_valid() const {
    return _icu_locale_data != nullptr && !static_cast<const icu::Locale*>(_icu_locale_data)->isBogus();
}
edvar::string_utf16 locale::display_language(const locale* in_locale) const {
    // if cached already, return cached value.
    if (auto result = _display_name_language_cache.try_get(in_locale)) {
        return result.get_value();
    }

    icu::UnicodeString result;
    if (in_locale) {
        result = static_cast<const icu::Locale*>(_icu_locale_data)
                     ->getDisplayLanguage(*static_cast<const icu::Locale*>(in_locale->_icu_locale_data), result);
        const_cast<locale*>(this)->_display_name_language_cache[in_locale] =
            edvar::string_utf16(result.getBuffer(), result.length());
    } else {
        result = static_cast<const icu::Locale*>(_icu_locale_data)->getDisplayLanguage(result);
    }
    edvar::string_utf16 return_value(result.getBuffer(), result.length());
    return return_value;
}

edvar::string_utf16 locale::display_script(const locale* in_locale) const {
    // if cached already, return cached value.
    if (auto result = _display_name_script_cache.try_get(in_locale)) {
        return result.get_value();
    }

    icu::UnicodeString result;
    if (in_locale) {
        result = static_cast<const icu::Locale*>(_icu_locale_data)
                     ->getDisplayScript(*static_cast<const icu::Locale*>(in_locale->_icu_locale_data), result);
        const_cast<locale*>(this)->_display_name_script_cache[in_locale] =
            edvar::string_utf16(result.getBuffer(), result.length());
    } else {
        result = static_cast<const icu::Locale*>(_icu_locale_data)->getDisplayScript(result);
    }
    edvar::string_utf16 return_value(result.getBuffer(), result.length());
    return return_value;
}

edvar::string_utf16 locale::display_country(const locale* in_locale) const {
    // if cached already, return cached value.
    if (auto result = _display_name_country_cache.try_get(in_locale)) {
        return result.get_value();
    }

    icu::UnicodeString result;
    if (in_locale) {
        result = static_cast<const icu::Locale*>(_icu_locale_data)
                     ->getDisplayCountry(*static_cast<const icu::Locale*>(in_locale->_icu_locale_data), result);
        const_cast<locale*>(this)->_display_name_country_cache[in_locale] =
            edvar::string_utf16(result.getBuffer(), result.length());
    } else {
        result = static_cast<const icu::Locale*>(_icu_locale_data)->getDisplayCountry(result);
    }
    edvar::string_utf16 return_value(result.getBuffer(), result.length());
    return return_value;
}

edvar::string_utf16 locale::display_variant(const locale* in_locale) const {
    // if cached already, return cached value.
    if (auto result = _display_name_variant_cache.try_get(in_locale)) {
        return result.get_value();
    }

    icu::UnicodeString result;
    if (in_locale) {
        result = static_cast<const icu::Locale*>(_icu_locale_data)
                     ->getDisplayVariant(*static_cast<const icu::Locale*>(in_locale->_icu_locale_data), result);
        const_cast<locale*>(this)->_display_name_variant_cache[in_locale] =
            edvar::string_utf16(result.getBuffer(), result.length());
    } else {
        result = static_cast<const icu::Locale*>(_icu_locale_data)->getDisplayVariant(result);
    }
    edvar::string_utf16 return_value(result.getBuffer(), result.length());
    return return_value;
}

edvar::string_utf16 locale::display_name(const locale* in_locale) const {
    // if cached already, return cached value.
    if (auto result = _display_name_cache.try_get(in_locale)) {
        return result.get_value();
    }

    icu::UnicodeString result;
    if (in_locale) {
        result = static_cast<const icu::Locale*>(_icu_locale_data)
                     ->getDisplayName(*static_cast<const icu::Locale*>(in_locale->_icu_locale_data), result);
        const_cast<locale*>(this)->_display_name_cache[in_locale] =
            edvar::string_utf16(result.getBuffer(), result.length());
    } else {
        result = static_cast<const icu::Locale*>(_icu_locale_data)->getDisplayName(result);
    }
    edvar::string_utf16 return_value(result.getBuffer(), result.length());
    return return_value;
}

locale::~locale() { delete static_cast<const icu::Locale*>(_icu_locale_data); }

void locale::register_from_icu_locale(const void* icu_locale) {
    const icu::Locale* loc = static_cast<const icu::Locale*>(icu_locale);
    locale* new_locale = new locale();
    new_locale->_icu_locale_data = loc;
    _locale_registry.add(edvar::move(new_locale));
    if (loc == &icu::Locale::getRoot()) {
        _root_locale = new_locale;
    }
}
} // namespace edvar::internationalization