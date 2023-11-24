#pragma once

#include "pack.hpp"

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class T>
concept ValidFunctor = requires { // Exposition-only.
    { T::Value } -> std::same_as<bool>;
};

template <template <class> class Functor, AnyPack P>
struct FoldOrImpl;

template <template <class> class Functor, class... Ts>
struct FoldOrImpl<Functor, Pack<Ts...>>
    : public std::bool_constant<(Functor<Ts>::Value || ...)>
{ };

/////////////////////////////////////////////////////////////////////////

template <template <class> class Functor, AnyPack P>
struct FoldAndImpl;

template <template <class> class Functor, class... Ts>
struct FoldAndImpl<Functor, Pack<Ts...>>
    : public std::bool_constant<(Functor<Ts>::Value && ...)>
{ };

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <template <class> class Functor, AnyPack P>
inline constexpr bool FoldAnd = detail::FoldAndImpl<Functor, P>::value; // NOLINT

/////////////////////////////////////////////////////////////////////////

template <template <class> class Functor, AnyPack P>
inline constexpr bool FoldOr = detail::FoldOrImpl<Functor, P>::value; // NOLINT

/////////////////////////////////////////////////////////////////////////

} // namespace util