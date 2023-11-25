#pragma once

#include "this.hpp"

#include <util/common/tag_invoke/tag_invoke.hpp>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

template <class Allocator>
struct DeleterCPO {
    using Signature = void(This&, Allocator&);

    template <class DecayedConcrete>
        requires TagInvocable<DeleterCPO, DecayedConcrete&, Allocator&>
    void operator()(DecayedConcrete& what, Allocator& allocator) const
        noexcept(NothrowTagInvocable<DeleterCPO, DecayedConcrete&, Allocator&>)
    {
        return TagInvoke(*this, what, allocator);
    }
};

template <class Allocator>
inline constexpr DeleterCPO<Allocator> Deleter = {}; // NOLINT

/////////////////////////////////////////////////////////////////////////

template <class Allocator, class StorageType>
struct MoveCPO {
    using Signature = void(This&&, StorageType&, Allocator&);

    template <class DecayedConcrete>
        requires TagInvocable<MoveCPO, DecayedConcrete&&, StorageType&, Allocator&>
    void operator()(DecayedConcrete&& from, StorageType& to, Allocator& allocator) const
        noexcept(NothrowTagInvocable<MoveCPO, DecayedConcrete&&, StorageType&, Allocator&>)
    {
        return TagInvoke(*this, std::move(from), to, allocator);
    }
};

template <class Allocator, class StorageType>
inline constexpr MoveCPO<Allocator, StorageType> Mover = {}; // NOLINT

/////////////////////////////////////////////////////////////////////////

template <class Allocator, class StorageType>
struct MoveReallocCPO {
    using Signature = void(This&&, StorageType&, Allocator&, Allocator&&);

    template <class DecayedConcrete>
        requires TagInvocable<MoveReallocCPO, DecayedConcrete&&, StorageType&, Allocator&, Allocator&&>
    void operator()(DecayedConcrete&& from, StorageType& to, Allocator& allocator_to, Allocator&& allocator_from) const
        noexcept(NothrowTagInvocable<MoveReallocCPO, DecayedConcrete&&, StorageType&, Allocator&, Allocator&&>)
    {
        return TagInvoke(*this, std::move(from), to, allocator_to, std::move(allocator_from));
    }
};

template <class Allocator, class StorageType>
inline constexpr MoveReallocCPO<Allocator, StorageType> ReallocMover = {}; // NOLINT

/////////////////////////////////////////////////////////////////////////

template <class Allocator, class StorageType>
struct CopyCPO {
    using Signature = void(const This&, StorageType&, Allocator&);

    template <class DecayedConcrete>
        requires TagInvocable<CopyCPO, const DecayedConcrete&, StorageType&, Allocator&>
    void operator()(const DecayedConcrete& from, StorageType& to, Allocator& allocator) const
        noexcept(NothrowTagInvocable<CopyCPO, const DecayedConcrete&, StorageType&, Allocator&>)
    {
        return TagInvoke(*this, from, to, allocator);
    }
};

template <class Allocator, class StorageType>
inline constexpr CopyCPO<Allocator, StorageType> Copier = {}; // NOLINT

/////////////////////////////////////////////////////////////////////////

// inline constexpr struct CopyConstructorCPO {
//     using Signature = void(Storage&);

//     template <class Concrete>
//         requires TagInvocable<CopyConstructorCPO, Concrete, Storage&>
//     void operator()(const Concrete& conc, Storage& to) const
//         noexcept(NothrowTagInvocable<CopyConstructorCPO, Concrete, Storage&>)
//     {
//         return TagInvoke(*this, conc, to);
//     }
// } CopyConstructor = {}; // NOLINT

// /////////////////////////////////////////////////////////////////////////

// inline constexpr struct CopyAssignerCPO {
//     using Signature = void(Storage&);

//     template <class Concrete>
//         requires TagInvocable<CopyAssignerCPO, Concrete, void*>
//     void operator()(const Concrete& conc, void* to) const
//         noexcept(NothrowTagInvocable<CopyAssignerCPO, Concrete, void*>)
//     {
//         return TagInvoke(*this, conc, to);
//     }
// } CopyAssigner = {}; // NOLINT

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail