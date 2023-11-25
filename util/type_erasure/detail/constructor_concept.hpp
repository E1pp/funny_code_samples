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
            Pack<CPOs...>,
            MoveConstructorCPO<Allocator, StorageType>,
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
        wrapper.DeleteWith(alloc);
    }

    // TagInvocable<MoveConstructorCPO, DecayedConcrete&&, StorageType&, Allocator&>
    friend void TagInvoke(
        MoveConstructorCPO<Allocator, StorageType>,
        Wrapper&& from,
        StorageType& to,
        Allocator& allocator)
            noexcept(MoveNoExcept<Concept>)
                requires(AddMoveConstructor<Concept>)
    {
        std::move(from).MoveConstruct(to, allocator);
    }
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure
