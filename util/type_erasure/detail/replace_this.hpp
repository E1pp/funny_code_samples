#pragma once

#include "../this.hpp"

#include <util/common/meta/decay.hpp>

#include <util/common/tag_invoke/signature.hpp>

#include <util/common/type_pack/erase.hpp>
#include <util/common/type_pack/first_of.hpp>
#include <util/common/type_pack/fold.hpp>
#include <util/common/type_pack/replace_all.hpp>
#include <util/common/type_pack/to_pack.hpp>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class What>
struct LikeThis
{
    static constexpr bool Value = std::same_as<Decay<What>, This>; // NOLINT 
};

/////////////////////////////////////////////////////////////////////////

template <class Sig>
concept FirstArgumentLikeThis = 
    Signature<Sig> &&
    detail::LikeThis<First<SigToPack<Sig>>>::Value;

template <class Sig>
concept SingleInsertionOfThis = 
    FirstArgumentLikeThis<Sig> &&
    !FoldOr<detail::LikeThis, PopFront<SigToPack<Sig>>>;

/////////////////////////////////////////////////////////////////////////

template <class With, class What>
struct ReplaceThisImpl
{
    using Type = What;
};

template <class With, class What>
    requires LikeThis<What>::Value
struct ReplaceThisImpl<With, What>
{
    using Type = Replace<With, What>;
};

/////////////////////////////////////////////////////////////////////////



} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class Sig>
concept TypeErasableSignature = detail::SingleInsertionOfThis<Sig>;

/////////////////////////////////////////////////////////////////////////

template <class With, class What>
using ReplaceThis = typename detail::ReplaceThisImpl<With, What>::Type;

template <class With, class... Args>
using Replaced = typename detail::ReplaceThisImpl<With, First<Pack<Args...>>>::Type;

/////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail
