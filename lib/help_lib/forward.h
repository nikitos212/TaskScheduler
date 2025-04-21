#pragma once
#include "type_traits.h"

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type&& arg) noexcept {
    static_assert(!is_lvalue_reference<T>::value, "Can not forward an rvalue as an lvalue");
    return static_cast<T&&>(arg);
}