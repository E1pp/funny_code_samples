#include <util/interface/mock.hpp>

#include <wheels/test/test_framework.hpp>

using namespace util; // NOLINT

TEST_SUITE(Interface)
{
    SIMPLE_TEST(JustWorks)
    {
        auto mock = CreateMock();

        MockTraits::Foo(mock, 5);
        MockTraits::Foo(mock, 6);
    }
}

RUN_ALL_TESTS()