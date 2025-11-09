#pragma once
#include "meta/index_sequence.h"

namespace edvar {
namespace __private {
template <uint32_t index, typename storage_type> struct tuple_leaf {
    storage_type value;
};

template <uint32_t index, typename... types> struct tuple_impl;

template <uint32_t index> struct tuple_impl<index> {};

template <uint32_t i, typename head, typename... tail>
struct tuple_impl<i, head, tail...> : tuple_leaf<i, head>, tuple_impl<i + 1, tail...> {
    template <uint32_t j> auto& get() {
        if constexpr (i == j) {
            return this->value;
        } else {
            return tuple_impl<i + 1, tail...>::template get<j>();
        }
    }
    template <uint32_t j> const auto& get() const {
        if constexpr (i == j) {
            return this->value;
        } else {
            return tuple_impl<i + 1, tail...>::template get<j>();
        }
    }
};
} // namespace __private

template <typename... types> class tuple {
public:
    template <typename... args> static tuple make_tuple(args... a) { return tuple<args...>(a...); }
    tuple() = default;
    tuple(types... a) : impl{a...} {}
    void set_data_from_bytes(uint8_t* bytes) { set_data_from_bytes_helper<0, types...>(*this, bytes); }
    void get_data_as_bytes(uint8_t* buffer) { get_data_as_bytes_helper<0, types...>(*this, buffer); }

    template <uint32_t index> auto& get() { return impl.template get<index>(); }
    template <uint32_t index> const auto& get() const { return impl.template get<index>(); }
    constexpr uint32_t get_total_byte_size() { return get_total_byte_size_of_pack<0, types...>(); }
    // apply
    template <typename return_type> return_type apply(return_type (*func)(types...)) {
        return apply_implementation(func, meta::make_index_sequence<sizeof...(types)>());
    }

private:
    template <typename return_type, uint32_t... index_sequence>
    return_type apply_implementation(return_type (*func)(types...), meta::index_sequence<index_sequence...>) {
        return func(((get<index_sequence>()), ...));
    }

    template <uint32_t index, typename head, typename... tail>
    constexpr uint32_t get_total_byte_size_of_pack(uint32_t start = 0) {
        start += sizeof(head);
        if constexpr (sizeof...(tail) == 0) {
            return start;
        } else {
            return start += get_total_byte_size_of_pack(start);
        }
    }
    template <uint32_t i, typename current, typename... args>
    void set_data_from_bytes_helper(tuple& in_tuple, uint8_t* bytes) {
        if constexpr (i >= sizeof...(args)) {
            return;
        }
        in_tuple.get<i>() = reinterpret_cast<current>(bytes);
        bytes += sizeof(current);
        return set_data_from_bytes_helper<i + 1, args...>(in_tuple, bytes);
    }
    template <uint32_t i, typename current, typename... args>
    void get_data_as_bytes_helper(const tuple& in_tuple, uint8_t* buffer) {
        if constexpr (i >= sizeof...(args)) {
            return;
        }
        auto d = in_tuple.get<i>();
        constexpr uint32_t s = sizeof(current);
        for (int index = 0; index < s; ++index) {
            buffer[index] = reinterpret_cast<uint8_t*>(d)[index];
        }
        buffer += s;
        return get_data_as_bytes_helper<i + 1, args...>(in_tuple, buffer);
    }
    __private::tuple_impl<0, types...> impl;
};

template <typename key_type, typename value_type> class pair : public tuple<key_type, value_type> {
public:
    pair() = default;
    pair(const key_type& k, const value_type& v) {
        this->template get<0>() = k;
        this->template get<1>() = v;
    }
    key_type& key() { return (this->template get<0>()); }
    value_type& value() { return (this->template get<1>()); }
    const key_type& key() const { return (this->template get<0>()); }
    const value_type& value() const { return (this->template get<1>()); }
};


template <typename... Args> inline auto forward_as_tuple(Args&&... args) -> edvar::tuple<Args&&...> {
    return edvar::tuple<Args&&...>(std::forward<Args>(args)...);
}
} // namespace edvar

