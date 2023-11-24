// #pragma once

// #include <util/common/typed_cpo.hpp>

// namespace util::type_erasure::detail {

// /////////////////////////////////////////////////////////////////////////

// template <class Derived, TypedCPO CPO, Signature Sig = typename CPO::Signature>
// struct WrappedTagInvoker;

// template <
//     class Derived,
//     TypedCPO CPO,
//     class Ret,
//     bool NoExcept,
//     class... Args
//     >
// struct WrappedTagInvoker<Derived, CPO, Ret(Args...) noexcept(NoExcept)>
// {
//     friend Ret TagInvoke(RawCPO<CPO> cpo, Derived& wrapper, Args&&... args) noexcept(NoExcept)
//     {
//         static_assert(requires (Derived& wrapper) {
//             typename Derived::ConcreteType;

//             { wrapper.Unwrap() } -> std::same_as<typename Derived::ConcreteType&>;
//         });

//         return std::move(cpo)(wrapper.Unwrap(), std::forward<Args>(args)...);
//     }
// };

// /////////////////////////////////////////////////////////////////////////

// } // namespace util::type_erasure::detail
