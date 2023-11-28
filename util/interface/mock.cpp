#include "mock.hpp"

#include <iostream>

namespace util {

/////////////////////////////////////////////////////////////////////////

class TMock
    : private ImplementorBase<TMock>
{
public:
    TMock() = default;

    void Implement(Tag<MockTraits::Foo>, int num) const noexcept
    {
        std::cout << "Foo from .cpp -> " << num << '\n';
    }
};

/////////////////////////////////////////////////////////////////////////

MockTraits::IMock CreateMock() noexcept
{
    return MockTraits::IMock(TMock{});
}

} // namespace util
