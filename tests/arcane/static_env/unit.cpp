#include <util/arcane/static_env.hpp>

#include <wheels/test/test_framework.hpp>

using namespace util; // NOLINT

struct SimpleEnv
{ };

struct SecondEnv
{ };

struct ThirdEnv
{ };

TEST_SUITE(StaticEnv)
{
    SIMPLE_TEST(JustWorks)
    {
        VerifyEnv<NullEnvironment>();

        [[maybe_unused]] auto beg_obj = BeginEnv<SimpleEnv>();

        VerifyEnv<SimpleEnv>();

        [[maybe_unused]] auto beg_obj2 = BeginEnv<SecondEnv>();

        VerifyEnv<SecondEnv>();

        [[maybe_unused]] auto end_obj2 = EndEnv<SecondEnv>();

        [[maybe_unused]] auto beg_obj3 = BeginEnv<ThirdEnv>();

        VerifyEnv<ThirdEnv>();

        [[maybe_unused]] auto end_obj3 = EndEnv<ThirdEnv>();

        [[maybe_unused]] auto end_obj = EndEnv<SimpleEnv>();

        VerifyEnv<NullEnvironment>();
    }
}

RUN_ALL_TESTS()