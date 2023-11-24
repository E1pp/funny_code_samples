#pragma once

#include <concepts>
#include <utility>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace tag_invoke_fn_ns {

// "poison pill" to hide overloads of TagInvoke possibly found in parent namespaces.
// We limit to overloads to the ones found by ADL.
void TagInvoke() = delete;

struct TagInvokeFunction {
    template <class Tag, class... Args>
        requires requires (Tag&& tag, Args&&... args) {
            TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
        }
    constexpr decltype(auto) operator()(Tag&& tag, Args&&... args) const
    noexcept(noexcept(TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...)))
    {
        return TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
    }
};

} // namespace tag_invoke_fn_ns

// Add inline namespace to avoid conflict if someone in util namespace
// defines customization of TagInvoke via hidden friends.
inline namespace inline_tag_invoke_ns {
    inline constexpr tag_invoke_fn_ns::TagInvokeFunction TagInvoke = {}; // NOLINT
} // inline namespace inline_tag_invoke_ns

/////////////////////////////////////////////////////////////////////////

template <class Tag, class... Args>
concept TagInvocable = requires (Tag&& tag, Args&&... args)
{
    TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
};

template <class Tag, class... Args>
concept NothrowTagInvocable = 
    TagInvocable<Tag, Args...> &&
    requires (Tag&& tag, Args&&... args)
    {
        { TagInvoke(std::forward<Tag>(tag), std::forward<Args>(args)...) } noexcept;
    };

/////////////////////////////////////////////////////////////////////////

template <class Tag, class... Args>
using TagInvokeResult = std::invoke_result_t<decltype(TagInvoke), Args...>;

template <auto TagValue>
using Tag = std::decay_t<decltype(TagValue)>;

/////////////////////////////////////////////////////////////////////////

} // namespace util
