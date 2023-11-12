#include <src/meta/static_env.hpp>

#include <iostream>

struct SimpleEnv
{ };

struct SecondEnv
{ };

struct ThirdEnv
{ };

int main()
{
    VerifyEnv<NullEnvironment>();

    [[maybe_unused]] auto begObj = BeginEnv<SimpleEnv>();

    VerifyEnv<SimpleEnv>();

    std::cout << PrintEnv() << '\n';

    [[maybe_unused]] auto begObj2 = BeginEnv<SecondEnv>();

    std::cout << PrintEnv() << '\n';

    VerifyEnv<SecondEnv>();

    [[maybe_unused]] auto endObj2 = EndEnv<SecondEnv>();

    std::cout << PrintEnv() << '\n';

    [[maybe_unused]] auto begObj3 = BeginEnv<ThirdEnv>();

    std::cout << PrintEnv() << '\n';

    VerifyEnv<ThirdEnv>();

    [[maybe_unused]] auto endObj3 = EndEnv<ThirdEnv>();

    [[maybe_unused]] auto endObj = EndEnv<SimpleEnv>();

    std::cout << PrintEnv() << '\n';

    VerifyEnv<NullEnvironment>();
}