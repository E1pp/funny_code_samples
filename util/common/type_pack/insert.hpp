#pragma once

#include "pack.hpp"

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class Where, class What, bool Should>
struct PushFrontImpl;

template <class... Ts, class What, bool Should>
struct PushFrontImpl<Pack<Ts...>, What, Should>
{
    using Type = Pack<What, Ts...>;
};

template <class... Ts, class What>
struct PushFrontImpl<Pack<Ts...>, What, false>
{
    using Type = Pack<Ts...>;
};

/////////////////////////////////////////////////////////////////////////

template <class Where, class What, bool Should>
struct PushBackImpl;

template <class... Ts, class What, bool Should>
struct PushBackImpl<Pack<Ts...>, What, Should>
{
    using Type = Pack<Ts..., What>;
};

template <class... Ts, class What>
struct PushBackImpl<Pack<Ts...>, What, false>
{
    using Type = Pack<Ts...>;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class Where, class What, bool Should>
using PushFront = typename detail::PushFrontImpl<Where, What, Should>::Type;

/////////////////////////////////////////////////////////////////////////

template <class Where, class What, bool Should>
using PushBack = typename detail::PushBackImpl<Where, What, Should>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util
