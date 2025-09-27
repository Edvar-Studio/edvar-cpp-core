#pragma once

namespace edvar {
template <typename value_type, typename error_type> class value_or_error {
public:
    value_or_error() : _state(ERROR), _value(), _error() {}
    static value_or_error from_value(const value_type& value) {
        value_or_error voe;
        voe._state = VALUE;
        voe._value = value;
        return voe;
    }
    static value_or_error from_value(value_type&& value) {
        value_or_error voe;
        voe._state = VALUE;
        voe._value = edvar::move(value);
        return voe;
    }
    static value_or_error from_error(const error_type& error) {
        value_or_error voe;
        voe._state = ERROR;
        voe._error = error;
        return voe;
    }
    static value_or_error from_error(error_type&& error) {
        value_or_error voe;
        voe._state = ERROR;
        voe._error = edvar::move(error);
        return voe;
    }

    bool is_value() const { return _state == VALUE; }
    bool is_error() const { return _state == ERROR; }
    const value_type& get_value() const { return _value; }
    const error_type& get_error() const { return _error; }
    value_type& get_value() { return _value; }
    error_type& get_error() { return _error; }

    value_type& value_or_default(value_type& default_value) {
        if (is_value())
            return _value;
        return default_value;
    }
    error_type& error_or_default(error_type& default_error) {
        if (is_error())
            return _error;
        return default_error;
    }

    value_or_error& attach_value_without_state_change(const value_type& value) {
        _value = value;
        return *this;
    }
    value_or_error& attach_value_without_state_change(value_type&& value) {
        _value = edvar::move(value);
        return *this;
    }
    value_or_error& attach_error_without_state_change(const error_type& error) {
        _error = error;
        return *this;
    }
    value_or_error& attach_error_without_state_change(error_type&& error) {
        _error = edvar::move(error);
        return *this;
    }

    operator bool() const { return is_value(); }

private:
    enum value_or_error_state { VALUE, ERROR } _state;
    value_type _value;
    error_type _error;
};
template <typename value_type> using value_or_error_code = value_or_error<value_type, int32>;
template <typename value_type> using value_or_error_message = value_or_error<value_type, wchar_t[1024]>;
} // namespace edvar