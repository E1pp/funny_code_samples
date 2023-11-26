#pragma once

#include <type_traits>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class T, template <class...> class Tmplt>
struct IsInstanceImpl
    : public std::false_type
{ };

template <class... Args, template <class...> class Tmplt>
struct IsInstanceImpl<Tmplt<Args...>, Tmplt>
    : public std::true_type
{ };

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class T, template <class...> class Tmplt>
concept IsInstance = detail::IsInstanceImpl<T, Tmplt>::value;

/////////////////////////////////////////////////////////////////////////

} // namespace util