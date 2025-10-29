#pragma once
#include "type_traits.h"

template <typename T>
typename remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(arg);
}