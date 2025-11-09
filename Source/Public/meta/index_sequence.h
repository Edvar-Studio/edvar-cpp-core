#pragma once

namespace edvar::meta {

template <uint32_t... sequence> struct index_sequence {};
template <uint32_t current, uint32_t... sequence> auto make_index_sequence_implementation() {
    if constexpr (current == 0)
        return index_sequence<sequence...>();
    else
        return make_index_sequence_implementation<current - 1, current - 1, sequence...>();
}

template <uint32_t length> using make_index_sequence = decltype(make_index_sequence_implementation<length>());

} // namespace edvar::meta