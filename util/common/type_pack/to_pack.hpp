#pragma once

#include "pack.hpp"

#include <util/common/tag_invoke/signature.hpp>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class Sig>
struct SigToPackImpl;

template <class Ret, bool NoExcept, class... Args>
struct SigToPackImpl<Ret(Args...) noexcept(NoExcept)>
{
    using Type = Pack<Args...>;
} ;

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <Signature Sig>
using SigToPack = typename detail::SigToPackImpl<Sig>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util
