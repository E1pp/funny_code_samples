#pragma once

#include <util/common/meta/fixed_string.hpp>

#include <concepts>

namespace util::arcane {

/////////////////////////////////////////////////////////////////////////

//! Baseline reflector which allows creation of concrete reflector bases
//! which have their types and methods uniquely tagged.
template <class Derived, size_t NameSize = 256>
struct ReflectorBase
{
    using TThis = Derived;

    static constexpr FixedString<NameSize> GetName() noexcept
    {
        static_assert(std::derived_from<TThis, ReflectorBase>);

        constexpr std::string_view kSv = __PRETTY_FUNCTION__;

        constexpr auto kBeg1 = std::find(kSv.begin(), kSv.end(), '[');
        constexpr auto kBeg2 = std::find(kBeg1, kSv.end(), '=');
        constexpr auto kBeg = kBeg2 + 2; // beg2
        constexpr auto kOffsetFromRight = std::find(kSv.rbegin(), kSv.rend(), ',') - kSv.rbegin();

        constexpr auto kAns = std::string_view(kBeg, kSv.end() - kOffsetFromRight - 1);

        static_assert(kAns.size() <= NameSize);

        return FixedString<NameSize>(kAns);
    }

    //! This hack is required because type Derived is incomplete until the end of this class definition
    //! So we must fall back to out of line definition of a struct instead of alias.
    template <template <FixedString<NameSize>> class Taggable>
    struct UniquelyTagged;
};

/////////////////////////////////////////////////////////////////////////

template <class Derived, size_t NameSize>
template <template <FixedString<NameSize>> class Taggable>
struct ReflectorBase<Derived, NameSize>::UniquelyTagged
{
    using Type = Taggable<GetName()>;
};

/////////////////////////////////////////////////////////////////////////

} // namespace util::arcane
