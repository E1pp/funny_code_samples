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
    using StaticStorage = std::aligned_storage_t<SizeSBO, AlignSBO>;

    union {
        [[no_unique_address]] std::monostate empty = {};
        void* data_ptr;
        StaticStorage inplace_storage;
    };

    template <class DecayedConcrete>
    static constexpr bool IsStatic()
    {
        return sizeof(DecayedConcrete) <= SizeSBO;
    }

    Storage() noexcept = default;

    Storage(const Storage& that) noexcept
        : Storage()
    {
        std::destroy_at(&empty);
        index_ = that.index_;

        switch (index_) {
            case 0:
                std::construct_at(&empty);
                break;
            case 1:
                std::construct_at(&data_ptr, that.data_ptr);
                break;
            case 2:
                std::construct_at(&inplace_storage, that.inplace_storage);
                break;
            default:
                WHEELS_UNREACHABLE();
        }
    }

    Storage(Storage&& that) noexcept
        : Storage()
    {
        std::destroy_at(&empty);
        index_ = that.index_;

        switch (index_) {
            case 0:
                std::construct_at(&empty);
                break;
            case 1:
                std::construct_at(&data_ptr, that.data_ptr);
                break;
            case 2:
                std::construct_at(&inplace_storage, std::move(that.inplace_storage));
                break;
            default:
                WHEELS_UNREACHABLE();
        }

        that.HardReset();
        that.index_ = 0;
        std::construct_at(&that.empty);

        return *this;
    }

    Storage& operator=(const Storage&) = delete;
    Storage& operator=(Storage&&) = delete;

    void Set(void* ptr) noexcept // NOLINT
    {
        HardReset();
        std::construct_at(&data_ptr, ptr);
        index_ = 1;
    }

    void Set(StaticStorage stat) noexcept // NOLINT
    {
        HardReset();
        std::construct_at(&inplace_storage, std::move(stat));
        index_ = 2;
    }

    void Reset() noexcept
    {
        HardReset();
        std::construct_at(&empty);
        index_ = 0;
    }

    ~Storage() noexcept
    {
        HardReset();
    }

private:
    int8_t index_ = 0;

    void HardReset() noexcept
    {
        switch (index_) {
            case 0:
                std::destroy_at(&empty);
                break;
            case 1:
                std::destroy_at(&data_ptr);
                break;
            case 2:
                std::destroy_at(&inplace_storage);
                break;
            default:
                WHEELS_UNREACHABLE();
        }
    }
};

/////////////////////////////////////////////////////////////////////////

template <class DecayedConcrete, class StorageType>
struct StorageTraits
{
    using DecayedStorage = std::remove_cvref_t<StorageType>;
    using Static = typename DecayedStorage::StaticStorage;

    static constexpr bool IsStatic = DecayedStorage::template IsStatic<DecayedConcrete>(); // NOLINT

    static DecayedConcrete& AsConcrete(DecayedStorage& stor) noexcept
    {
        using Ptr = DecayedConcrete*;

        if constexpr (IsStatic) {
            return *std::launder(reinterpret_cast<Ptr>(&stor.inplace_storage));
        } else {
            return *static_cast<Ptr>(stor.data_ptr);
        }
    }

    static const DecayedConcrete& AsConcrete(const DecayedStorage& stor) noexcept
    {
        using Ret = const DecayedConcrete*;

        if constexpr (IsStatic) {
            return *std::launder(reinterpret_cast<Ret>(&stor.inplace_storage));
        } else {
            return *static_cast<Ret>(stor.data_ptr);
        }
    }

    static DecayedConcrete&& AsConcrete(DecayedStorage&& stor) noexcept
    {
        using Ret = DecayedConcrete*;

        if constexpr (IsStatic) {
            return std::move(*std::launder(reinterpret_cast<Ret>(&stor.inplace_storage)));
        } else {
            return std::move(*static_cast<Ret>(stor.data_ptr));
        }
    }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::detail
