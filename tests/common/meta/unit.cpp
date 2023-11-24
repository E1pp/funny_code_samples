#include <util/common/meta/decay.hpp>
#include <util/common/meta/replace.hpp>

#include <wheels/test/test_framework.hpp>

#include <concepts>
#include <vector>

using namespace util; // NOLINT

void Foo()
{ }

using FunctionType = decltype(&Foo);

#define ITERATE_OVER_COMMON_TYPES \
    Checker(int) \
    Checker(unsigned char) \
    Checker(bool) \
    Checker(char) \
    Checker(FunctionType) \
    Checker(std::vector<int>) \

TEST_SUITE(Meta)
{
    SIMPLE_TEST(Decay)
    {
        #define Checker(Type) \
            static_assert(std::same_as<Type, Decay<Type>>); \
            static_assert(std::same_as<Type, Decay<const Type>>); \
            static_assert(std::same_as<Type, Decay<Type&&>>); \
            static_assert(std::same_as<Type, Decay<const Type&>>); \
            static_assert(std::same_as<Type, Decay<Type*>>); \
            static_assert(std::same_as<Type, Decay<const Type*>>); \

        
        ITERATE_OVER_COMMON_TYPES

        #undef Checker
    }

    SIMPLE_TEST(Replace)
    {
        #define Checker(Type) \
            static_assert(std::same_as<FunctionType, Replace<FunctionType, Type>>); \
            static_assert(std::same_as<const FunctionType, Replace<FunctionType, const Type>>); \
            static_assert(std::same_as<FunctionType&&, Replace<FunctionType, Type&&>>); \
            static_assert(std::same_as<FunctionType&, Replace<FunctionType, Type&>>); \
            static_assert(std::same_as<const FunctionType&, Replace<FunctionType, const Type&>>); \
            static_assert(std::same_as<FunctionType*, Replace<FunctionType, Type*>>); \
            static_assert(std::same_as<const FunctionType*, Replace<FunctionType, const Type*>>); \

        ITERATE_OVER_COMMON_TYPES

        #undef Checker

        #define Checker(Type) \
            static_assert(std::same_as<bool, Replace<bool, Type>>); \
            static_assert(std::same_as<const bool, Replace<bool, const Type>>); \
            static_assert(std::same_as<bool&&, Replace<bool, Type&&>>); \
            static_assert(std::same_as<bool&, Replace<bool, Type&>>); \
            static_assert(std::same_as<const bool&, Replace<bool, const Type&>>); \
            static_assert(std::same_as<bool*, Replace<bool, Type*>>); \
            static_assert(std::same_as<const bool*, Replace<bool, const Type*>>); \

        ITERATE_OVER_COMMON_TYPES

        #undef Checker
    }
}

RUN_ALL_TESTS()