#include "mock.hpp"

#include <util/common/tag_invoke/generic_cpo.hpp>
#include <util/common/tag_invoke/tag_invoke.hpp>
#include <util/common/tag_invoke/typed_cpo.hpp>

#include <util/type_erasure/any_object.hpp>

#include <wheels/test/test_framework.hpp>

#include <array>
#include <iostream>

using namespace util; // NOLINT

/////////////////////////////////////////////////////////////////////////

inline constexpr GenericCPO Test = {}; // NOLINT

/////////////////////////////////////////////////////////////////////////

class Pinned
{
public:
    Pinned() {};

    explicit Pinned(int) {};

    Pinned(Pinned&&) = delete;
    Pinned& operator=(Pinned&&) = delete;

    Pinned(const Pinned&) = delete;
    Pinned& operator=(const Pinned&) = delete;
};

template <bool NoExcept, bool ANoExcept = NoExcept>
class MoveOnly
{
public:
    MoveOnly() {};

    MoveOnly(MoveOnly&&) noexcept(NoExcept) {};
    MoveOnly& operator=(MoveOnly&&) noexcept(ANoExcept) { return *this; };

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
};

template <bool MNoExcept, bool CNoExcept, bool AMNoExcept = MNoExcept, bool ACNoExcept = CNoExcept>
class Copyable
{
public:
    Copyable() {};

    Copyable(Copyable&&) noexcept(MNoExcept) {};
    Copyable& operator=(Copyable&&) noexcept(AMNoExcept) { return *this; };

    Copyable(const Copyable&) noexcept(CNoExcept) {};
    Copyable& operator=(const Copyable&) noexcept(ACNoExcept) { return *this; };
};

/////////////////////////////////////////////////////////////////////////

class TNOL
{ };

class TOL1
{
    friend bool TagInvoke(Tag<Test>, TOL1&)
    {
        return true;
    }
};

class TOL2
{
    friend bool TagInvoke(Tag<Test>, TOL2&) noexcept
    {
        return false;
    }
};

class TOL3
{
    friend bool TagInvoke(Tag<Test>, TOL3&&) noexcept
    {
        return true;
    }
};

class TOL4
{
    friend bool TagInvoke(Tag<Test>, const TOL4&) noexcept
    {
        return false;
    }
};

template <size_t Padding>
class Checker
{
public:
    static const auto& GetValues() 
    {
        return values;
    }

    static auto GetLogger()
    {
        return Logger{};
    }

private:
    class Logger
    {
        friend void TagInvoke(Tag<Test>, Logger&, int value) noexcept
        {
            values.push_back(value);
        }

        [[maybe_unused]] std::array<std::byte, Padding> padding_;
    };

    static inline std::vector<int> values = {};
};

/////////////////////////////////////////////////////////////////////////

template<std::size_t Padding, bool Reallocate>
void TestSBO() {
    using Alloc = TrackerAllocator<std::byte, Reallocate>;
    using Checker = Checker<Padding>;
    using Any = fine_tuning::AnyObject<31, 32, Alloc, EConstructorConcept::NothrowCopyConstructible>;
    Alloc::ResetCounters();

    
    auto any = Any();
    ASSERT_EQ(Alloc::AllocCount(), 0u);
    ASSERT_EQ(Alloc::PlacementCount(), 0u);

    // Arbitrary decently-sized constant.
    constexpr bool kEnableSBO = Padding <= 64;

    const std::size_t per_copy_alloc_inc = !kEnableSBO ? 1 : 0;
    const std::size_t per_move_assign_alloc_inc = !kEnableSBO && Reallocate ? 1 : 0;
    const std::size_t per_move_assign_place_inc = kEnableSBO || Reallocate ? 1 : 0;
    const std::size_t per_move_construct_place_inc = kEnableSBO ? 1 : 0;

    any.Emplace(Checker::GetLogger());
    ASSERT_EQ(Alloc::AllocCount(), per_copy_alloc_inc);
    ASSERT_EQ(Alloc::PlacementCount(), 1u);

    any.Emplace(Checker::GetLogger());
    ASSERT_EQ(Alloc::AllocCount(), 2 * per_copy_alloc_inc);
    ASSERT_EQ(Alloc::PlacementCount(), 2u);

    // Copy construct
    [[maybe_unused]] auto any1 = any;
    ASSERT_EQ(Alloc::AllocCount(), 3 * per_copy_alloc_inc);
    ASSERT_EQ(Alloc::PlacementCount(), 3u);

    // Copy assign
    Any any2;
    any2 = any;
    ASSERT_EQ(Alloc::AllocCount(), 4 * per_copy_alloc_inc);
    ASSERT_EQ(Alloc::PlacementCount(), 4u);

    // Move assign. Unequal alloc, expect reallocations
    Any any3(Alloc{1});
    any3 = std::move(any);
    ASSERT_EQ(Alloc::AllocCount(), 4 * per_copy_alloc_inc + per_move_assign_alloc_inc);
    ASSERT_EQ(Alloc::PlacementCount(), 4u + per_move_assign_place_inc);

    // Move construct, always grab allocator
    auto any4 = std::move(any3);
    ASSERT_EQ(Alloc::AllocCount(), 4 * per_copy_alloc_inc + per_move_assign_alloc_inc);
    ASSERT_EQ(Alloc::PlacementCount(), 4u + per_move_assign_place_inc + per_move_construct_place_inc);
}

/////////////////////////////////////////////////////////////////////////

TEST_SUITE(AnyObject)
{
    SIMPLE_TEST(ConstructorConcepts)
    {
        using AnyP = AnyObject<EConstructorConcept::Pinned>;
        using AnyM = AnyObject<EConstructorConcept::MoveConstructible>;
        using AnyNTM = AnyObject<EConstructorConcept::NothrowMoveConstructible>;
        using AnyC = AnyObject<EConstructorConcept::CopyConstructible>;
        using AnyNTC = AnyObject<EConstructorConcept::NothrowCopyConstructible>;

        static_assert(!std::movable<AnyP>);
        
        static_assert(std::movable<AnyM>);
        static_assert(!std::copyable<AnyM>);
        static_assert(!std::is_nothrow_move_constructible_v<AnyM>);
        static_assert(!std::is_nothrow_move_assignable_v<AnyM>);

        static_assert(std::movable<AnyNTM>);
        static_assert(!std::copyable<AnyNTM>);
        static_assert(std::is_nothrow_move_constructible_v<AnyNTM>);
        static_assert(std::is_nothrow_move_assignable_v<AnyNTM>);

        static_assert(std::copyable<AnyC>);
        static_assert(!std::is_nothrow_move_constructible_v<AnyC>);
        static_assert(!std::is_nothrow_move_assignable_v<AnyC>);
        static_assert(!std::is_nothrow_copy_constructible_v<AnyC>);
        static_assert(!std::is_nothrow_copy_assignable_v<AnyC>);

        static_assert(detail::CopyNoExcept<EConstructorConcept::NothrowCopyConstructible>);

        static_assert(std::copyable<AnyNTC>);
        static_assert(std::is_nothrow_move_constructible_v<AnyNTC>);
        static_assert(std::is_nothrow_move_assignable_v<AnyNTC>);
        static_assert(std::is_nothrow_copy_constructible_v<AnyNTC>);
        static_assert(std::is_nothrow_copy_assignable_v<AnyNTC>);
    }

    SIMPLE_TEST(ConstrainConcrete1)
    {
        using AnyP = AnyObject<EConstructorConcept::Pinned>;
        using AnyM = AnyObject<EConstructorConcept::MoveConstructible>;
        using AnyNTM = AnyObject<EConstructorConcept::NothrowMoveConstructible>;
        using AnyC = AnyObject<EConstructorConcept::CopyConstructible>;
        using AnyNTC = AnyObject<EConstructorConcept::NothrowCopyConstructible>;

        static_assert(!std::constructible_from<AnyP, Pinned>);
        static_assert(std::constructible_from<AnyP, std::in_place_type_t<Pinned>, std::allocator<std::byte>, int>);
        static_assert(std::constructible_from<AnyP, MoveOnly<false>>);

        static_assert(!std::constructible_from<AnyM, Pinned>);
        static_assert(!std::constructible_from<AnyM, std::in_place_type_t<Pinned>, std::allocator<std::byte>, int>);
        static_assert(std::constructible_from<AnyM, MoveOnly<false>>);

        static_assert(!std::constructible_from<AnyNTM, Pinned>);
        static_assert(!std::constructible_from<AnyNTM, std::in_place_type_t<Pinned>, std::allocator<std::byte>, int>);
        static_assert(!std::constructible_from<AnyNTM, MoveOnly<false>>);
        static_assert(!std::constructible_from<AnyNTM, MoveOnly<true, false>>);
        static_assert(std::constructible_from<AnyNTM, MoveOnly<true>>);
        static_assert(std::constructible_from<AnyNTM, Copyable<true, false>>);

        static_assert(!std::constructible_from<AnyC, Pinned>);
        static_assert(!std::constructible_from<AnyC, std::in_place_type_t<Pinned>, std::allocator<std::byte>, int>);
        static_assert(!std::constructible_from<AnyC, MoveOnly<true>>);
        static_assert(std::constructible_from<AnyC, Copyable<false, false>>);

        static_assert(!std::constructible_from<AnyNTC, Pinned>);
        static_assert(!std::constructible_from<AnyNTC, std::in_place_type_t<Pinned>, std::allocator<std::byte>, int>);
        static_assert(!std::constructible_from<AnyNTC, MoveOnly<true>>);
        static_assert(!std::constructible_from<AnyNTC, Copyable<false, false>>);
        static_assert(!std::constructible_from<AnyNTC, Copyable<true, false>>);
        static_assert(!std::constructible_from<AnyNTC, Copyable<true, true, true, false>>);
        static_assert(!std::constructible_from<AnyNTC, Copyable<true, true, false, true>>);
        static_assert(std::constructible_from<AnyNTC, Copyable<true, true>>);
    }

    SIMPLE_TEST(ConstrainConcrete2)
    {
        using Any = AnyObject<
            EConstructorConcept::Pinned,
            Overload<Tag<Test>, void(This&) noexcept>
            >;

        static_assert(!std::constructible_from<Any, TOL1>);
        static_assert(!std::constructible_from<Any, TNOL>);
        static_assert(std::constructible_from<Any, TOL2>);
        static_assert(!std::constructible_from<Any, TOL3>);
    }

    SIMPLE_TEST(TagInvocable)
    {
        using AnyCPO = AnyObject<
            EConstructorConcept::NothrowCopyConstructible,
            Overload<Tag<Test>, bool(This&) noexcept>
            >;

        static_assert(std::invocable<Tag<Test>, AnyCPO&>);
        static_assert(TagInvocable<Tag<Test>, AnyCPO&>);
    }

    SIMPLE_TEST(JustWorks)
    {
        using Any = AnyObject<
            EConstructorConcept::MoveConstructible,
            Overload<Tag<Test>, bool(This&)>
            >;

        Any any{TOL1{}};

        ASSERT_TRUE(Test(any));

        any = {TOL2{}};

        ASSERT_FALSE(Test(any));
    }

    SIMPLE_TEST(Move)
    {
        using Any = AnyObject<
            EConstructorConcept::MoveConstructible,
            Overload<Tag<Test>, bool(This&&)>
        >;

        Any any{};

        ASSERT_FALSE(any);

        any = TOL3{};

        ASSERT_TRUE(any);

        ASSERT_TRUE(Test(std::move(any)));

        // NB: Any doesn't know if move construction consumes the object or not
        // If you want to keep track of this, use std::optional<Any>.
        ASSERT_TRUE(any);

        Any any2 = std::move(any);

        ASSERT_FALSE(any);
        ASSERT_TRUE(any2);

        ASSERT_TRUE(Test(std::move(any2)));
    }

    SIMPLE_TEST(Copy)
    {
        using Any = AnyObject<
            EConstructorConcept::CopyConstructible,
            Overload<Tag<Test>, bool(const This&)>
        >;

        Any any(TOL4{});

        ASSERT_FALSE(Test(any));

        Any any2 = any;

        ASSERT_TRUE(any);
        ASSERT_TRUE(any2);

        ASSERT_FALSE(Test(any2));
    }

    SIMPLE_TEST(Ambiguous)
    {
        using Any = AnyObject<
            EConstructorConcept::MoveConstructible
        >;

        Any any(std::move(std::in_place_type<MoveOnly<true>>), std::allocator<std::byte>{});
    }

    SIMPLE_TEST(SBODisabled)
    {
        ::TestSBO<128, false>();
        ::TestSBO<128, true>();
    }

    SIMPLE_TEST(SBOEnabled)
    {
        ::TestSBO<0, false>();
        ::TestSBO<0, true>();
    }
}

/////////////////////////////////////////////////////////////////////////

RUN_ALL_TESTS()