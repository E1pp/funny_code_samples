#pragma once

#include "decay.hpp"

#include <concepts>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class With, class What>
struct ReplaceImpl
{
    using Type = With;
};

template <class With, class What>
struct ReplaceImpl<With, const What>
{
    using Type = const With;
};

template <class With, class What>
struct ReplaceImpl<With, What&>
{
    using Type = With&;
};

template <class With, class What>
struct ReplaceImpl<With, const What&>
{
    using Type = const With&;
};

template <class With, class What>
struct ReplaceImpl<With, What*>
{
    using Type = With*;
};

template <class With, class What>
struct ReplaceImpl<With, const What*>
{
    using Type = const With*;
};

template <class With, class What>
struct ReplaceImpl<With, What&&>
{
    using Type = With&&;
};

template <class With, class Ret, bool NoExcept, class... Args>
struct ReplaceImpl<With, Ret (*)(Args...) noexcept(NoExcept)>
{
    using Type = With;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class With, class What>
using Replace = typename detail::ReplaceImpl<Decay<With>, What>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util
