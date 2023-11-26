#pragma once

#include "fragment.hpp"

#include <util/type_erasure/any_object.hpp>

namespace util {

/////////////////////////////////////////////////////////////////////////

struct MockTraits
{
    static constexpr Fragment<MockTraits, void(This&, int) noexcept> Foo = {}; // NOLINT

    using IMock = fine_tuning::AnyObject<0, 0, EConstructorConcept::NothrowCopyConstructible, true, std::allocator<std::byte>, Tag<Foo>>;
};

/////////////////////////////////////////////////////////////////////////

MockTraits::IMock CreateMock() noexcept;

/////////////////////////////////////////////////////////////////////////

} // namespace util
