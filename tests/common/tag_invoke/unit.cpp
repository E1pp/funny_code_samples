#include <util/common/tag_invoke.hpp>

#include <wheels/test/test_framework.hpp>

using namespace util; // NOLINT

inline constexpr struct __TestCPO1 {
    template <class... Args>
        requires TagInvocable<__TestCPO1, Args...>
    constexpr auto operator()(Args&&... args) const
        noexcept(NothrowTagInvocable<__TestCPO1, Args...>)
    {
        return TagInvoke(*this, std::forward<Args>(args)...);
    }

    constexpr int operator()(int v) const noexcept
    {
        return v + 1;
    }

} TagInvokeTester = {};

struct OverloadedType1
{
    friend bool TagInvoke(Tag<TagInvokeTester>, OverloadedType1) noexcept
    {
        counter++;
        return counter % 2 == 0;
    }

    static inline int counter = 0;
};

struct OverloadedType2
{
    friend constexpr bool TagInvoke(Tag<TagInvokeTester>, OverloadedType2) noexcept
    {
        return true;
    }
};

struct OverloadedType3
{
    friend constexpr bool TagInvoke(Tag<TagInvokeTester>, OverloadedType3) noexcept
    {
        return false;
    }
    
    operator OverloadedType2() const noexcept
    {
        return OverloadedType2{};
    }
};

TEST_SUITE(TagInvoke)
{
    SIMPLE_TEST(JustWorks)
    {
        ASSERT_EQ(43, TagInvokeTester(42));

        ASSERT_FALSE(TagInvokeTester(OverloadedType1{}));
        ASSERT_TRUE(TagInvokeTester(OverloadedType1{}));
    }

    SIMPLE_TEST(BestMatch)
    {
        ASSERT_TRUE(TagInvokeTester(OverloadedType2{}));
        ASSERT_FALSE(TagInvokeTester(OverloadedType3{}));
    }
}

RUN_ALL_TESTS()