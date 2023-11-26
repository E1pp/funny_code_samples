#pragma once

#include <memory>

/////////////////////////////////////////////////////////////////////////

template<class T, bool reallocate>
class TrackerAllocator : private std::allocator<T> {
public:
  TrackerAllocator() = default;
  TrackerAllocator(std::size_t id) : id_{id} {}  // NOLINT

  friend bool operator==(const TrackerAllocator&, const TrackerAllocator&) = default;

  using propagate_on_container_copy_assignment = std::integral_constant<bool, !reallocate>; // NOLINT
  using propagate_on_container_move_assignment = std::integral_constant<bool, !reallocate>; // NOLINT

  using value_type = T; // NOLINT

  template<class U>
  struct rebind { using other = TrackerAllocator<U, reallocate>; }; // NOLINT

  [[nodiscard]] constexpr T* allocate(std::size_t n) { // NOLINT
    ++allocation_counter;
    return std::allocator_traits<std::allocator<T>>::allocate(*this, n);
  }

  constexpr void deallocate(T* p, std::size_t n) { // NOLINT
    return std::allocator_traits<std::allocator<T>>::deallocate(*this, p, n);
  }

  template<class U, class... Args>
  constexpr void construct(U* p, Args&&... args) { // NOLINT
    ++placement_counter;
    std::allocator_traits<std::allocator<T>>::template construct<U>(*this, p, std::forward<Args>(args)...);
  }

  template<class U>
  constexpr void destroy(U* p) { // NOLINT
    std::allocator_traits<std::allocator<T>>::template destroy<U>(*this, p);
  }

  static void ResetCounters() {
    allocation_counter = 0;
    placement_counter = 0;
  }

  static std::size_t AllocCount() { return allocation_counter; }
  static std::size_t PlacementCount() { return placement_counter; }

private:
  std::size_t id_ = 0;
  inline static std::size_t allocation_counter = 0;
  inline static std::size_t placement_counter = 0;
};

/////////////////////////////////////////////////////////////////////////
