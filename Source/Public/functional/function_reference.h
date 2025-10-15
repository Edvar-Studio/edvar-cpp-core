#pragma once
#include "containers/tuple.h"
#include "containers/map.h"
#include "diagnostics/assertion.h"
#include "memory/smart_pointers.h"

namespace edvar {
namespace __private {
class function_reference_impl_base {
public:
    virtual ~function_reference_impl_base() = default;

protected:
    virtual void invoke() = 0;
    virtual bool is_bound() const = 0;
};

template <typename return_type, typename... args>
class typed_function_reference_impl : public function_reference_impl_base {
public:
    virtual ~typed_function_reference_impl() = default;
    return_type invoke(const args&... arguments) {
        context = invocation_context{.call_arguments = tuple<args...>::make_tuple(arguments...)};
        invoke();
        return context.return_value;
    }

protected:
    class invocation_context {
        tuple<args...> call_arguments;
        return_type return_value;
    };
    invocation_context context;
};

template <typename return_type, typename... args>
class static_function_impl_reference : public typed_function_reference_impl<return_type, args...> {
public:
    static_function_impl_reference(return_type (*func)(args...)) : _func(func) {}

protected:
    virtual void invoke() override {
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(_func);
    }
    virtual bool is_bound() const override { return _func != nullptr; }

    return_type (*_func)(args...);
};

template <typename object_type, typename return_type, typename... args>
class member_function_reference : public typed_function_reference_impl<return_type, args...> {
public:
    member_function_reference(object_type* instance, return_type (object_type::*method)(args...))
        : _instance(instance), _method(method) {}

    virtual void invoke() override {
        auto apply_proxy = [this](args... arguments) -> return_type { return _instance->_method(arguments...); };
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(apply_proxy);
    }
    virtual bool is_bound() const override { return _instance != nullptr && _method != nullptr; }

private:
    object_type* _instance;
    return_type (object_type::*_method)(args...);
};

template <typename object_type, typename return_type, typename... args>
class member_const_function_reference : public typed_function_reference_impl<return_type, args...> {
public:
    member_const_function_reference(const object_type* instance, return_type (object_type::*method)(args...) const)
        : _instance(instance), _method(method) {}
    virtual void invoke() override {
        auto apply_proxy = [this](args... arguments) -> return_type { return _instance->_method(arguments...); };
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(apply_proxy);
    }
    virtual bool is_bound() const override { return _instance != nullptr && _method != nullptr; }

private:
    const object_type* _instance;
    return_type (object_type::*_method)(args...) const;
};

template <typename object_type, typename return_type, typename... args>
class shared_pointer_function_reference : public typed_function_reference_impl<return_type, args...> {
public:
    shared_pointer_function_reference(const memory::shared_pointer<object_type>& instance,
                                      return_type (object_type::*method)(args...))
        : _instance(instance), _method(method) {}

    virtual void invoke() override {
        auto apply_proxy = [this](args... arguments) -> return_type { return _instance->_method(arguments...); };
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(apply_proxy);
    }
    virtual bool is_bound() const override { return _instance.is_valid() && _method != nullptr; }

private:
    memory::shared_pointer<object_type> _instance;
    return_type (object_type::*_method)(args...);
};

template <typename object_type, typename return_type, typename... args>
class shared_pointer_const_function_reference : public typed_function_reference_impl<return_type, args...> {
public:
    shared_pointer_const_function_reference(const memory::shared_pointer<const object_type>& instance,
                                            return_type (object_type::*method)(args...) const)
        : _instance(instance), _method(method) {}

    virtual void invoke() override {
        auto apply_proxy = [this](args... arguments) -> return_type { return _instance->_method(arguments...); };
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(apply_proxy);
    }
    virtual bool is_bound() const override { return _instance.is_valid() && _method != nullptr; }

private:
    memory::shared_pointer<const object_type>& _instance;
    return_type (object_type::*_method)(args...) const;
};

template <typename object_type, typename return_type, typename... args>
class weak_pointer_function_reference : public typed_function_reference_impl<return_type, args...> {
public:
    weak_pointer_function_reference(const memory::weak_pointer<object_type>& instance,
                                    return_type (object_type::*method)(args...) const)
        : _instance(instance), _method(method) {}
    virtual void invoke() {
        auto apply_proxy = [this](args... arguments) -> return_type { return _instance->_method(arguments...); };
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(apply_proxy);
    }

    virtual bool is_bound() const override { return _instance.is_valid() && _method != nullptr; }

private:
    memory::weak_pointer<object_type> _instance;
    return_type (object_type::*_method)(args...);
};

template <typename object_type, typename return_type, typename... args>
class weak_pointer_const_function_reference : public typed_function_reference_impl<return_type, args...> {
public:
    weak_pointer_const_function_reference(const memory::weak_pointer<const object_type>& instance,
                                          return_type (object_type::*method)(args...) const)
        : _instance(instance), _method(method) {}
    virtual void invoke() {
        auto apply_proxy = [this](args... arguments) -> return_type { return _instance->_method(arguments...); };
        typed_function_reference_impl<return_type, args...>::context.return_value =
            typed_function_reference_impl<return_type, args...>::context.call_arguments.apply(apply_proxy);
    }

    virtual bool is_bound() const override { return _instance.is_valid() && _method != nullptr; }

private:
    memory::weak_pointer<const object_type> _instance;
    return_type (object_type::*_method)(args...) const;
};
} // namespace __private
template <typename return_value_type, typename... args> class function_reference {
public:
    static function_reference make_from_static(return_value_type (*func)(args...)) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>>(
                new __private::static_function_impl_reference<return_value_type, args...>(func));
        return to_return;
    }

    template <typename object_type>
    static function_reference make_from_shared(const memory::shared_pointer<object_type>& object_value,
                                               return_value_type (object_type::*func)(args...)) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>>(
                new __private::shared_pointer_function_reference<object_type, return_value_type, args...>(object_value,
                                                                                                          func));
        return to_return;
    }
    template <typename object_type>
    static function_reference make_from_shared(const memory::shared_pointer<const object_type>& object_value,
                                               return_value_type (object_type::*func)(args...) const) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>>(
                new __private::shared_pointer_const_function_reference<object_type, return_value_type, args...>(
                    object_value, func));
        return to_return;
    }

    template <typename object_type>
    static function_reference make_from_weak(const memory::shared_pointer<const object_type>& object_value,
                                             return_value_type (object_type::*func)(args...) const) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>>(
                new __private::weak_pointer_const_function_reference<object_type, args...>(object_value, func));
        return to_return;
    }
    template <typename object_type>
    static function_reference make_from_weak(const memory::shared_pointer<object_type>& object_value,
                                             return_value_type (object_type::*func)(args...)) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<object_type, return_value_type, args...>>(
                new __private::weak_pointer_function_reference<object_type, return_value_type, args...>(object_value,
                                                                                                        func));
        return to_return;
    }

    template <typename object_type>
    static function_reference make_from_raw_pointer(object_type* object_value,
                                                       return_value_type (object_type::*func)(args...)) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>>(
                new __private::member_function_reference<object_type, return_value_type, args...>(object_value, func));
        return to_return;
    }

    template <typename object_type>
    static function_reference make_from_raw_pointer(const object_type* object_value,
                                                       return_value_type (object_type::*func)(args...) const) {
        function_reference to_return;
        to_return._implementation =
            memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>>(
                new __private::member_const_function_reference<object_type, return_value_type, args...>(object_value,
                                                                                                        func));
        return to_return;
    }

    return_value_type operator()(const args&... arguments) const {
        edvar::diagnostics::check(is_bound(), L"attempting to call unbound function reference");
        if constexpr (std::is_void_v<return_value_type>) {
            static_cast<__private::typed_function_reference_impl<return_value_type, args...>*>(
                _implementation.get_unsafe())
                ->invoke(arguments...);
            return;
        } else {
            return static_cast<__private::typed_function_reference_impl<return_value_type, args...>*>(
                       _implementation.get_unsafe())
                ->invoke(arguments...);
        }
    }

    bool is_bound() const { return _implementation.is_valid() && _implementation->is_bound(); }

private:
    function_reference() = default;

    memory::shared_pointer<__private::typed_function_reference_impl<return_value_type, args...>> _implementation;
};

struct function_reference_array_handle {
    uint32 id;
};

template <typename... args> class function_reference_array {
public:
    function_reference_array() = default;
    ~function_reference_array() = default;

    function_reference_array_handle add(const function_reference<void, args...>& func) {
        function_reference_array_handle h = _handle_manager.allocate_handle();
        _handle_manager.handle_map.insert(h, func);
        return h;
    }
    void remove(function_reference_array_handle h) {
        if (_handle_manager.is_handle_valid(h)) {
            _handle_manager.free_handle(h);
        }
    }

    bool is_empty() const { return _handle_manager.is_empty(); }
    bool is_handle_valid(function_reference_array_handle h) const { return _handle_manager.is_handle_valid(h); }

    void invoke_all(const args&&... arguments) {
        for (const pair<function_reference_array_handle, function_reference<void, args...>>& p :
             _handle_manager.handle_map) {
            function_reference<void, args...> function_to_call = p.value();
            if (function_to_call.is_bound()) {
                function_to_call(arguments...);
            }
        }
    }

private:
    struct handle_manager {
        container::map<function_reference_array_handle, function_reference<void, args...>> handle_map;
        uint32 _next_id = 1;
        // probably won't ever reach this many, but just in case
        static constexpr uint32 MAX_ID = 0xFFFFF000;

        function_reference_array_handle allocate_handle() {
            // make sure handle id is unique
            while (handle_map.contains(_next_id) && _next_id != 0 && _next_id <= MAX_ID) {
                ++_next_id;
            }
            edvar::diagnostics::check(_next_id != 0 && _next_id <= MAX_ID,
                                      L"ran out of function reference array handles");
            function_reference_array_handle to_return{_next_id};
            ++_next_id;
            return to_return;
        }
        void free_handle(function_reference_array_handle h) { handle_map.remove(h); }

        bool is_handle_valid(function_reference_array_handle h) const { return handle_map.contains(h); }

        bool is_empty() const { return handle_map.is_empty(); }
    } _handle_manager;
};
} // namespace edvar