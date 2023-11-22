#include <concepts>
#include <utility>

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace tag_invoke_fn_ns {

// "poison pill" to hide overloads of TagInvoke possibly found in parent namespaces.
// We limit to overloads to the ones found by ADL.
void TagInvoke() = delete;

struct TagInvokeFunction {
    template <class __Tag, class... Args>
        requires requires (__Tag&& tag, Args&&... args) {
            TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...);
        }
    constexpr decltype(auto) operator()(__Tag&& tag, Args&&... args) const
    noexcept(noexcept(TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...)))
    {
        return TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...);
    }
};

} // namespace tag_invoke_fn_ns

// Add inline namespace to avoid conflict if someone in util namespace
// defines customization of TagInvoke via hidden friends.
inline namespace inline_tag_invoke_ns {
    inline constexpr tag_invoke_fn_ns::TagInvokeFunction TagInvoke = {};
} // inline namespace inline_tag_invoke_ns

/////////////////////////////////////////////////////////////////////////

template <class __Tag, class... Args>
concept TagInvocable = requires (__Tag&& tag, Args&&... args)
{
    TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...);
};

template <class __Tag, class... Args>
concept NothrowTagInvocable = 
    TagInvocable<__Tag, Args...> &&
    requires (__Tag&& tag, Args&&... args)
    {
        { TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...) } noexcept;
    };

/////////////////////////////////////////////////////////////////////////

template <class __Tag, class Ret, class... Args>
concept ValidThrowingTagInvokeSignature = requires (__Tag&& tag, Args&&... args)
{
    { TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...) } -> std::same_as<Ret>;
};

template <class __Tag, class Ret, class... Args>
concept ValidNothrowTagInvokeSignature = requires (__Tag&& tag, Args&&... args)
{
    { TagInvoke(std::forward<__Tag>(tag), std::forward<Args>(args)...) } noexcept -> std::same_as<Ret>;
};

template <class __Tag, class Ret, bool IsNoexcept, class... Args>
concept ValidTagInvokeSignature = 
    (IsNoexcept ?
    ValidNothrowTagInvokeSignature<__Tag, Ret, Args...> :
    ValidThrowingTagInvokeSignature<__Tag, Ret, Args...>);

/////////////////////////////////////////////////////////////////////////

template <class __Tag, class... Args>
using TagInvokeResult = std::invoke_result_t<decltype(TagInvoke), Args...>;

template <auto __Tag>
using Tag = std::decay_t<decltype(__Tag)>;

/////////////////////////////////////////////////////////////////////////

} // namespace util