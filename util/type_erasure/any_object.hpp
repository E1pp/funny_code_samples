#pragma once

#include "detail/value_wrapper.hpp"
#include "detail/erased_tag_invoker.hpp"

#include <memory>

namespace util::type_erasure {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <size_t SizeSBO, size_t AlignSBO, class Allocator, EConstructorConcept Concept, class... CPOs>
class AnyObject
    : private ErasedTagInvoker<AnyObject<SizeSBO, AlignSBO, Allocator, Concept, CPOs...>, CPOs>...
{
public:
    explicit AnyObject(Allocator alloc = {}) noexcept
        : allocator_(std::move(alloc))
    { }

    template <class Concrete>
    AnyObject(Concrete&& conc, Allocator alloc = {}) // NOLINT
        : AnyObject(std::move(alloc))
    {
        Set(std::forward<Concrete>(conc));
    }

    // Some other constructors? Inplace?

    AnyObject(AnyObject&& that) noexcept(MoveNoExcept<Concept>) 
        requires AddMoveConstructor<Concept>
        : vtable_(that.vtable_)
        , allocator_(std::move(that.allocator_))
    {
        if (vtable_) {
            auto move_constructor_fwd = vtable_.template Get<MoveConstructorCPO<Allocator, StorageType>>();

            move_constructor_fwd(MoveConstructor<Allocator, StorageType>, std::move(that.storage_), storage_, allocator_);
            that.vtable_ = {};
            that.storage_ = {};
        }
    }

    AnyObject& operator= (AnyObject&& that) noexcept(MoveNoExcept<Concept>)
        requires AddMoveConstructor<Concept>
    {
        if (this == &that) {
            return *this;
        }

        Reset();
        vtable_ = that.vtable_;

        if constexpr (AllocTraits::propagate_on_container_move_assignment::value) {
            allocator_ = std::move(allocator_);
        }

        if (vtable_) {
            if constexpr (AllocTraits::propagate_on_container_move_assignment::value) {
                auto move_constructor_fwd = vtable_.template Get<MoveConstructorCPO<Allocator, StorageType>>();

                move_constructor_fwd(MoveConstructor<Allocator, StorageType>, std::move(that.storage_), storage_, allocator_);
            } else {
                // ???
            }

            that.vtable_ = {};
            that.storage_ = {};
        }

        return *this;
    }

//     // AnyObject(const AnyObject& that) noexcept(detail::CopyNoExcept<Concept>)
//     //     requires detail::AddCopyConstructor<Concept>
//     //     : vtable_(that.vtable_)
//     // {
//     //     if (vtable_) {
//     //         auto copy_constructor_forwarder = vtable_.template Get<detail::CopyConstructorCPO>();

//     //         copy_constructor_forwarder(detail::CopyConstructor, const_cast<detail::Storage&>(that.data_), data_); // NOLINT
//     //     }
//     // }

//     // AnyObject& operator=(const AnyObject& that) noexcept(detail::CopyNoExcept<Concept>)
//     //     requires detail::AddCopyConstructor<Concept>
//     // {
//     //     if (this == &that) {
//     //         return *this;
//     //     }

//     //     // Reset();

//     //     // vtable_ = that.vtable_;

//     //     // if (vtable_) {
//     //     //     auto copyer = vtable_.template Get<detail::CopyConstructorCPO>();

//     //     //     copyer(data_, that.data_);
//     //     // }

//     //     return *this;
//     // }

    ~AnyObject() noexcept
    {
        Reset();
    }

private:
    using AllocTraits = std::allocator_traits<Allocator>;
    using StorageType = Storage<SizeSBO, AlignSBO>;
    using VTableType = AugmentedVTable<Concept, Allocator, StorageType, CPOs...>;

    StorageType storage_;
    VTableType vtable_;
    [[no_unique_address]] Allocator allocator_;

    template <class Derived, TypedCPO CPO, Signature Sig>
    friend struct ErasedTagInvoker;

    StorageType& GetObjectStorage() & noexcept
    {
        return storage_;
    }

    const StorageType GetObjectStorage() const & noexcept
    {
        return storage_;
    }

    StorageType&& GetObjectStorage() && noexcept
    {
        // Make sure to use vtable before calling this
        vtable_ = {};

        return std::move(storage_);
    }

    const auto* GetVTable() const noexcept
    {
        return &vtable_;
    }

    void Reset() noexcept
    {
        if (vtable_) {
            auto delete_fwd = vtable_.template Get<DeleterCPO<Allocator>>();

            // void(Deleter, Storage&, Allocator&)
            delete_fwd(Deleter<Allocator>, storage_, allocator_);
        }
        vtable_ = {};
        storage_ = {};
    }

    template <class Concrete>
    void Set(Concrete&& concrete)
    {
        using DecayedConcrete = Decay<Concrete>;
        using Wrapper = ValueWrapper<Concept, DecayedConcrete, Allocator, StorageType, CPOs...>;
        using StorTraits = StorageTraits<Wrapper, StorageType&>;

        Reset();

        static constexpr auto vtable = VTableType::template Create<Wrapper>(); // NOLINT
        vtable_ = vtable;

        storage_ = AllocTraits::allocate(allocator_, sizeof(Wrapper));
        AllocTraits::template construct<Wrapper>(allocator_, &StorTraits::AsConcrete(storage_), std::forward<Concrete>(concrete));
    }
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

// This is a placeholder which should be placed in the beggining
// of a signature with proper const/ref qualificators.
using detail::This;

template <EConstructorConcept Concept, class... CPOs>
using AnyObject = detail::AnyObject<63, 64, std::allocator<std::byte>, Concept, CPOs...>;

} // namespace util::type_erasure
