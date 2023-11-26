#pragma once

#include "signature.hpp"

#include <concepts>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

/////////////////////////////////////////////////////////////////////////

template <std::default_initializable CPO, Signature Sig>
struct Overload
{
    using Signature = Sig;

    constexpr Overload() noexcept = default;
    constexpr explicit Overload(CPO) noexcept { };
};

/////////////////////////////////////////////////////////////////////////

template <class CPO>
struct RawCPOImpl
{
    using Type = CPO;
};

template <class CPO, class Sig>
struct RawCPOImpl<Overload<CPO, Sig>>
{
    using Type = CPO;
};

/////////////////////////////////////////////////////////////////////////

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class T>
concept TypedCPO =
    std::default_initializable<T> &&
    requires {
        typename T::Signature;
    } &&
    util::Signature<typename T::Signature>;

/////////////////////////////////////////////////////////////////////////

using detail::Overload;

/////////////////////////////////////////////////////////////////////////

template <class CPO>
using RawCPO = typename detail::RawCPOImpl<CPO>::Type;

/////////////////////////////////////////////////////////////////////////

template <class Tag, class Ret, class... Args>
concept ValidThrowingTagInvokeSignature = requires (Tag&& tag, Args&&... args)
{
    { TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...) } -> std::same_as<Ret>;
};

template <class Tag, class Ret, class... Args>
concept ValidNothrowTagInvokeSignature = requires (Tag&& tag, Args&&... args)
{
    { TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...) } noexcept -> std::same_as<Ret>;
};

template <class Tag, class Ret, bool NoExcept, class... Args>
concept ValidTagInvokeSignature = 
    (NoExcept ?
    ValidNothrowTagInvokeSignature<Tag, Ret, Args...> :
    ValidThrowingTagInvokeSignature<Tag, Ret, Args...>);

/////////////////////////////////////////////////////////////////////////

} // namespace util
