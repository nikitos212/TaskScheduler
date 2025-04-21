#pragma once
#include "forward.h"

namespace detail {
template<typename F, typename... Args>
constexpr auto invoke_impl(F&& f, Args&&... args) 
    noexcept(noexcept(forward<F>(f)(forward<Args>(args)...)))
    -> decltype(forward<F>(f)(forward<Args>(args)...))
{
    return forward<F>(f)(forward<Args>(args)...);
}

template<typename Base, typename T, typename Derived, typename... Args>
constexpr auto invoke_impl(T Base::*pmf, Derived&& ref, Args&&... args)
    noexcept(noexcept((forward<Derived>(ref).*pmf)(forward<Args>(args)...)))
    -> decltype((forward<Derived>(ref).*pmf)(forward<Args>(args)...))
{
    return (forward<Derived>(ref).*pmf)(forward<Args>(args)...);
}

template<typename Base, typename T, typename Derived>
constexpr auto invoke_impl(T Base::*pmd, Derived&& ref)
    noexcept(noexcept(forward<Derived>(ref).*pmd))
    -> decltype(forward<Derived>(ref).*pmd)
{
    return forward<Derived>(ref).*pmd;
}
}

template<typename F, typename... Args>
constexpr auto invoke(F&& f, Args&&... args)
    noexcept(noexcept(detail::invoke_impl(forward<F>(f), forward<Args>(args)...)))
    -> decltype(detail::invoke_impl(forward<F>(f), forward<Args>(args)...))
{
    return detail::invoke_impl(forward<F>(f), forward<Args>(args)...);
}