#include <util/arcane/reflector_base.hpp>

#include <wheels/test/test_framework.hpp>

#include <fmt/core.h>

using namespace util; // NOLINT
using namespace util::arcane; // NOLINT

struct Alex
    : public ReflectorBase<Alex>
{ };

template <bool Formula>
struct TrickyAlex
    : public ReflectorBase<TrickyAlex<Formula>>
{ };

template <class Other, bool Flag>
struct TrickierAlex
    : public ReflectorBase<TrickierAlex<Other, Flag>>
{ };

template <FixedString<256> Tag>
struct PrintName
{
    static constexpr std::string_view Do() noexcept
    {
        return Tag;
    }
};

TEST_SUITE(Reflector)
{
    SIMPLE_TEST(JustWorks)
    {
        static_assert(Alex::GetName() == "Alex"_cstr);
        static_assert(TrickyAlex<true>::GetName() == "TrickyAlex<true>"_cstr);
        static_assert(TrickyAlex<5 == 6>::GetName() == "TrickyAlex<false>"_cstr);
        static_assert(TrickierAlex<TrickyAlex<42 == 34>, 5 == 5>::GetName() == "TrickierAlex<TrickyAlex<false>, true>"_cstr);
    }

    SIMPLE_TEST(Rebind)
    {
        static_assert("Alex"_cstr == Alex::UniquelyTagged<PrintName>::Type::Do());
        static_assert("TrickyAlex<true>"_cstr == TrickyAlex<true>::UniquelyTagged<PrintName>::Type::Do());
    }
}

RUN_ALL_TESTS()