#pragma once
#include "meta/index_sequence.h"

namespace edvar {
namespace __private {
template <uint32 index, typename storage_type> struct tuple_leaf {
    storage_type value;
};

template <uint32 index, typename... types> struct tuple_impl;

template <uint32 index> struct tuple_impl<index> {};

template <uint32 i, typename head, typename... tail>
struct tuple_impl<i, head, tail...> : tuple_leaf<i, head>, tuple_impl<i + 1, tail...> {
    using tuple_leaf<i, head>::value;
    using tuple_impl<i + 1, tail...>::get;
    template <uint32 j> auto& get() {
        if constexpr (i == j) {
            return value;
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
    void set_data_from_bytes(uint8* bytes) { set_data_from_bytes_helper<0, types...>(*this, bytes); }
    void get_data_as_bytes(uint8* buffer) { get_data_as_bytes_helper<0, types...>(*this, buffer); }

    template <uint32 index> auto& get() { return impl.template get<index>(); }
    constexpr uint32 get_total_byte_size() { return get_total_byte_size_of_pack<0, types...>(); }
    // apply
    template <typename return_type> return_type apply(return_type (*func)(types...)) {
        return apply_implementation(func, meta::make_index_sequence<sizeof...(types)>());
    }

private:
    template <typename return_type, uint32... index_sequence>
    return_type apply_implementation(return_type (*func)(types...), meta::index_sequence<index_sequence...>) {
        return func(((get<index_sequence>()), ...));
    }

    template <uint32 index, typename head, typename... tail>
    constexpr uint32 get_total_byte_size_of_pack(uint32 start = 0) {
        start += sizeof(head);
        if constexpr (sizeof...(tail) == 0) {
            return start;
        } else {
            return start += get_total_byte_size_of_pack(start);
        }
    }
    template <uint32 i, typename current, typename... args>
    void set_data_from_bytes_helper(tuple& in_tuple, uint8* bytes) {
        if constexpr (i >= sizeof...(args)) {
            return;
        }
        in_tuple.get<i>() = reinterpret_cast<current>(bytes);
        bytes += sizeof(current);
        return set_data_from_bytes_helper<i + 1, args...>(in_tuple, bytes);
    }
    template <uint32 i, typename current, typename... args>
    void get_data_as_bytes_helper(const tuple& in_tuple, uint8* buffer) {
        if constexpr (i >= sizeof...(args)) {
            return;
        }
        auto d = in_tuple.get<i>();
        constexpr uint32 s = sizeof(current);
        for (int index = 0; index < s; ++index) {
            buffer[index] = reinterpret_cast<uint8*>(d)[index];
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
    key_type& key() { return this->template get<0>(); }
    value_type& value() { return this->template get<1>(); }
    const key_type& key() const { return this->template get<0>(); }
    const value_type& value() const { return this->template get<1>(); }
};
} // namespace edvar