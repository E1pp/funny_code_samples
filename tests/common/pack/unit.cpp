#include <util/common/type_pack/erase.hpp>
#include <util/common/type_pack/first_of.hpp>
#include <util/common/type_pack/fold.hpp>
#include <util/common/type_pack/insert.hpp>
#include <util/common/type_pack/replace_all.hpp>
#include <util/common/type_pack/to_pack.hpp>

#include <wheels/test/test_framework.hpp>

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

template <class A>
struct SameAsVoid
{
    static constexpr bool Value = std::same_as<A, void>; // NOLINT
};

struct ThisPH
{ };

struct StorageType
{ };

template <class A>
struct SameAsThis
{
    static constexpr bool Value = std::same_as<Decay<A>, ThisPH>; // NOLINT
};

TEST_SUITE(Pack)
{
    SIMPLE_TEST(First)
    {
        static_assert(std::same_as<int, First<Pack<int, bool, char>>>);
    }

    SIMPLE_TEST(PushFront)
    {
        static_assert(std::same_as<Pack<int, bool, char>, PushFront<Pack<bool, char>, int, true>>);
        static_assert(std::same_as<Pack<bool, char>, PushFront<Pack<bool, char>, int, false>>);

        static_assert(std::same_as<Pack<int>, PushFront<Pack<>, int, true>>);
        static_assert(std::same_as<Pack<>, PushFront<Pack<>, int, false>>);
    }

    SIMPLE_TEST(PushBack)
    {
        static_assert(std::same_as<Pack<int, bool, char>, PushBack<Pack<int, bool>, char, true>>);
        static_assert(std::same_as<Pack<int, bool>, PushBack<Pack<int, bool>, char, false>>);

        static_assert(std::same_as<Pack<int>, PushBack<Pack<>, int, true>>);
        static_assert(std::same_as<Pack<>, PushBack<Pack<>, int, false>>);
    }

    SIMPLE_TEST(PopFront)
    {
        static_assert(std::same_as<Pack<>, PopFront<Pack<int>>>);
        static_assert(std::same_as<Pack<int>, PopFront<Pack<char, int>>>);
    }

    SIMPLE_TEST(PopBack)
    {
        static_assert(std::same_as<Pack<>, PopBack<Pack<int>>>);
        static_assert(std::same_as<Pack<char>, PopBack<Pack<char, int>>>);
    }

    SIMPLE_TEST(ReplaceAll)
    {
        static_assert(
            std::same_as<
                Pack<int, bool, double*, const char*>,
                ReplaceAll<char, unsigned char, Pack<int, bool, double*, const unsigned char*>>
            >);

        #define Checker(Type) \
        static_assert( \
            std::same_as< \
                Pack<StorageType, bool, double*, Type>, \
                ReplaceAll<StorageType, ThisPH, Pack<ThisPH, bool, double*, Type>> \
            >); \
        static_assert( \
            std::same_as< \
                Pack<StorageType*, bool, double*, Type>, \
                ReplaceAll<StorageType, ThisPH, Pack<ThisPH*, bool, double*, Type>> \
            >); \
        static_assert( \
            std::same_as< \
                Pack<StorageType&, bool, double*, Type>, \
                ReplaceAll<StorageType, ThisPH, Pack<ThisPH&, bool, double*, Type>> \
            >); \
        static_assert( \
            std::same_as< \
                Pack<StorageType&&, bool, double*, Type>, \
                ReplaceAll<StorageType, ThisPH, Pack<ThisPH&&, bool, double*, Type>> \
            >); \
        static_assert( \
            std::same_as< \
                Pack<const StorageType&, bool, double*, Type>, \
                ReplaceAll<StorageType, ThisPH, Pack<const ThisPH&, bool, double*, Type>> \
            >);

        ITERATE_OVER_COMMON_TYPES

        #undef Checker
    }

    SIMPLE_TEST(SigToPack)
    {
        static_assert(
            std::same_as<
                Pack<>,
                SigToPack<void()>
            >);

        static_assert(std::same_as<
            Pack<int, bool>,
            SigToPack<int(int, bool)>
        >);
    }

    SIMPLE_TEST(Fold)
    {
        static_assert(!FoldOr<SameAsVoid, Pack<>>);
        static_assert(FoldOr<SameAsVoid, Pack<void, int>>);
        static_assert(!FoldAnd<SameAsVoid, Pack<void, int>>);
        static_assert(FoldAnd<SameAsVoid, Pack<void, void>>);

        static_assert(!FoldOr<SameAsThis, Pack<>>);
        static_assert(FoldOr<SameAsThis, Pack<ThisPH, int>>);
        static_assert(FoldOr<SameAsThis, Pack<ThisPH*, int>>);
        static_assert(!FoldAnd<SameAsThis, Pack<void, int>>);
        static_assert(FoldAnd<SameAsThis, Pack<ThisPH, ThisPH>>);
        static_assert(FoldAnd<SameAsThis, Pack<ThisPH&, ThisPH*>>);
    }
}

RUN_ALL_TESTS()