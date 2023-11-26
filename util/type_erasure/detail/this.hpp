#pragma once

#include <util/common/meta/decay.hpp>

#include <util/common/tag_invoke/tag_invoke.hpp>
#include <util/common/tag_invoke/typed_cpo.hpp>

#include <util/common/type_pack/erase.hpp>
#include <util/common/type_pack/first_of.hpp>
#include <util/common/type_pack/fold.hpp>
#include <util/common/type_pack/replace_all.hpp>
#include <util/common/type_pack/to_pack.hpp>

namespace util::detail {

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

/////////////////////////////////////////////////////////////////////////

template <class Implementor, TypedCPO CPO, Signature Sig = typename CPO::Signature>
struct TypeErasableCPO
    : public std::false_type
{ };

template <
    class Implementor,
    TypedCPO CPO,
    class Ret,
    class FirstArg,
    bool NoExcept,
    class... Args
    >
struct TypeErasableCPO<Implementor, CPO, Ret(FirstArg, Args...) noexcept(NoExcept)>
    : public std::bool_constant<
        NoExcept ? 
        NothrowTagInvocable<RawCPO<CPO>, Replace<Implementor, FirstArg>, Args...> :
        TagInvocable<RawCPO<CPO>, Replace<Implementor, FirstArg>, Args...>>
{ };

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class Sig>
concept TypeErasableSignature = detail::SingleInsertionOfThis<Sig>;

/////////////////////////////////////////////////////////////////////////

template <class Implementor, class... CPOs>
concept TypeErasable = (detail::TypeErasableCPO<Implementor, CPOs>::value && ...);

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
