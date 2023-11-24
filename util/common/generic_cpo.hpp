#include "tag_invoke.hpp"

namespace util {

/////////////////////////////////////////////////////////////////////////

// CPO which doesn't have any default behavior.
// Ideally you should define unique type for every CPO.
// Since if type overloads TagInvoke for this one, every CPO of this type will be invocable
// with these arguments even if the user meant to customize the call only for a particular instance.
struct GenericCPO
{
    template <class... Args>
        requires TagInvocable<GenericCPO, Args...>
    constexpr auto operator()(Args&&... args) const
    noexcept(NothrowTagInvocable<GenericCPO, Args...>)
    {
        return TagInvoke(*this, std::forward<Args>(args)...);
    }
};

/////////////////////////////////////////////////////////////////////////

} // namespace util
