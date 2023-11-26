#pragma once

#include "storage.hpp"

#include <util/common/meta/replace.hpp>

#include <util/common/tag_invoke/typed_cpo.hpp>

#include <wheels/core/assert.hpp>

namespace util::detail {

/////////////////////////////////////////////////////////////////////////

struct EmptyAnyException
    : public std::exception
{ };

/////////////////////////////////////////////////////////////////////////

// Defines tag invoke for our type erasure class
// Derived == AnyObject
template <class Any, TypedCPO CPO, Signature Sig = typename CPO::Signature>
struct ErasedTagInvoker;

template <
    class Any,
    TypedCPO CPO,
    class Ret,
    bool NoExcept,
    class FirstArg,
    class... Args
    >
struct ErasedTagInvoker<Any, CPO, Ret(FirstArg, Args...) noexcept(NoExcept)>
{
    using Replaced = Replace<Any, FirstArg>;

    friend Ret TagInvoke(RawCPO<CPO> cpo, Replaced any_object, Args... args) noexcept(NoExcept)
    {
        static_assert(requires (Replaced any) {
            { any.GetVTable()->template Get<CPO>() };
            { any.GetObjectStorage() };
        });

        auto* vtable = any_object.GetVTable();

        WHEELS_VERIFY(*vtable, "Empty Any!");

        auto* function = vtable->template Get<CPO>();

        return function(cpo, std::forward<Replaced>(any_object).GetObjectStorage(), std::forward<Args>(args)...);
    }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
