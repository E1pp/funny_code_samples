#include <util/common/tag_invoke/generic_cpo.hpp>
#include <util/common/tag_invoke/tag_invoke.hpp>
#include <util/common/tag_invoke/typed_cpo.hpp>

#include <util/type_erasure/any_object.hpp>

#include <wheels/test/test_framework.hpp>

#include <util/type_erasure/detail/vtable.hpp>

#include <iostream>

using namespace util; // NOLINT

inline constexpr GenericCPO Test = {}; // NOLINT

class TestArg
{
    friend constexpr bool TagInvoke(Tag<Test>, TestArg) noexcept
    {
        return true;
    }
};

class AnotherTestArg
{
    friend constexpr bool TagInvoke(Tag<Test>, AnotherTestArg) noexcept
    {
        return false;
    }
};

TEST_SUITE(AnyObject)
{
    SIMPLE_TEST(JustWorks)
    {
        // using Any = type_erasure::AnyObject<type_erasure::EConstructorConcept::NothrowCopyConstructible, Tag<Overload<Tag<Test>, bool() noexcept>()>>;

        // TestArg arg = {};

        // ASSERT_TRUE(Test(arg));

        // Any any{arg};

        // ASSERT_TRUE(Test(any));

        // // any = AnotherTestArg{};

        // // ASSERT_FALSE(Test(any));

        // // const Any& ref = any;

        // // Any any2(ref);
        // // Any any3(std::move(any2));
    }
}

RUN_ALL_TESTS()