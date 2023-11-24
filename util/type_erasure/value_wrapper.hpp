#pragma once

#include "constructor_concept.hpp"
#include "wrapped_tag_invoker.hpp"

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

// Wraps value
template <EConstructorConcept Concept, class Concrete, class Allocator, TypedCPO... CPOs>
struct ValueWrapper final
    : private ConstructorBase<ValueWrapper<Concept, Concrete, Allocator, CPOs...>, Allocator, Concept>
    , private WrappedTagInvoker<ValueWrapper<Concept, Concrete, Allocator, CPOs...>, CPOs>...
{
    using AllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<ValueWrapper>;
    using Traits = std::allocator_traits<AllocatorType>;
    using ConcreteType = Concrete;

    template <class... Args>
        requires std::constructible_from<Concrete, Args...>
    explicit ValueWrapper(Args&&... args)
        noexcept(std::is_nothrow_constructible<Concrete, Args...>::value)
        : value_(std::forward<Args>(args)...)
    { }

    ConcreteType& Unwrap() const noexcept
    {
        return value_;
    }

    void DoDelete(Allocator& allocator) noexcept
    {
        AllocatorType alloc(std::move(allocator));

        Traits::destroy(alloc, this);

        Traits::deallocate(alloc, this, sizeof(ValueWrapper));

        allocator = std::move(alloc);
    }

    mutable ConcreteType value_;
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail
