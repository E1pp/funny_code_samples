#pragma once

#include <concepts>

namespace util {

/////////////////////////////////////////////////////////////////////////

template <class... Ts>
struct Pack
{ };

template <class T>
concept AnyPack = requires (T& t) {
    [] <class... Ts> (Pack<Ts...>&) {} (t);
};

/////////////////////////////////////////////////////////////////////////

} // namespace util
