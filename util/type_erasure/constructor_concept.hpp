#pragma once

#include "constructor_cpo.hpp"

#include <util/common/insert.hpp>

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

template <class Allocator, class StorageType, class VTableLike, EConstructorConcept Concept>
using VTableWithConstructors = 
    Insert<
        Insert<
            VTableLike,
            DeleterCPO<Allocator>,
            true
        >,
        StorageType,
        true
    >;

/////////////////////////////////////////////////////////////////////////

template <class Derived, class Allocator, EConstructorConcept Concept>
struct ConstructorBase
{
    friend void TagInvoke(DeleterCPO<Allocator>, Derived& wrapper, Allocator& allocator) noexcept
    {
        wrapper.DoDelete(allocator);
    }
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure
