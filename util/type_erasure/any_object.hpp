#pragma once

#include "detail/value_wrapper.hpp"
#include "detail/erased_tag_invoker.hpp"

#include <util/common/meta/is_instance.hpp>

#include <memory>
#include <optional>

namespace util {

/////////////////////////////////////////////////////////////////////////

struct BadAnyAccess
    : public std::exception
{ };

/////////////////////////////////////////////////////////////////////////

namespace fine_tuning {

// TODO:
// 1) More tests?
template <size_t SizeSBO, size_t AlignSBO, EConstructorConcept Concept, bool StaticVTable, class Allocator, class... CPOs>
class AnyObject
    : private detail::ErasedTagInvoker<AnyObject<SizeSBO, AlignSBO, Concept, StaticVTable, Allocator, CPOs...>, CPOs>...
{
public:
    explicit AnyObject(Allocator alloc = {}) noexcept
        : allocator_(std::move(alloc))
    { }

    template <class Concrete>
        requires 
            (!std::same_as<std::remove_cvref_t<Concrete>, AnyObject>) &&
            (std::constructible_from<std::remove_cvref_t<Concrete>, Concrete>) &&
            (detail::AccordinglyConstructible<std::remove_cvref_t<Concrete>, Concept>) &&
            (detail::TypeErasable<Concrete, CPOs...>)
    AnyObject(Concrete&& conc, Allocator alloc = {}) // NOLINT
        noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Concrete>, Concrete>)
        : AnyObject(std::move(alloc))
    {
        Set<Concrete>(std::forward<Concrete>(conc));
    }

    template <class Concrete, class... Args>
        requires 
            (!std::same_as<std::remove_cvref_t<Concrete>, AnyObject>) &&
            (std::constructible_from<std::remove_cvref_t<Concrete>, Args...>) &&
            (detail::AccordinglyConstructible<std::remove_cvref_t<Concrete>, Concept>) &&
            (detail::TypeErasable<Concrete, CPOs...>)
    AnyObject(std::in_place_type_t<Concrete>, Allocator alloc, Args&&... args) // NOLINT
        noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Concrete>, Args...>)
        : AnyObject(std::move(alloc))
    {
        Set<Concrete>(std::forward<Args>(args)...);
    }

    AnyObject(AnyObject&& that) noexcept(detail::MoveNoExcept<Concept>) 
        requires detail::AddMoveConstructor<Concept>
        : vtable_(that.vtable_)
        , allocator_(std::move(that.allocator_))
    {
        if (vtable_) {
            auto* mover = vtable_->template Get<detail::MoveCPO<Allocator, StorageType>>();
            mover(detail::Mover<Allocator, StorageType>, std::move(that.storage_), storage_, allocator_);

            that.vtable_.Reset();
            that.storage_.Reset();
        }
    }

    AnyObject& operator= (AnyObject&& that) noexcept(detail::MoveNoExcept<Concept>)
        requires detail::AddMoveConstructor<Concept>
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
                auto* mover = vtable_->template Get<detail::MoveCPO<Allocator, StorageType>>();
                mover(detail::Mover<Allocator, StorageType>, std::move(that.storage_), storage_, allocator_);
            } else {
                auto* mover = vtable_->template Get<detail::MoveReallocCPO<Allocator, StorageType>>();
                mover(detail::ReallocMover<Allocator, StorageType>, std::move(that.storage_), storage_, allocator_, std::move(that.allocator_));
            }

            that.vtable_.Reset();
            that.storage_.Reset();
        }

        return *this;
    }

    AnyObject(const AnyObject& that) noexcept(detail::CopyNoExcept<Concept>)
        requires detail::AddCopyConstructor<Concept>
        : vtable_(that.vtable_)
        , allocator_(AllocTraits::select_on_container_copy_construction(that.allocator_))
    {
        if (vtable_) {
            auto copier = vtable_->template Get<detail::CopyCPO<Allocator, StorageType>>();
            copier(detail::Copier<Allocator, StorageType>, that.storage_, storage_, allocator_);
        }
    }

    AnyObject& operator=(const AnyObject& that) noexcept(detail::CopyNoExcept<Concept>)
        requires detail::AddCopyConstructor<Concept>
    {
        if (this == &that) {
            return *this;
        }

        Reset();
        vtable_ = that.vtable_;

        if constexpr (AllocTraits::propagate_on_container_copy_assignment::value) {
            allocator_ = that.allocator_;
        }

        if (vtable_) {
            auto* copier = vtable_->template Get<detail::CopyCPO<Allocator, StorageType>>();
            copier(detail::Copier<Allocator, StorageType>, that.storage_, storage_, allocator_);
        }

        return *this;
    }

    constexpr explicit operator bool() const noexcept
    {
        return static_cast<bool>(vtable_);
    }

    template <class Concrete>
        requires
            (!std::same_as<std::remove_cvref_t<Concrete>, AnyObject>) &&
            (std::constructible_from<std::remove_cvref_t<Concrete>, Concrete>) &&
            (detail::AccordinglyConstructible<std::remove_cvref_t<Concrete>, Concept>) &&
            (detail::TypeErasable<Concrete, CPOs...>)
    void Emplace(Concrete&& conc) 
        noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<Concrete>, Concrete>)
    {
        Set<Concrete>(std::forward<Concrete>(conc));
    }

    ~AnyObject() noexcept
    {
        Reset();
    }

    template <class Concrete>
        requires
            (!std::same_as<std::remove_cvref_t<Concrete>, AnyObject>) &&
            (detail::AccordinglyConstructible<std::remove_cvref_t<Concrete>, Concept>) &&
            (detail::TypeErasable<Concrete, CPOs...>)
    std::remove_cvref_t<Concrete>& Get() &
    {
        using DecayedConcrete = Decay<Concrete>;
        using TesterVTableEntry = detail::VTableEntry<StorageType, detail::DeleterCPO<Allocator>>;
        using TesterWrapper = detail::ValueWrapper<Concept, DecayedConcrete, Allocator, StorageType, CPOs...>;

        if (!vtable_ ||
            vtable_->template Get<detail::DeleterCPO<Allocator>>() 
                != TesterVTableEntry::template Create<TesterWrapper>().Get())
        {
            throw BadAnyAccess{};
        }

        return storage_.template AsConcrete<DecayedConcrete>();
    }

    template <class Concrete>
        requires
            (!std::same_as<std::remove_cvref_t<Concrete>, AnyObject>) &&
            (detail::AccordinglyConstructible<std::remove_cvref_t<Concrete>, Concept>) &&
            (detail::TypeErasable<Concrete, CPOs...>)
    const std::remove_cvref_t<Concrete>& Get() const &
    {
        using DecayedConcrete = Decay<Concrete>;
        using TesterVTableEntry = detail::VTableEntry<StorageType, detail::DeleterCPO<Allocator>>;
        using TesterWrapper = detail::ValueWrapper<Concept, DecayedConcrete, Allocator, StorageType, CPOs...>;

        if (!vtable_ ||
            vtable_->template Get<detail::DeleterCPO<Allocator>>() 
                != TesterVTableEntry::template Create<TesterWrapper>().Get())
        {
            throw BadAnyAccess{};
        }

        return storage_.template AsConcrete<DecayedConcrete>();
    }

private:
    using AllocTraits = std::allocator_traits<Allocator>;
    using StorageType = detail::Storage<SizeSBO, AlignSBO>;
    using VTableType = detail::AugmentedVTable<Concept, Allocator, StorageType, CPOs...>;

    StorageType storage_;
    detail::VTableHolder<StaticVTable, VTableType> vtable_;
    [[no_unique_address]] Allocator allocator_;

    template <class Derived, TypedCPO CPO, Signature Sig>
    friend struct detail::ErasedTagInvoker;

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
        return std::move(storage_);
    }

    const auto& GetVTable() const noexcept
    {
        return vtable_;
    }

    void Reset() noexcept
    {
        if (vtable_) {
            auto* deleter = vtable_->template Get<detail::DeleterCPO<Allocator>>();
            deleter(detail::Deleter<Allocator>, storage_, allocator_);

            vtable_.Reset();
            storage_.Reset();
        }
    }

    template <class Concrete, class... Args>
    void Set(Args&&... args)
    {
        using DecayedConcrete = Decay<Concrete>;
        using Wrapper = detail::ValueWrapper<Concept, DecayedConcrete, Allocator, StorageType, CPOs...>;

        Reset();

        static constexpr auto vtable = VTableType::template Create<Wrapper>(); // NOLINT
        vtable_ = vtable;

        if constexpr (StorageType::template IsStatic<DecayedConcrete>) {
            storage_.Set();
        } else {
            storage_.Set(AllocTraits::allocate(allocator_, sizeof(Wrapper)));
        }

        AllocTraits::template construct<Wrapper>(
            allocator_,
            &storage_.template AsConcrete<Wrapper>(),
            std::forward<Args>(args)...);
    }
};

} // namespace fine_tuning

/////////////////////////////////////////////////////////////////////////

// This is a placeholder which should be placed in the beggining
// of a signature with proper const/ref qualificators.
using detail::This;

template <EConstructorConcept Concept, class... CPOs>
using AnyObject = fine_tuning::AnyObject<63, 64, Concept, false, std::allocator<std::byte>, CPOs...>;

template <class Concrete, class Any>
    // requires IsInstanceOfAny
decltype(auto) Get(Any&& any) noexcept
{
    return std::forward<Any>(any).template Get<Concrete>();
}

} // namespace util
