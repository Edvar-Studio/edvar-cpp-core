#pragma once

namespace Edvar::Utils {
template <typename T> class Optional {
public:
    Optional() : hasValue(false), value() {}
    Optional(const T& val) : hasValue(true), value(val) {}
    Optional(T&& val) : hasValue(true), value(std::move(val)) {}

    Optional(const Optional& other) : hasValue(other.hasValue), value(other.value) {}
    Optional(Optional&& other) noexcept : hasValue(other.hasValue), value(std::move(other.value)) {
        other.hasValue = false;
    }

    Optional& operator=(const Optional& other) {
        if (this != &other) {
            hasValue = other.hasValue;
            value = other.value;
        }
        return *this;
    }
    Optional& operator=(Optional&& other) noexcept {
        if (this != &other) {
            hasValue = other.hasValue;
            value = std::move(other.value);
            other.hasValue = false;
        }
        return *this;
    }

    T& Get() { return value; }
    const T& Get() const { return value; }
    T GetOrDefault(const T& defaultValue = T()) const { return hasValue ? value : defaultValue; }

    bool HasValue() const { return hasValue; }

    void Reset() {
        hasValue = false;
        value = T();
    }
    void Set(const T& val) {
        hasValue = true;
        value = val;
    }

    const T& operator*() const { return value; }
    T& operator*() { return value; }
    const T* operator->() const { return &value; }
    T* operator->() { return &value; }

    operator bool() const { return hasValue; }

private:
    bool hasValue;
    T value;
};
} // namespace Edvar::Utils