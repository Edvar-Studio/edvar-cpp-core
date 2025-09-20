#pragma once

namespace edvar::internationalization {
class locale {
public:
    static locale& invariant();
    static locale& current();

    static locale& get(const char* language, const char* country = nullptr, const char* variant = nullptr,
                       const char* keywords_and_values = nullptr);
    static locale& get_from_lang_tag(const char* lang_tag);

    const char* language() const;
    const char* script() const;
    const char* country() const;
    const char* variant() const;
    const char* name() const;
    const char* language_iso3() const;
    const char* country_iso3() const;
    uint32 lcid() const;
    bool is_right_to_left() const;

    edvar::string display_language(const locale* in_locale = nullptr) const;
    edvar::string display_script(const locale* in_locale = nullptr) const;
    edvar::string display_country(const locale* in_locale = nullptr) const;
    edvar::string display_variant(const locale* in_locale = nullptr) const;
    edvar::string display_name(const locale* in_locale = nullptr) const;

    bool is_valid() const;

    operator bool() const { return is_valid(); }
    bool operator==(const locale& other) const;
    bool operator!=(const locale& other) const { return !(*this == other); }

private:
    const void* _icu_locale_data = nullptr;

    locale() = default;
    ~locale();
    locale(const locale&) = default;
    locale& operator=(const locale&) = default;
    locale(locale&&) = default;
    locale& operator=(locale&&) = default;
    static void register_from_icu_locale(const void* icu_locale);

    static array<locale*> _locale_registry;
    friend class _locale_initializer;
};

} // namespace edvar::internationalization