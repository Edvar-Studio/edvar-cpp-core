#pragma once

namespace edvar::internationalization {
class locale {
public:
    static const locale& invariant();
    static const locale& current();

    static const locale* get(const char* language, const char* country = nullptr, const char* variant = nullptr,
                       const char* keywords_and_values = nullptr);
    static const locale* get_from_lang_tag(const char* lang_tag);

    const char* language() const;
    const char* script() const;
    const char* country() const;
    const char* variant() const;
    const char* name() const;
    const char* language_iso3() const;
    const char* country_iso3() const;
    uint32 lcid() const;
    bool is_right_to_left() const;

    edvar::string_utf16 display_language(const locale* in_locale = nullptr) const;
    edvar::string_utf16 display_script(const locale* in_locale = nullptr) const;
    edvar::string_utf16 display_country(const locale* in_locale = nullptr) const;
    edvar::string_utf16 display_variant(const locale* in_locale = nullptr) const;
    edvar::string_utf16 display_name(const locale* in_locale = nullptr) const;

    bool is_valid() const;

    operator bool() const { return is_valid(); }
    bool operator==(const locale& other) const;
    bool operator!=(const locale& other) const { return !(*this == other); }
    const void* get_icu_locale_data() const { return _icu_locale_data; }

private:
    const void* _icu_locale_data = nullptr;
    // cache display names for other locales
    map<const locale*, edvar::string_utf16> _display_name_language_cache;
    map<const locale*, edvar::string_utf16> _display_name_script_cache;
    map<const locale*, edvar::string_utf16> _display_name_country_cache;
    map<const locale*, edvar::string_utf16> _display_name_variant_cache;
    map<const locale*, edvar::string_utf16> _display_name_cache;

    locale() = default;
    ~locale();
    locale(const locale& other) = delete; // non-copyable
    locale(locale&& other) {
        _icu_locale_data = other._icu_locale_data;
        other._icu_locale_data = nullptr;
    }
    locale& operator=(locale&& other) {
        if (this != &other) {
            _icu_locale_data = other._icu_locale_data;
            other._icu_locale_data = nullptr;
        }
        return *this;
    }
    static void register_from_icu_locale(const void* icu_locale);

    static array<locale*> _locale_registry;
    static locale* _root_locale;
    friend class _locale_initializer;
};

} // namespace edvar::internationalization