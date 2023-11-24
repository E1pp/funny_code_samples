#pragma once

namespace util {

/////////////////////////////////////////////////////////////////////////

namespace detail {

template <class What>
struct DecayImpl
{
    using Type = What;
};

template <class What>
struct DecayImpl<const What>
{
    using Type = What;
};

template <class What>
struct DecayImpl<What&>
{
    using Type = What;
};

template <class What>
struct DecayImpl<const What&>
{
    using Type = What;
};

template <class What>
struct DecayImpl<What*>
{
    using Type = What;
};

template <class What>
struct DecayImpl<const What*>
{
    using Type = What;
};

template <class What>
struct DecayImpl<What&&>
{
    using Type = What;
};

// Keep function pointer as is.
template <class Ret, bool NoExcept, class... Args>
struct DecayImpl<Ret (*)(Args...) noexcept(NoExcept)>
{
    using Type = Ret (*)(Args...) noexcept(NoExcept);
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////

// const/ref
template <class What>
using Decay = typename detail::DecayImpl<What>::Type;

/////////////////////////////////////////////////////////////////////////

} // namespace util
