#pragma once

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class... T>
struct FirstOfImpl;

template <class First, class... Suffix>
struct FirstOfImpl<First, Suffix...>
{
    using Type = First;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class... Args>
using FirstOf = typename detail::FirstOfImpl<Args...>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util