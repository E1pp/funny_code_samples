// #pragma once

// #include "storage.hpp"

// #include <util/common/typed_cpo.hpp>

// namespace util::type_erasure::detail {

// /////////////////////////////////////////////////////////////////////////

// // Defines tag invoke for our type erasure class
// // Derived == AnyObject
// template <class Derived, TypedCPO CPO, Signature Sig = typename CPO::Signature>
// struct ErasedTagInvoker;

// template <
//     class Derived,
//     TypedCPO CPO,
//     class Ret,
//     bool NoExcept,
//     class... Args
//     >
// struct ErasedTagInvoker<Derived, CPO, Ret(Args...) noexcept(NoExcept)>
// {
//     friend Ret TagInvoke(RawCPO<CPO> cpo, Derived& drv, Args&&... args) noexcept(NoExcept)
//     {
//         auto& storage = drv.GetObjectStorage();
//         auto* function = drv.GetVTable().template Get<CPO>();

//         return function(std::move(cpo), storage, std::forward<Args>(args)...);
//     }
// };

// /////////////////////////////////////////////////////////////////////////

// } // namespace util::type_erasure::detail
