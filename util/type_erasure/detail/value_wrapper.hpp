// #pragma once

#include "constructor_concept.hpp"
#include "wrapped_tag_invoker.hpp"

namespace util::detail {

/////////////////////////////////////////////////////////////////////////

// Wraps value
template <
    EConstructorConcept Concept,
    class DecayedConcrete,
    class Allocator,
    class StorageT,
    TypedCPO... CPOs
>
struct ValueWrapper final
    : private ConstructorBase<
        ValueWrapper<Concept, DecayedConcrete, Allocator, StorageT, CPOs...>,
        Allocator,
        StorageT,
        Concept
    >
    , private WrappedTagInvoker<
        ValueWrapper<Concept, DecayedConcrete, Allocator, StorageT, CPOs...>,
        CPOs
    >...
{
    using Concrete = DecayedConcrete;
    using StorageType = StorageT;
    using Traits = std::allocator_traits<Allocator>;
    using StorTraits = StorageTraits<ValueWrapper, StorageType>;

    template <class... Args>
        requires std::constructible_from<Concrete, Args...>
    explicit ValueWrapper(Args&&... args)
        noexcept(std::is_nothrow_constructible<Concrete, Args...>::value)
        : value_(std::forward<Args>(args)...)
    { }

    void Delete(Allocator& allocator) & noexcept
    {
        Traits::template destroy<ValueWrapper>(allocator, this);

        if constexpr (!StorTraits::IsStatic) {
            Traits::deallocate(allocator, reinterpret_cast<std::byte*>(this), sizeof(ValueWrapper));
        }
    }

    void Move(StorageType& to, Allocator& allocator) &&
    {
        if constexpr (StorTraits::IsStatic) {
            to.Set(typename StorTraits::Static{});

            Traits::template construct<ValueWrapper>(allocator, &StorTraits::AsConcrete(to), std::move(*this));
            Traits::template destroy<ValueWrapper>(allocator, this);
        } else {
            to.Set(static_cast<void*>(this));
        }
    }

    void MoveRealloc(StorageType& to, Allocator& alloc_to, Allocator&& alloc_from)
    {
        if constexpr (StorTraits::IsStatic) {
            to.Set(typename StorTraits::Static{});
        } else {
            to.Set(Traits::allocate(alloc_to, sizeof(ValueWrapper)));
        }

        Traits::template construct<ValueWrapper>(alloc_to, &StorTraits::AsConcrete(to), std::move(*this));

        Delete(alloc_from);
    }

    void Copy(StorageType& to, Allocator& allocator) const &
    {
        if constexpr (StorTraits::IsStatic) {
            to.Set(typename StorTraits::Static{});
        } else {
            to.Set(Traits::allocate(allocator, sizeof(ValueWrapper)));
        }
        
        Traits::template construct<ValueWrapper>(allocator, &StorTraits::AsConcrete(to), *this);
    }

    Concrete& Unwrap() & noexcept
    {
        return value_;
    }

    const Concrete& Unwrap() const & noexcept
    {
        return value_;
    }

    Concrete&& Unwrap() && noexcept
    {
        return std::move(value_);
    }

    Concrete value_;
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
