#pragma once

#include "storage.hpp"

#include <util/common/tag_invoke.hpp>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

template <class Allocator>
struct DeleterCPO {
    using Signature = void(Allocator&);

    template <class Concrete>
        requires TagInvocable<DeleterCPO, Concrete&, Allocator&>
    void operator()(Concrete& conc, Allocator& allocator) const
        noexcept(NothrowTagInvocable<DeleterCPO, Concrete&, Allocator&>)
    {
        return TagInvoke(*this, conc, allocator);
    }
};

template <class Allocator>
inline constexpr DeleterCPO<Allocator> Deleter = {}; // NOLINT

// template <class Allocator>
// struct DeleterCPO {
//     using Signature = void(Allocator&);

//     template <class Concrete>
//         requires TagInvocable<DeleterCPO, Concrete, Allocator&>
//     void operator()(Concrete&& conc, Allocator& allocator) const
//         noexcept(NothrowTagInvocable<DeleterCPO, Concrete, Allocator&>)
//     {
//         return TagInvoke(*this, std::forward<Concrete>(conc), allocator);
//     }
// };

// template <class Allocator>
// inline constexpr DeleterCPO<Allocator> Deleter = {}; // NOLINT

// /////////////////////////////////////////////////////////////////////////

// inline constexpr struct MoveConstructorCPO {
//     using Signature = void(Storage&); // void(Storage& to)

//     template <class Concrete>
//         requires TagInvocable<MoveConstructorCPO, Concrete, Storage&>
//     void operator()(Concrete&& conc, Storage& to) const
//         noexcept(NothrowTagInvocable<MoveConstructorCPO, Concrete, Storage&>)
//     {
//         return TagInvoke(*this, std::move(conc), to);
//     }
// } MoveConstructor = {}; // NOLINT

// /////////////////////////////////////////////////////////////////////////

// inline constexpr struct MoverAssignerCPO {
//     using Signature = void(Storage&);

//     template <class Concrete>
//         requires TagInvocable<MoverAssignerCPO, Concrete, void*>
//     void operator()(Concrete&& conc, void* to) const
//         noexcept(NothrowTagInvocable<MoverAssignerCPO, Concrete, void*>)
//     {
//         return TagInvoke(*this, std::move(conc), to);
//     }
// } MoverAssigner = {}; // NOLINT

// /////////////////////////////////////////////////////////////////////////

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

// /////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail