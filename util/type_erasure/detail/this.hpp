#pragma once

#include <util/common/meta/decay.hpp>

#include <util/common/tag_invoke/signature.hpp>

#include <util/common/type_pack/erase.hpp>
#include <util/common/type_pack/first_of.hpp>
#include <util/common/type_pack/fold.hpp>
#include <util/common/type_pack/replace_all.hpp>
#include <util/common/type_pack/to_pack.hpp>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

// This is a placeholder which should be placed in the beggining
// of a signature with proper const/ref qualificators.
struct This
{ };

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

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class Sig>
concept TypeErasableSignature = detail::SingleInsertionOfThis<Sig>;

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail
