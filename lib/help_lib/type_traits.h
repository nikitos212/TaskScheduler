#pragma once

template<typename T>
struct type_identity {
    using type = T;
};

template<typename T> 
struct remove_reference : type_identity<T> {};

template<typename T> 
struct remove_reference<T&> : type_identity<T> {};

template<typename T> 
struct remove_reference<T&&> : type_identity<T> {};

template<typename T> 
struct remove_const : type_identity<T> {};

template<typename T> 
struct remove_const<const T> : type_identity<T> {};

template<typename T> 
struct remove_const<const T&> : type_identity<T> {};

template<typename T> 
struct remove_const<const T&&> : type_identity<T> {};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<typename T>
struct is_lvalue_reference {
    static constexpr bool value = false;
};

template<typename T>
struct is_lvalue_reference<T&> {
    static constexpr bool value = true;
};

template<typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template<typename T>
struct is_rvalue_reference {
    static constexpr bool value = false;
};

template<typename T>
struct is_rvalue_reference<T&&> {
    static constexpr bool value = true;
};

template<typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template<typename T, typename U>
struct is_same {
    static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static constexpr bool value = true;
};

template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;