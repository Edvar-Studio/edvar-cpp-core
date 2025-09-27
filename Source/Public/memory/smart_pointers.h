#pragma once

#include "reference_counters.h"

namespace edvar::memory {
template <typename storage_type> class shared_pointer {
public:
    shared_pointer(nullptr_t) : _ptr(nullptr), _ref_counter(nullptr) {}
    shared_pointer() : shared_pointer(nullptr) {}
    shared_pointer(storage_type* ptr) : _ptr(ptr), _ref_counter(new reference_counter()) {}
    shared_pointer(const shared_pointer& other) : _ptr(other._ptr), _ref_counter(other._ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_ref();
        }
    }
    shared_pointer(shared_pointer&& other) noexcept : _ptr(other._ptr), _ref_counter(other._ref_counter) {
        other._ptr = nullptr;
        other._ref_counter = nullptr;
    }
    ~shared_pointer() {
        if (_ref_counter && _ref_counter->release() == 0) {
            delete _ptr;
            delete _ref_counter;
        }
    }
    storage_type& operator*() const { return *_ptr; }
    storage_type* operator->() const { return _ptr; }
    void reset() {
        if (_ref_counter && _ref_counter->release() == 0) {
            delete _ptr;
            delete _ref_counter;
        }
        _ptr = nullptr;
        _ref_counter = nullptr;
    }
    bool is_valid() const { return _ptr != nullptr; }
    operator bool() const { return is_valid(); }
    storage_type* get() const { return _ptr; }
    int32 use_count() const { return _ref_counter ? _ref_counter->get_count() : 0; }

private:
    storage_type* _ptr;
    reference_counter* _ref_counter;
};

namespace __private {
class weak_pointer_base {
protected:
    virtual ~weak_pointer_base() = default;
    virtual void reset() = 0;
    bool _is_destroyed;
    friend class ::edvar::memory::reference_counter;
};
} // namespace __private

template <typename storage_type> class weak_pointer : public __private::weak_pointer_base {
public:
    weak_pointer() : _ptr(nullptr), _ref_counter(nullptr) {}
    weak_pointer(nullptr_t) : weak_pointer() {}
    weak_pointer(const shared_pointer<storage_type>& shared_ptr)
        : _ptr(shared_ptr.get()), _ref_counter(shared_ptr._ref_counter) {}
    weak_pointer(const weak_pointer& other) : _ptr(other._ptr), _ref_counter(other._ref_counter) {}
    weak_pointer(weak_pointer&& other) noexcept : _ptr(other._ptr), _ref_counter(other._ref_counter) {
        other._ptr = nullptr;
        other._ref_counter = nullptr;
    }
    ~weak_pointer() = default;

    shared_pointer<storage_type> lock() const {
        if (_ref_counter && _ref_counter->get_count() > 0) {
            return shared_pointer<storage_type>(_ptr);
        }
        return shared_pointer<storage_type>(nullptr);
    }

    virtual void reset() override {
        _ptr = nullptr;
        _ref_counter = nullptr;
    }

    bool is_valid() const { return !_is_destroyed && _ptr != nullptr && _ref_counter && _ref_counter->get_count() > 0; }
    operator bool() const { return is_valid(); }
    storage_type* get() const { return is_valid() ? _ptr : nullptr; }
    int32 use_count() const { return _ref_counter ? _ref_counter->get_count() : 0; }

private:
    reference_counter* _ref_counter;
    storage_type* _ptr;
};

template <typename storage_type> class unique_pointer {
public:
    unique_pointer() : _ptr(nullptr) {}
    unique_pointer(nullptr_t) : unique_pointer() {}
    explicit unique_pointer(storage_type* ptr) : _ptr(ptr) {}
    unique_pointer(const unique_pointer& other) = delete; // non-copyable
    unique_pointer(unique_pointer&& other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }
    ~unique_pointer() {
        delete _ptr;
        _ptr = nullptr;
    }
    unique_pointer& operator=(const unique_pointer& other) = delete; // non-copyable
    unique_pointer& operator=(unique_pointer&& other) noexcept {
        if (this != &other) {
            delete _ptr;
            _ptr = other._ptr;
            other._ptr = nullptr;
        }
        return *this;
    }
    storage_type& operator*() const { return *_ptr; }
    storage_type* operator->() const { return _ptr; }
    void reset() {
        delete _ptr;
        _ptr = nullptr;
    }
    bool is_valid() const { return _ptr != nullptr; }
    operator bool() const { return is_valid(); }
    storage_type* get() const { return _ptr; }

private:
    storage_type* _ptr;
};

} // namespace edvar::memory