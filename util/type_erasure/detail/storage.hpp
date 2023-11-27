#pragma once

#include <memory>
#include <variant>

namespace util::detail {

/////////////////////////////////////////////////////////////////////////

template <size_t SizeSBO, size_t AlignSBO>
class Storage
{
public:
    static constexpr size_t kPaddedSize = SizeSBO < sizeof(void*) ? sizeof(void*) : SizeSBO;
    static constexpr size_t kPaddedAlign = AlignSBO < alignof(void*) ? alignof(void*) : AlignSBO;

    template <class DecayedConcrete>
    static constexpr bool IsStatic = // NOLINT
        sizeof(DecayedConcrete) <= SizeSBO &&
        alignof(DecayedConcrete) <= AlignSBO;

    Storage() noexcept = default;

    // Must be after def ctor or Reset
    constexpr void Set(void* ptr) noexcept
    {
        std::construct_at<void*>(reinterpret_cast<void**>(&storage_), ptr);
    }

    // Must be after def ctor or Reset
    constexpr void Set() noexcept
    {
        std::construct_at(storage_);
    }

    template <class DecayedConcrete>
    constexpr DecayedConcrete& AsConcrete() & noexcept
    {
        using Ptr = DecayedConcrete*;

        if constexpr (IsStatic<DecayedConcrete>) {
            return *std::launder(reinterpret_cast<Ptr>(&storage_));
        } else {
            return *static_cast<Ptr>(*std::launder(reinterpret_cast<void**>(&storage_)));
        }
    }

    template <class DecayedConcrete>
    constexpr const DecayedConcrete& AsConcrete() const & noexcept
    {
        using Ptr = const DecayedConcrete*;

        if constexpr (IsStatic<DecayedConcrete>) {
            return *std::launder(reinterpret_cast<Ptr>(&storage_));
        } else {
            return *static_cast<Ptr>(*std::launder(reinterpret_cast<void const* const*>(&storage_)));
        }
    }

    template <class DecayedConcrete>
    constexpr DecayedConcrete&& AsConcrete() && noexcept
    {
        using Ptr = DecayedConcrete*;

        if constexpr (IsStatic<DecayedConcrete>) {
            return std::move(*std::launder(reinterpret_cast<Ptr>(&storage_)));
        } else {
            return std::move(*static_cast<Ptr>(*std::launder(reinterpret_cast<void**>(&storage_))));
        }
    }

    // Must be after def ctor or Reset
    ~Storage() noexcept
    { }

    // Must be after Set
    constexpr void Reset() noexcept
    {
        std::destroy_at(&storage_);
    }

private:
    alignas(kPaddedAlign) std::byte storage_[kPaddedSize];
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
