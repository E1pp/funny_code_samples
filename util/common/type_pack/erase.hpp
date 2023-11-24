#pragma once

#include "pack.hpp"

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <AnyPack P>
struct PopFrontImpl;

template <class T, class... Ts>
struct PopFrontImpl<Pack<T, Ts...>>
{
    using Type = Pack<Ts...>;
};

/////////////////////////////////////////////////////////////////////////

template <class... Ts>
struct PopBackImpl;

template <class FirstUnchecked, class... UncheckedSuffix, class... Checked>
struct PopBackImpl<Pack<FirstUnchecked, UncheckedSuffix...>, Checked...>
{
    using Type = typename PopBackImpl<Pack<UncheckedSuffix...>, Checked..., FirstUnchecked>::Type;
};

template <class Last, class... Checked>
struct PopBackImpl<Pack<Last>, Checked...>
{
    using Type = Pack<Checked...>;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <AnyPack P>
using PopFront = typename detail::PopFrontImpl<P>::Type;

/////////////////////////////////////////////////////////////////////////

template <AnyPack P>
using PopBack = typename detail::PopBackImpl<P>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util