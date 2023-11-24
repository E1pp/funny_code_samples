#pragma once

#include "storage.hpp"

#include <util/common/first_of.hpp>
#include <util/common/typed_cpo.hpp>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

template <class StorageType, TypedCPO CPO, class Sig = typename CPO::Signature>
class VTableEntry;

template <
    class StorageType,
    TypedCPO CPO,
    class Ret,
    bool NoExcept,
    class... Args
    >
class VTableEntry<StorageType, CPO, Ret(Args...) noexcept(NoExcept)>
{
public:
    using Function = Ret(RawCPO<CPO>, StorageType&, Args...) noexcept(NoExcept);

    constexpr VTableEntry() noexcept = default;

    constexpr Function* Get() const noexcept
    {
        return function_;
    }

    // If T is concrete type then for some Args
    // CPO(T, Args...) is well-formed
    // VTableEntry->Get gives func(CPO, Storage&, Args...)
    // s.t. it is equal to std::move(cpo)(Concrete&, Args...)
    template <class Concrete>
    static constexpr VTableEntry Create() noexcept
    {
        return VTableEntry([] (
            RawCPO<CPO> cpo,
            StorageType& erased,
            Args&&... args) 
            noexcept(NoExcept) -> Ret
        {
            return std::move(cpo)(AsConcreteRef<Concrete, StorageType>(erased), std::forward<Args>(args)...);
        });
    }

private:
    constexpr explicit VTableEntry(Function* function) noexcept
        : function_(function)
    { }

    Function* function_ = nullptr;
};

/////////////////////////////////////////////////////////////////////////

template <class... Args>
class VTable;

template <>
class VTable<>
{ };

template <class StorageType, class... CPOs>
class VTable<StorageType, CPOs...>
    : private VTableEntry<StorageType, CPOs>...
{
public:
    constexpr VTable() noexcept = default;

    template <class Concrete>
    static constexpr VTable Create() noexcept
    {
        return VTable{VTableEntry<StorageType, CPOs>::template Create<Concrete>()...};
    }

    template <class CPO>
    constexpr auto Get() const noexcept
    {
        return static_cast<const VTableEntry<StorageType, CPO>*>(this)->Get();
    }

    constexpr explicit operator bool() const noexcept
        requires (sizeof...(CPOs) > 0)
    {
        return static_cast<const VTableEntry<StorageType, FirstOf<CPOs...>>*>(this)->Get() != nullptr;
    }

private:
    constexpr explicit VTable(VTableEntry<StorageType, CPOs>... entries) noexcept
        : VTableEntry<StorageType, CPOs>(entries)...
    { }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail