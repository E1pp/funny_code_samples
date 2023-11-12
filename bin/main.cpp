#include <src/meta/static_env.hpp>

#include <iostream>

struct SimpleEnv
{ };

struct SecondEnv
{ };

struct ThirdEnv
{ };

// must be tagged and be constexpr/consteval
template <auto Tag = []{}>
constexpr void EnvAwareFunction()
{
    if constexpr (IsCurrentEnv<SimpleEnv, Tag>()) {
        std::cout << "Foo" << '\n';
    } else {
        if constexpr (IsCurrentEnv<SecondEnv, Tag>()) {
            std::cout << "Bar" << '\n';
        } else {
            if constexpr (IsCurrentEnv<ThirdEnv, Tag>()) {
                std::cout << "Bax" << '\n';
            }
        }
    }
}

int main()
{
    VerifyEnv<NullEnvironment>();

    [[maybe_unused]] auto begObj = BeginEnv<SimpleEnv>();

    VerifyEnv<SimpleEnv>();
    EnvAwareFunction();

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