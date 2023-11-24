#pragma once

#include <utility>
#include <type_traits>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class Sig>
struct SignatureImpl
    : public std::false_type
{ };

template <class Ret, bool NoExcept, class... Args>
struct SignatureImpl<Ret(Args...) noexcept(NoExcept)>
    : public std::true_type
{ };

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class Sig>
concept Signature = detail::SignatureImpl<Sig>::value;

/////////////////////////////////////////////////////////////////////////

} // namespace util