#pragma once

#include "pack.hpp"

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <AnyPack P>
struct FirstImpl;

template <class First, class... Suffix>
struct FirstImpl<Pack<First, Suffix...>>
{
    using Type = First;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <AnyPack P>
using First = typename detail::FirstImpl<P>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util