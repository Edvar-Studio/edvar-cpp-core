#pragma once

namespace edvar {
// std::forward like implementation
template <typename T> constexpr T&& forward(typename edvar::meta::remove_reference<T>& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <typename T> constexpr T&& move(T& v) noexcept { return static_cast<T&&>(v); }


template<typename T> void swap(T& a, T& b) noexcept {
    T temp = edvar::move(a);
    a = edvar::move(b);
    b = edvar::move(temp);
}
} // namespace edvar