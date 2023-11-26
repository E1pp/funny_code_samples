#pragma once

#include "storage.hpp"

#include "this.hpp"

#include <util/common/tag_invoke/typed_cpo.hpp>

namespace util::detail {

/////////////////////////////////////////////////////////////////////////

template <class StorageType, class CPO, class Sig = typename CPO::Signature>
    requires TypeErasableSignature<Sig>
class VTableEntry;

template <
    class StorageType,
    class CPO,
    class Ret,
    bool NoExcept,
    class FirstArg,
    class... Args
    >
    requires TypeErasableSignature<Ret(FirstArg, Args...) noexcept(NoExcept)>
class VTableEntry<StorageType, CPO, Ret(FirstArg, Args...) noexcept(NoExcept)>
{
public:
    using Replaced = Replace<StorageType, FirstArg>;
    using Function = Ret(RawCPO<CPO>, Replaced, Args...) noexcept(NoExcept);

    constexpr VTableEntry() noexcept = default;

    constexpr Function* Get() const noexcept
    {
        return function_;
    }

    // If T is concrete type then for some Args
    // CPO(T, Args...) is well-formed
    // VTableEntry->Get gives func(CPO, Storage&, Args...)
    // s.t. it is equal to std::move(cpo)(Concrete&, Args...)
    template <class DecayedConcrete>
    static constexpr VTableEntry Create() noexcept
    {
        return VTableEntry([] (
            RawCPO<CPO> cpo,
            Replaced storage,
            Args... args) 
            noexcept(NoExcept) -> Ret
        {
            return cpo(
                std::forward<Replaced>(storage).template AsConcrete<DecayedConcrete>(),
                std::forward<Args>(args)...);
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

template <class StorageType, TypedCPO... CPOs>
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
        return static_cast<const VTableEntry<StorageType, First<Pack<CPOs...>>>*>(this)->Get() != nullptr;
    }

private:
    constexpr explicit VTable(VTableEntry<StorageType, CPOs>... entries) noexcept
        : VTableEntry<StorageType, CPOs>(entries)...
    { }
};

/////////////////////////////////////////////////////////////////////////

template <bool IsStatic, class Table>
class VTableHolder
{
public:
    constexpr VTableHolder() noexcept = default;

    constexpr VTableHolder(const Table& table) noexcept // NOLINT
        : table_(&table)
    { }

    constexpr const Table& operator*() const noexcept
    {
        return *table_;
    }

    constexpr const Table* operator->() const noexcept
    {
        return table_;
    }

    constexpr explicit operator bool() const noexcept
    {
        if (!table_) {
            return false;
        }

        return static_cast<bool>(table_);
    }

    void Reset() noexcept
    {
        table_ = nullptr;
    }

private:
   const Table* table_ = nullptr;
};

template <class Table>
class VTableHolder<false, Table>
{
public:
    constexpr VTableHolder() noexcept = default;

    constexpr VTableHolder(const Table& table) noexcept // NOLINT
        : table_(table)
    { }

    constexpr Table& operator*() noexcept
    {
        return table_;
    }

    constexpr const Table& operator*() const noexcept
    {
        return table_;
    }

    constexpr Table* operator->() noexcept
    {
        return &table_;
    }

    constexpr const Table* operator->() const noexcept
    {
        return &table_;
    }

    constexpr explicit operator bool() const noexcept
    {
        return static_cast<bool>(table_);
    }

    void Reset() noexcept
    {
        table_ = {};
    }

private:
    Table table_ = {};
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
