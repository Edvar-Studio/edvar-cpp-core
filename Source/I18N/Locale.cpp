#include "I18N/Locale.hpp"
#include "Threading/Mutex.hpp"
#include "Utils/CString.hpp"
#include "unicode/locid.h"

namespace Edvar::I18N {

Containers::List<Locale*> Locale::createdLocales;
Threading::Mutex Locale::createdLocalesMutex;

Locale::Locale(const char16_t* language, const char16_t* country, const char16_t* variant, bool addToRegistry) {
    char *langBuffer = nullptr, *countryBuffer = nullptr, *variantBuffer = nullptr;
    if (language != nullptr) {
        uint32_t langBufferLen = Utils::CStrings::ToCharString(language, nullptr, 0);
        langBuffer = new char[langBufferLen];
        Utils::CStrings::ToCharString(language, langBuffer, langBufferLen);
    }
    if (country != nullptr) {
        uint32_t countryLen = Utils::CStrings::ToCharString(country, nullptr, 0);
        countryBuffer = new char[countryLen];
        Utils::CStrings::ToCharString(country, countryBuffer, countryLen);
    }
    if (variant != nullptr) {
        uint32_t variantLen = Utils::CStrings::ToCharString(variant, nullptr, 0);
        variantBuffer = new char[variantLen];
        Utils::CStrings::ToCharString(variant, variantBuffer, variantLen);
    }

    icuLocale = new icu::Locale(langBuffer, countryBuffer, variantBuffer);
    if (addToRegistry) {
        Threading::ScopedLock lock(createdLocalesMutex);
        createdLocales.Add(this);
    }
    delete[] langBuffer;
    delete[] countryBuffer;
    delete[] variantBuffer;
}
Locale::~Locale() {
    Threading::ScopedLock lock(createdLocalesMutex);
    createdLocales.Remove(this);
    delete static_cast<icu::Locale*>(icuLocale);
}
const Locale* Locale::Find(const char16_t* language, const char16_t* country, const char16_t* variant) {
    // try to find in existing locales
    Locale tempLocale(language, country, variant, false);
    if (tempLocale.icuLocale == nullptr)
        return nullptr;
    Threading::ScopedLock lock(createdLocalesMutex);
    for (int i = 0; i < createdLocales.Length(); ++i) {
        Locale* locale = createdLocales.Get(i);
        if (locale->icuLocale == nullptr)
            continue;
        icu::Locale* icuLocalePtr = static_cast<icu::Locale*>(locale->icuLocale);
        if (*icuLocalePtr == *static_cast<icu::Locale*>(tempLocale.icuLocale)) {
            return locale;
        }
    }
    // linear-search couldn't find it, create a new one
    Locale* newLocale = new Locale(language, country, variant);
    if (newLocale->IsValid() == false) {
        delete newLocale;
        return nullptr;
    }
    return newLocale;
}
const Locale& Locale::Default() {
    static Locale* defaultLocale = nullptr;
    if (defaultLocale == nullptr) [[unlikely]] {
        icu::Locale icuDefaultLocale = icu::Locale::getDefault();
        auto* country = Utils::CStrings::CreateConvertedString<char16_t>(icuDefaultLocale.getCountry());
        auto* language = Utils::CStrings::CreateConvertedString<char16_t>(icuDefaultLocale.getLanguage());
        auto* variant = Utils::CStrings::CreateConvertedString<char16_t>(icuDefaultLocale.getVariant());

        const Locale* foundLocale = Find(language, country, variant);
        defaultLocale = const_cast<Locale*>(foundLocale);
        if (defaultLocale == nullptr) {
            defaultLocale = const_cast<Locale*>(&Invariant());
        }
        delete[] country;
        delete[] language;
        delete[] variant;
    }
    return *defaultLocale;
}
const Locale& Locale::Invariant() {
    static Locale* invariantLocale = nullptr;
    if (invariantLocale == nullptr) [[unlikely]] {
        icu::Locale icuInvariantLocale = icu::Locale::getRoot();
        auto* country = Utils::CStrings::CreateConvertedString<char16_t>(icuInvariantLocale.getCountry());
        auto* language = Utils::CStrings::CreateConvertedString<char16_t>(icuInvariantLocale.getLanguage());
        auto* variant = Utils::CStrings::CreateConvertedString<char16_t>(icuInvariantLocale.getVariant());
        const Locale* foundLocale = Find(language, country, variant);
        invariantLocale = const_cast<Locale*>(foundLocale);
        // This should always be found
        delete[] country;
        delete[] language;
        delete[] variant;
    }
    return *invariantLocale;
}
const Locale& Locale::English() {
    static Locale* englishLocale = nullptr;
    if (englishLocale == nullptr) [[unlikely]] {
        icu::Locale icuEnglishLocale = icu::Locale::getEnglish();
        auto* country = Utils::CStrings::CreateConvertedString<char16_t>(icuEnglishLocale.getCountry());
        auto* language = Utils::CStrings::CreateConvertedString<char16_t>(icuEnglishLocale.getLanguage());
        auto* variant = Utils::CStrings::CreateConvertedString<char16_t>(icuEnglishLocale.getVariant());
        const Locale* foundLocale = Find(language, country, variant);
        englishLocale = const_cast<Locale*>(foundLocale);
        // This should always be found
        delete[] country;
        delete[] language;
        delete[] variant;
    }
    return *englishLocale;
}
const Locale& Locale::Turkish() {
    static Locale* turkishLocale = nullptr;
    if (turkishLocale == nullptr) [[unlikely]] {
        icu::Locale icuEnglishLocale = icu::Locale("tr", "TR");
        auto* country = Utils::CStrings::CreateConvertedString<char16_t>(icuEnglishLocale.getCountry());
        auto* language = Utils::CStrings::CreateConvertedString<char16_t>(icuEnglishLocale.getLanguage());
        auto* variant = Utils::CStrings::CreateConvertedString<char16_t>(icuEnglishLocale.getVariant());
        const Locale* foundLocale = Find(language, country, variant);
        if (foundLocale == nullptr) {
            foundLocale = &Default();
        }
        turkishLocale = const_cast<Locale*>(foundLocale);
        delete[] country;
        delete[] language;
        delete[] variant;
    }
    return *turkishLocale;
}
const Locale& Locale::Japanese() {
    static Locale* japaneseLocale = nullptr;
    if (japaneseLocale == nullptr) [[unlikely]] {
        icu::Locale icuJapaneseLocale = icu::Locale::getJapanese();
        auto* country = Utils::CStrings::CreateConvertedString<char16_t>(icuJapaneseLocale.getCountry());
        auto* language = Utils::CStrings::CreateConvertedString<char16_t>(icuJapaneseLocale.getLanguage());
        auto* variant = Utils::CStrings::CreateConvertedString<char16_t>(icuJapaneseLocale.getVariant());
        const Locale* foundLocale = Find(language, country, variant);
        japaneseLocale = const_cast<Locale*>(foundLocale);
        // This should always be found
        delete[] country;
        delete[] language;
        delete[] variant;
    }
    return *japaneseLocale;
}

bool Locale::IsValid() const {
    if (icuLocale == nullptr) [[unlikely]] {
        return false;
    }
    icu::Locale* icuLocalePtr = static_cast<icu::Locale*>(icuLocale);
    return icuLocalePtr->isBogus() == false;
}

String Locale::ToString() const {
    if (icuLocale == nullptr) {
        return String();
    }
    icu::Locale* icuLocalePtr = static_cast<icu::Locale*>(icuLocale);
    auto* localeName = Utils::CStrings::CreateConvertedString<char16_t>(icuLocalePtr->getName());
    String result(localeName);
    delete[] localeName;
    return result;
}
} // namespace Edvar::I18N