#pragma once

#include <util/type_erasure/detail/this.hpp>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class T, TypeErasableSignature Sig>
struct FragmentImpl;

template <
    class T,
    class Ret,
    bool NoExcept,
    class FirstArg,
    class... Args>
struct FragmentImpl<T, Ret(FirstArg, Args...) noexcept(NoExcept)>
{
    using Signature = Ret(FirstArg, Args...) noexcept(NoExcept);

    template <class Concrete>
        requires 
            (std::same_as<Concrete, Replace<Concrete, FirstArg>>) &&
            (TagInvocable<FragmentImpl, Concrete, Args...>)
    constexpr Ret operator()(Concrete&& conc, Args... args) const
        noexcept(NothrowTagInvocable<FragmentImpl, Concrete, Args...>)
    {
        return TagInvoke(
            *this,
            std::forward<Concrete>(conc),
            std::forward<Args>(args)...);
    }
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <class Interface, detail::TypeErasableSignature Sig>
using Fragment = detail::FragmentImpl<Interface, Sig>;

/////////////////////////////////////////////////////////////////////////

} // namespace util
