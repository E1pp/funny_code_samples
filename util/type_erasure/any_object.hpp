#pragma once

#include "erased_tag_invoker.hpp"
#include "value_wrapper.hpp"
#include "vtable.hpp"

#include <memory>

namespace util::type_erasure {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <size_t SizeSBO, size_t AlignSBO, class Allocator, EConstructorConcept Concept, class... CPOs>
class AnyObject
    : private detail::ErasedTagInvoker<AnyObject<SizeSBO, AlignSBO, Allocator, Concept, CPOs...>, CPOs>...
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

    // AnyObject(AnyObject&& that) noexcept(detail::MoveNoExcept<Concept>) 
    //     requires detail::AddMoveConstructor<Concept>
    //     : vtable_(that.vtable_)
    // {
    //     if (vtable_) {
    //         auto move_constructor_forwarder = vtable_.template Get<detail::MoveConstructorCPO>();

    //         move_constructor_forwarder(detail::MoveConstructor, that.data_, data_);
    //         that.vtable_ = {};
    //     }
    // }

    // AnyObject& operator= (AnyObject&& that) noexcept(detail::MoveNoExcept<Concept>)
    //     requires detail::AddMoveConstructor<Concept>
    // {
    //     if (this == &that) {
    //         return *this;
    //     }

    //     // Reset();
    //     // vtable_ = that.vtable_;

    //     // if (vtable_) {
    //     //     auto mover = vtable_.template Get<detail::MoveConstructorCPO>();

    //     //     mover(data_, std::move(that.data_));
    //     //     that.vtable_ = {};
    //     // }

    //     return *this;
    // }

    // AnyObject(const AnyObject& that) noexcept(detail::CopyNoExcept<Concept>)
    //     requires detail::AddCopyConstructor<Concept>
    //     : vtable_(that.vtable_)
    // {
    //     if (vtable_) {
    //         auto copy_constructor_forwarder = vtable_.template Get<detail::CopyConstructorCPO>();

    //         copy_constructor_forwarder(detail::CopyConstructor, const_cast<detail::Storage&>(that.data_), data_); // NOLINT
    //     }
    // }

    // AnyObject& operator=(const AnyObject& that) noexcept(detail::CopyNoExcept<Concept>)
    //     requires detail::AddCopyConstructor<Concept>
    // {
    //     if (this == &that) {
    //         return *this;
    //     }

    //     // Reset();

    //     // vtable_ = that.vtable_;

    //     // if (vtable_) {
    //     //     auto copyer = vtable_.template Get<detail::CopyConstructorCPO>();

    //     //     copyer(data_, that.data_);
    //     // }

    //     return *this;
    // }

    ~AnyObject() noexcept
    {
        Reset();
    }

private:
    using StorageType = detail::Storage<SizeSBO, AlignSBO>;
    using VTableType = detail::VTableWithConstructors<Allocator, StorageType, detail::VTable<CPOs...>, Concept>;

    StorageType data_;
    VTableType vtable_;
    [[no_unique_address]] Allocator allocator_;

    template <class Derived, TypedCPO CPO, Signature Sig>
    friend struct detail::ErasedTagInvoker;

    StorageType& GetObjectStorage() noexcept
    {
        return data_;
    }

    const auto& GetVTable() const noexcept
    {
        return vtable_;
    }

    void Reset() noexcept
    {
        if (vtable_) {
            auto delete_forwarder = vtable_.template Get<detail::DeleterCPO<Allocator>>();

            delete_forwarder(detail::Deleter<Allocator>, data_, allocator_);
        }
        vtable_ = {};
        data_ = {};
    }

    template <class Concrete>
    void Set(Concrete&& concrete)
    {
        using DecayedConcrete = std::decay_t<Concrete>;
        using Wrapper = ValueWrapper<Concept, DecayedConcrete, Allocator, CPOs...>;
        using Traits = std::allocator_traits<Allocator>;

        Reset();

        static constexpr auto vtable = VTableType::template Create<Wrapper>(); // NOLINT
        vtable_ = vtable;

        data_ = Traits::allocate(allocator_, sizeof(Wrapper));
        Traits::template construct<Wrapper>(allocator_, &AsConcreteRef<Wrapper, StorageType>(data_), std::forward<Concrete>(concrete));
    }
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

template <EConstructorConcept Concept, class... CPOs>
using AnyObject = detail::AnyObject<63, 64, std::allocator<std::byte>, Concept, CPOs...>;

} // namespace util::type_erasure
