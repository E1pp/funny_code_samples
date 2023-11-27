#pragma once

#include <util/arcane/reflector_base.hpp>

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

} // namespace util
