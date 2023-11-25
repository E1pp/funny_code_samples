#pragma once

#include "constructor_cpo.hpp"
#include "vtable.hpp"

#include <util/common/type_pack/insert.hpp>

#include <memory>

namespace util::type_erasure {

/////////////////////////////////////////////////////////////////////////

enum class EConstructorConcept : int
{
    Pinned                   = 0,
    MoveConstructible        = (1 << 0),
    NothrowMoveConstructible = (1 << 1) | MoveConstructible,
    CopyConstructible        = (1 << 2) | MoveConstructible,
    NothrowCopyConstructible = (1 << 3) | CopyConstructible | NothrowMoveConstructible,
};

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <EConstructorConcept Concept>
concept AddMoveConstructor = (static_cast<int>(Concept) & static_cast<int>(EConstructorConcept::MoveConstructible)) == 1;

template <EConstructorConcept Concept>
concept MoveNoExcept = 
    AddMoveConstructor<Concept> &&
    (((static_cast<int>(Concept) & static_cast<int>(EConstructorConcept::NothrowMoveConstructible)) >> 1) == 1);

template <EConstructorConcept Concept>
concept AddCopyConstructor = ((static_cast<int>(Concept) & static_cast<int>(EConstructorConcept::CopyConstructible)) >> 2) == 1;

template <EConstructorConcept Concept>
concept CopyNoExcept = 
    AddCopyConstructor<Concept> &&
    (((static_cast<int>(Concept) & static_cast<int>(EConstructorConcept::NothrowMoveConstructible)) >> 3) == 1);

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <EConstructorConcept Concept, class Allocator, class StorageType, class... CPOs>
struct InjectConstructors
{
    using Type = 
    PushFront<
        PushFront<
            PushFront<
                PushFront<Pack<CPOs...>,
                CopyCPO<Allocator, StorageType>,
                AddCopyConstructor<Concept>
                >,
                MoveCPO<Allocator, StorageType>,
                AddMoveConstructor<Concept>
            >,
            MoveReallocCPO<Allocator, StorageType>,
            AddMoveConstructor<Concept>
        >,
        DeleterCPO<Allocator>,
        true
    >;
};

/////////////////////////////////////////////////////////////////////////

template <AnyPack P, class StorageType>
struct AugmentedVTableImpl;

template <class... PArgs, class StorageType>
struct AugmentedVTableImpl<Pack<PArgs...>, StorageType>
{
    using Type = VTable<StorageType, PArgs...>;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <EConstructorConcept Concept, class Allocator, class StorageType, class... CPOs>
using AugmentedVTable = 
    typename detail::AugmentedVTableImpl<
        typename detail::InjectConstructors<Concept, Allocator, StorageType, CPOs...>::Type, 
        StorageType
    >::Type;

/////////////////////////////////////////////////////////////////////////

template <class Wrapper, class Allocator, class StorageType, EConstructorConcept Concept>
struct ConstructorBase
{
    // TagInvocable<DeleterCPO, Concrete&, Allocator&>
    friend void TagInvoke(DeleterCPO<Allocator>, Wrapper& wrapper, Allocator& alloc) noexcept
    {
        wrapper.Delete(alloc);
    }

    // TagInvocable<MoveConstructorCPO, DecayedConcrete&&, StorageType&, Allocator&>
    friend void TagInvoke(
        MoveCPO<Allocator, StorageType>,
        Wrapper&& from,
        StorageType& to,
        Allocator& allocator)
            noexcept(MoveNoExcept<Concept>)
                requires(AddMoveConstructor<Concept>)
    {
        std::move(from).Move(to, allocator);
    }

    // TagInvocable<MoveReallocCPO, DecayedConcrete&&, StorageType&, Allocator&, Allocator&&>
    friend void TagInvoke(
        MoveReallocCPO<Allocator, StorageType>,
        Wrapper&& from,
        StorageType& to,
        Allocator& allocator_to,
        Allocator&& allocator_from)
            noexcept(MoveNoExcept<Concept>)
                requires(AddMoveConstructor<Concept>)
    {
        std::move(from).MoveRealloc(to, allocator_to, std::move(allocator_from));
    }

    // TagInvocable<CopyCPO, const DecayedConcrete&, StorageType&, Allocator&>
    friend void TagInvoke(
        CopyCPO<Allocator, StorageType>,
        const Wrapper& from,
        StorageType& to,
        Allocator& allocator)
            noexcept(CopyNoExcept<Concept>)
                requires(AddCopyConstructor<Concept>)
    {
        from.Copy(to, allocator);
    }
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure
