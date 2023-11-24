#pragma once

#include <variant>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

template <size_t SizeSBO, size_t AlignSBO>
using StaticStorage = std::aligned_storage_t<SizeSBO, AlignSBO>;

template <size_t SizeSBO, size_t AlignSBO>
using Storage = std::variant<void*>;

/////////////////////////////////////////////////////////////////////////

template <class DecayedConcrete, class StorageType>
struct StorageTraits;

template <class DecayedConcrete, class StorageType>
struct StorageTraits<DecayedConcrete, StorageType&>
{
    static DecayedConcrete& AsConcrete(StorageType& stor) noexcept
    {
        return *static_cast<DecayedConcrete*>(std::get<void*>(stor));
    }
};

template <class DecayedConcrete, class StorageType>
struct StorageTraits<DecayedConcrete, const StorageType&>
{
    static const DecayedConcrete& AsConcrete(const StorageType& stor) noexcept
    {
        return *static_cast<const DecayedConcrete*>(std::get<void*>(stor));
    }
};

template <class DecayedConcrete, class StorageType>
struct StorageTraits<DecayedConcrete, StorageType&&>
{
    static DecayedConcrete&& AsConcrete(StorageType&& stor) noexcept
    {
        return std::move(*static_cast<DecayedConcrete*>(std::get<void*>(stor)));
    }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail
