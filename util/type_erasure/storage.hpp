#pragma once

#include <variant>

namespace util::type_erasure::detail {

/////////////////////////////////////////////////////////////////////////

template <size_t SizeSBO, size_t AlignSBO>
using StaticStorage = std::aligned_storage_t<SizeSBO, AlignSBO>;

template <size_t SizeSBO, size_t AlignSBO>
using Storage = void*;

template <class Concrete, class T, bool AsConst = false>
decltype(auto) AsConcreteRef(std::conditional_t<AsConst, const T&, T&> storage) noexcept
{
    using Ret = std::conditional_t<AsConst, const Concrete*, Concrete*>;

    return *static_cast<Ret>(storage);
}

/////////////////////////////////////////////////////////////////////////

} // namespace util::type_erasure::detail
