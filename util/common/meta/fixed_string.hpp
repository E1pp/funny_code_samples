#pragma once

#include <array>
#include <cstdlib>
#include <string_view>

namespace util {

/////////////////////////////////////////////////////////////////////////

template<size_t max_length>
struct FixedString {
  constexpr FixedString(const char* string, size_t length) noexcept
  {
    for (size_t idx = 0; idx < length; ++idx) {
      impl[idx] = string[idx];
    }

    for (size_t idx = length; idx <= max_length; ++idx) {
      impl[idx] = '\0';
    }
  }

  constexpr explicit FixedString(std::string_view str) noexcept
    : FixedString(str.data(), str.size())
  { }

  constexpr operator std::string_view() const // NOLINT
  {
    size_t endpos = 0;

    while (endpos <= max_length && impl[endpos] != '\0') {
      endpos++;
    }

    return {impl.begin(), impl.begin() + endpos};
  }

  template <std::size_t that_len>
  friend constexpr bool operator==(const FixedString<max_length>& a, const FixedString<that_len>& b)
  {
    return a.impl == b.impl;
  }

  std::array<char, max_length + 1> impl;
};

/////////////////////////////////////////////////////////////////////////

constexpr FixedString<256> operator ""_cstr(const char* string, std::size_t size)
{
  return FixedString<256>(string, size);
}

/////////////////////////////////////////////////////////////////////////

} // namespace util
