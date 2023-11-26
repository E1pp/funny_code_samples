#pragma once

#include <variant>

#include <wheels/core/assert.hpp>
#include <wheels/core/compiler.hpp>
#include <wheels/core/panic.hpp>

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
        sizeof(DecayedConcrete) <= kPaddedSize &&
        alignof(DecayedConcrete) <= kPaddedAlign;

    Storage() noexcept = default;

    // Must be after def ctor or Reset
    constexpr void Set(void* ptr) noexcept
    {
        std::destroy_at(&empty_);
        std::construct_at<void*>(reinterpret_cast<void**>(&storage_), ptr);
    }

    // Must be after def ctor or Reset
    constexpr void Set() noexcept
    {
        std::destroy_at(&empty_);
        std::construct_at(&storage_);
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
    {
        std::destroy_at(&empty_);
    }

    // Must be after Set
    constexpr void Reset() noexcept
    {
        std::destroy_at(&storage_);
        std::construct_at(&empty_);
    }

private:
    union {
        [[no_unique_address]] std::monostate empty_ = {};
        std::aligned_storage_t<kPaddedSize, kPaddedAlign> storage_;
    };
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
