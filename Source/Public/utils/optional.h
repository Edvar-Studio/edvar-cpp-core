#pragma once

namespace edvar {
template <typename in_type> struct optional {

    optional() : _has_value(false), _value() {}
    optional(const in_type& val) : _has_value(true), _value(val) {}
    operator bool() const { return _has_value; }
    in_type& operator*() { return _value; }
    const in_type& operator*() const { return _value; }

    in_type& value_or(const in_type& default_value) { return _has_value ? _value : default_value; }
    in_type& value() { return _value; }

    const in_type& value_or(const in_type& default_value) const { return _has_value ? _value : default_value; }
    const in_type& value() const { return _value; }

    bool has_value() const { return _has_value; }


private:
    bool _has_value;
    in_type _value;
};


template <typename in_type> struct error_result{
    error_result() : _has_error(false), _error() {}
    error_result(const in_type& err) : _has_error(true), _error(err) {}
    operator bool() const { return _has_error; }
    in_type& operator*() { return _error; }
    const in_type& operator*() const { return _error; }

    in_type& error() { return _error; }
    const in_type& error() const { return _error; }



    template<typename select_type> select_type& select(select_type& on_success, select_type& on_error) const{
        if(_has_error){
            return on_error;
        }
        return on_success;
    }

    bool has_error() const { return _has_error; }
private:
    bool _has_error;
    in_type _error;
};
} // namespace edvar