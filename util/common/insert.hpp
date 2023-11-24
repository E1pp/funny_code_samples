#pragma once

#include <type_traits>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class Instance, class Insertible, bool ShouldInsert>
struct Insert;

template <
    class... Args, 
    template <class...> class Template,
    class Insertible,
    bool ShouldInsert
    >
struct Insert<Template<Args...>, Insertible, ShouldInsert>
{
    using Type = std::conditional_t<
        ShouldInsert,
        Template<Insertible, Args...>,
        Template<Args...>
        >;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class T, class Insertible, bool ShouldInsert>
using Insert = typename detail::Insert<T, Insertible, ShouldInsert>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util
