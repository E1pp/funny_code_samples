#pragma once

#include <algorithm>
#include <cstdlib>
#include <concepts>
#include <string_view>

namespace util {

template <size_t N>
struct EnvDeclarator
{
    friend consteval auto GetEnvImpl(EnvDeclarator<N>);
};

template <class EnvTypeList, size_t N>
struct EnvInstantiator
{
    friend consteval auto GetEnvImpl(EnvDeclarator<N>)
    {
        return EnvTypeList{};
    }

    static constexpr size_t EnvIndex = N;
};

struct NullEnvironment
{ 
    using Outer = NullEnvironment;
    using Current = NullEnvironment;
};

template struct EnvDeclarator<0>;
template struct EnvInstantiator<NullEnvironment, 0>;

/////////////////////////////////////////////////////////////////////////

template <class Prev, class Curr>
struct Cons
{
    using Outer = Prev;
    using Current = Curr;
};

template <auto Tag, size_t Next = 0>
consteval auto GetCurrentEnvListImpl()
{
    constexpr bool ShouldBreak = !requires(EnvDeclarator<Next> ed) {
        GetEnvImpl(ed);
    };

    if constexpr (ShouldBreak) {
        return GetEnvImpl(EnvDeclarator<Next - 1>{});
    } else {
        return GetCurrentEnvListImpl<Tag, Next + 1>();
    }
}

template <auto Tag = []{}>
constexpr auto GetCurrentEnvList()
{
    return GetCurrentEnvListImpl<Tag>();
}

template <auto Tag>
using CurrentEnvList = std::decay_t<decltype(GetCurrentEnvList<Tag>())>;

template <auto Tag>
using CurrentEnv = typename CurrentEnvList<Tag>::Current;

template <auto Tag>
using OuterEnv = typename CurrentEnvList<Tag>::Outer::Current;

template <auto Tag = []{}>
consteval auto GetCurrentEnv()
{
    return CurrentEnv<Tag>{};
}

template <class NextList, auto Tag, size_t Next = 0>
consteval auto BeginEnvImpl()
{
    constexpr bool ShouldBreak = !requires (EnvDeclarator<Next> ed) {
        GetEnvImpl(ed);
    };

    if constexpr (ShouldBreak) {
        [[maybe_unused]] EnvInstantiator<NextList, Next> unused;
        return typename NextList::Current{};
    } else {
        return BeginEnvImpl<NextList, Tag, Next + 1>();
    }
}

template <class OldEnv, auto Tag, size_t Next = 0>
consteval auto EndEnvImpl()
{
    constexpr bool ShouldBreak = !requires (EnvDeclarator<Next> ed) {
        GetEnvImpl(ed);
    };

    if constexpr (ShouldBreak) {
        static_assert(Next > 0);

        using CurrentEnvList = std::decay_t<decltype(GetEnvImpl(EnvDeclarator<Next - 1>{}))>;
        using CurrentEnv = typename CurrentEnvList::Current;
        using NextList = typename CurrentEnvList::Outer;

        static_assert(std::same_as<OldEnv, CurrentEnv>);

        [[maybe_unused]] EnvInstantiator<NextList, Next> unused;
        return CurrentEnv{};
    } else {
        return EndEnvImpl<OldEnv, Tag, Next + 1>();
    }
}

template <class Env, auto Tag>
constexpr auto PrintEnvImpl()
{
    constexpr std::string_view sv = __PRETTY_FUNCTION__;
    constexpr auto beg1 = std::find(sv.begin(), sv.end(), '[');
    constexpr auto beg = std::find(beg1, sv.end(), '=');
    constexpr auto end = std::find(beg, sv.end(), ',');

    return std::string_view(beg + 2, end);
}

template <class NewEnv, auto Tag = []{}>
constexpr auto BeginEnv()
{
    return BeginEnvImpl<Cons<CurrentEnvList<Tag>, NewEnv>, Tag>();
}

template <class OldEnv, auto Tag = []{}>
constexpr auto EndEnv()
{
    return EndEnvImpl<OldEnv, Tag>();
}

template <class Env, auto Tag = []{}>
constexpr bool IsCurrentEnv()
{
    return std::same_as<std::decay_t<decltype(GetCurrentEnv<Tag>())>, std::decay_t<Env>>;
}

template <class Env, auto Tag = []{}>
constexpr void VerifyEnv()
{
    static_assert(IsCurrentEnv<Env, Tag>());
}

template <auto Tag = []{}>
constexpr auto PrintEnv()
{
    return PrintEnvImpl<CurrentEnv<Tag>, Tag>();
}

/////////////////////////////////////////////////////////////////////////

} // namespace util

