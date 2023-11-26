#include "mock.hpp"

#include <iostream>

namespace util {

/////////////////////////////////////////////////////////////////////////

class TMock
{
public:
    TMock() = default;

    friend void TagInvoke(Tag<MockTraits::Foo>, TMock&, int num) noexcept
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
