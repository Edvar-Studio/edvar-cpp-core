#pragma once
namespace Edvar::I18N {
class Locale {
public:
    static const Locale& Default();
    static const Locale& Invariant();

    static const Locale& English();
    static const Locale& Turkish();
    static const Locale& Japanese();

    static const Locale* Find(const char16_t* language = nullptr, const char16_t* country = nullptr,
                              const char16_t* variant = nullptr);

    bool operator==(const Locale& other);
    bool operator!=(const Locale& other) { return !(*this == other); }

    static void SetDefault(const Locale& toSetLocale);

    bool IsValid() const;

    void* GetData() const { return icuLocale; }


    Containers::StringBase<char16_t> ToString() const;
private:
    explicit Locale(const char16_t* language = nullptr, const char16_t* country = nullptr,
                    const char16_t* variant = nullptr, bool addToRegistry = true);
    ~Locale();

    void* icuLocale = nullptr;

    static Containers::List<Locale*> createdLocales;
    static Threading::Mutex createdLocalesMutex;
};
} // namespace Edvar::I18N