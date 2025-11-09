#pragma once

#include "reference_counters.h"

namespace edvar::memory {
template <typename> class shared_pointer;
template <typename> class weak_pointer;

// enable_shared_from_this support: lightweight implementation that relies on the
// library's weak_pointer/shared_pointer. No std headers beyond what's already
// available are used here.
template <typename T> class enable_shared_from_this {
protected:
    mutable weak_pointer<T> _weak_this;

    enable_shared_from_this() = default;
    enable_shared_from_this(const enable_shared_from_this&) {}
    enable_shared_from_this& operator=(const enable_shared_from_this&) { return *this; }
    ~enable_shared_from_this() = default;

public:
    shared_pointer<T> as_shared() { return _weak_this.lock(); }
    weak_pointer<T> as_weak() { return _weak_this; }

    template <typename U> shared_pointer<U> shared_from_this(const U* in_this) {
        return static_cast_shared_pointer<U>(_weak_this.lock());
    }
    template <typename U> shared_pointer<U> shared_from_this(const U* in_this) const {
        return static_cast_shared_pointer<U>(_weak_this.lock());
    }

    template <typename U> weak_pointer<U> weak_from_this() { return static_cast_weak_pointer<U>(_weak_this); }
    template <typename U> weak_pointer<U> weak_from_this() const { return static_cast_weak_pointer<U>(_weak_this); }

private:
    template <typename> friend class shared_pointer;
    void _internal_accept_owner(const shared_pointer<T>& sp) const { _weak_this = weak_pointer<T>(sp); }
};

template <typename storage_type> class shared_pointer {
public:
    shared_pointer(nullptr_t) : _ptr(nullptr), _ref_counter(nullptr) {}
    shared_pointer() : shared_pointer(nullptr) {}
    shared_pointer(storage_type* ptr) : _ptr(ptr), _ref_counter(new reference_counter()) {
        if constexpr (std::is_base_of_v<::edvar::memory::enable_shared_from_this<storage_type>, storage_type>) {
            if (_ptr) {
                // inform the enable_shared_from_this base about this owner
                _ptr->_internal_accept_owner(*this);
            }
        }
    }

    // construct a shared_pointer that adopts an existing reference_counter (used by weak_pointer::lock)
    shared_pointer(storage_type* ptr, reference_counter* ref_counter) : _ptr(ptr), _ref_counter(ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_ref();
        }
    }
    shared_pointer(const shared_pointer& other) : _ptr(other._ptr), _ref_counter(other._ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_ref();
        }
    }
    // converting copy constructor (allow casting between compatible shared_pointer types)
    template <typename U>
    shared_pointer(const shared_pointer<U>& other)
        : _ptr(static_cast<storage_type*>(other._ptr)), _ref_counter(other._ref_counter) {
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
    int32_t use_count() const { return _ref_counter ? _ref_counter->get_count() : 0; }

private:
    storage_type* _ptr;
    reference_counter* _ref_counter;
};

namespace __private {
class weak_pointer_base {
protected:
    virtual ~weak_pointer_base() = default;
    virtual void reset() = 0;
    bool _is_destroyed = false;
    friend class ::edvar::memory::reference_counter;
};
} // namespace __private

template <typename storage_type> class weak_pointer : public __private::weak_pointer_base {
public:
    weak_pointer() : _ptr(nullptr), _ref_counter(nullptr) {}
    weak_pointer(nullptr_t) : weak_pointer() {}
    weak_pointer(const shared_pointer<storage_type>& shared_ptr)
        : _ptr(shared_ptr.get()), _ref_counter(shared_ptr._ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_weak_ref(this);
        }
    }
    // converting constructors (allow casting between compatible weak_pointer/shared_pointer types)
    template <typename U>
    weak_pointer(const shared_pointer<U>& shared_ptr)
        : _ptr(static_cast<storage_type*>(shared_ptr.get())), _ref_counter(shared_ptr._ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_weak_ref(this);
        }
    }
    template <typename U>
    weak_pointer(const weak_pointer<U>& other)
        : _ptr(static_cast<storage_type*>(other._ptr)), _ref_counter(other._ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_weak_ref(this);
        }
    }
    weak_pointer(const weak_pointer& other) : _ptr(other._ptr), _ref_counter(other._ref_counter) {
        if (_ref_counter) {
            _ref_counter->add_weak_ref(this);
        }
    }
    weak_pointer(weak_pointer&& other) noexcept : _ptr(other._ptr), _ref_counter(other._ref_counter) {
        other._ptr = nullptr;
        other._ref_counter = nullptr;
    }
    ~weak_pointer() = default;

    shared_pointer<storage_type> lock() const {
        if (_ref_counter && _ref_counter->get_count() > 0) {
            return shared_pointer<storage_type>(_ptr, _ref_counter);
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
    int32_t use_count() const { return _ref_counter ? _ref_counter->get_count() : 0; }

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

// Utility cast helpers - mirror std::static_pointer_cast semantics for shared/weak pointers
template <typename To, typename From> shared_pointer<To> static_cast_shared_pointer(const shared_pointer<From>& sp) {
    return shared_pointer<To>(sp);
}

template <typename To, typename From> weak_pointer<To> static_cast_weak_pointer(const weak_pointer<From>& wp) {
    return weak_pointer<To>(wp);
}

template <typename To, typename From> weak_pointer<To> static_cast_weak_pointer(const shared_pointer<From>& sp) {
    return weak_pointer<To>(sp);
}



template<typename T, typename... Args> shared_pointer<T> make_shared(Args... args){
    return shared_pointer<T>(new T(std::forward<Args>(args)...));
}

} // namespace edvar::memory