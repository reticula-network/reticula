#pragma once

#include <ranges>

/*^
  This class (and the rest of this file) is just a replacement for the
  `std::views::enumerate` that is not available in certain compilers.
*/


namespace reticula::views {
struct enumerate_fn {
  template <std::ranges::viewable_range R>
  constexpr auto operator()(R&& r) const {
    using V = std::views::all_t<R>;
    using D = std::ranges::range_difference_t<V>; // usually ptrdiff_t

    if constexpr (std::ranges::sized_range<V>) {
      // sized zip -> stays sized; random-access if the base is
      return std::views::zip(
        std::views::iota(D{0}, static_cast<D>(std::ranges::size(r))),
        std::views::all(std::forward<R>(r)));
    } else {
      // open-ended iota; zip stops when the range ends
      return std::views::zip(
        std::views::iota(D{0}), std::views::all(std::forward<R>(r)));
    }
  }

  // Allow piping: rng | views::enumerate
  template <std::ranges::viewable_range R>
  friend constexpr auto operator|(R&& r, enumerate_fn f) {
    return f(std::forward<R>(r));
  }
};

inline constexpr enumerate_fn enumerate{};
} // namespace reticula::views

