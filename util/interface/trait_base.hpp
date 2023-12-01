#pragma once

#include "fragment.hpp"

namespace util {

/////////////////////////////////////////////////////////////////////////

template <class Derived>
struct TraitBase
{
public:
    template <detail::TypeErasableSignature Sig>
    static consteval auto Fragment() noexcept
    {
        return ::util::Fragment<Derived, Sig>{};
    }

private:
    TraitBase() = delete;
    ~TraitBase() = default;

    template <class... Args>
    explicit TraitBase(Args&&...) {};

    template <class... Args>
    TraitBase& operator= (Args&&...) { return *this; };

    TraitBase& operator= (const TraitBase&) { return *this; };
};

/////////////////////////////////////////////////////////////////////////

template <class Derived>
struct ImplementorBase
{
    template <class Tag, class... Args>
    static constexpr bool HasRefMethod = requires (Derived& drv, Tag tag, Args&&... args) // NOLINT
    {
        drv.Implement(tag, std::forward<Args>(args)...);
    };

    template <class Tag, class... Args>
    static constexpr bool HasCRefMethod = requires (const Derived& drv, Tag tag, Args&&... args) // NOLINT
    {
        drv.Implement(tag, std::forward<Args>(args)...);
    };

    template <class Tag, class... Args>
    static constexpr bool HasRValueRefMethod = requires (Derived&& drv, Tag tag, Args&&... args) // NOLINT
    {
        std::move(drv).Implement(tag, std::forward<Args>(args)...);
    };

    template <class Tag, class... Args>
        requires HasRefMethod<Tag, Args...>
    friend constexpr auto TagInvoke(Tag tag, Derived& drv, Args&&... args)
        noexcept(noexcept(drv.Implement(tag, std::forward<Args>(args)...)))
    {
        return drv.Implement(tag, std::forward<Args>(args)...);
    }

    template <class Tag, class... Args>
        requires HasCRefMethod<Tag, Args...>
    friend constexpr auto TagInvoke(Tag tag, const Derived& drv, Args&&... args)
        noexcept(noexcept(drv.Implement(tag, std::forward<Args>(args)...)))
    {
        return drv.Implement(tag, std::forward<Args>(args)...);
    }

    template <class Tag, class... Args>
        requires HasRValueRefMethod<Tag, Args...>
    friend constexpr auto TagInvoke(Tag tag, Derived&& drv, Args&&... args)
        noexcept(noexcept(std::move(drv).Implement(tag, std::forward<Args>(args)...)))
    {
        return std::move(drv).Implement(tag, std::forward<Args>(args)...);
    }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util
