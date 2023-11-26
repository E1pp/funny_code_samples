#pragma once

#include <util/common/meta/replace.hpp>

#include <util/common/tag_invoke/typed_cpo.hpp>

namespace util::detail {

/////////////////////////////////////////////////////////////////////////

template <class Derived, TypedCPO CPO, Signature Sig = typename CPO::Signature>
struct WrappedTagInvoker;

template <
    class Derived,
    TypedCPO CPO,
    class Ret,
    bool NoExcept,
    class FirstArg,
    class... Args
    >
struct WrappedTagInvoker<Derived, CPO, Ret(FirstArg, Args...) noexcept(NoExcept)>
{
    using Replaced = Replace<Derived, FirstArg>;

    friend Ret TagInvoke(RawCPO<CPO> cpo, Replaced wrapper, Args... args) noexcept(NoExcept)
    {
        return cpo(std::forward<Replaced>(wrapper).Unwrap(), std::forward<Args>(args)...);
    }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
