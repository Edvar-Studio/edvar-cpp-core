#pragma once

namespace edvar::meta {

template <uint32... sequence> struct index_sequence {};
template <uint32 current, uint32... sequence> auto make_index_sequence_implementation() {
    if constexpr (current == 0)
        return index_sequence<sequence...>();
    else
        return make_index_sequence_implementation<current - 1, current - 1, sequence...>();
}

template <uint32 length> using make_index_sequence = decltype(make_index_sequence_implementation<length>());

} // namespace edvar::meta