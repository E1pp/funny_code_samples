#pragma once

#include "pack.hpp"

#include <util/common/meta/replace.hpp>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class With, class What, AnyPack P>
struct ReplaceAllImpl;

template <class With, class What, class... Ts>
struct ReplaceAllImpl<With, What, Pack<Ts...>>
{
    using Type = Pack<
        std::conditional_t<
            std::same_as<Decay<Ts>, Decay<What>>,
            Replace<With, Ts>,
            Ts
        >...
    >;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class With, class What, AnyPack P>
using ReplaceAll = typename detail::ReplaceAllImpl<With, What, P>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util
