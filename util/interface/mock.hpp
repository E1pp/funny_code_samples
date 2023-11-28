#pragma once

#include "trait_base.hpp"

#include <util/type_erasure/any_object.hpp>

namespace util {

/////////////////////////////////////////////////////////////////////////

struct MockTraits
    : private TraitBase<MockTraits>
{
    static constexpr auto Foo = Fragment<void(const This&, int) noexcept>(); // NOLINT

    using IMock = fine_tuning::AnyObject<0, 0, EConstructorConcept::NothrowCopyConstructible, true, std::allocator<std::byte>, Tag<Foo>>;
};

/////////////////////////////////////////////////////////////////////////

MockTraits::IMock CreateMock() noexcept;

/////////////////////////////////////////////////////////////////////////

} // namespace util
