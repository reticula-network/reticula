#pragma once

#include <ranges>

/**
  This class (and the rest of this file) is just a replacement for the
  `std::ranges::views::concat` functionality that is not available in C++23.
*/

namespace reticula {
template <std::ranges::view V1, std::ranges::view V2>
  requires requires {
    typename std::common_reference_t<
      std::ranges::range_reference_t<V1>, std::ranges::range_reference_t<V2>>;
  }
class chain_view : public std::ranges::view_interface<chain_view<V1, V2>> {
  V1 v1_;
  V2 v2_;

  template <bool Const>
  struct iterator {
    using Base1 = std::conditional_t<Const, const V1, V1>;
    using Base2 = std::conditional_t<Const, const V2, V2>;
    using It1 = std::ranges::iterator_t<Base1>;
    using S1 = std::ranges::sentinel_t<Base1>;
    using It2 = std::ranges::iterator_t<Base2>;
    using S2 = std::ranges::sentinel_t<Base2>;

    It1 it1_{};
    S1 end1_{};
    It2 it2_{};
    S2 end2_{};
    bool in_first_{true};

    using iterator_concept = std::input_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;

    using reference = std::common_reference_t<
      std::ranges::range_reference_t<Base1>,
      std::ranges::range_reference_t<Base2>>;

    using value_type = std::common_type_t<
      std::ranges::range_value_t<Base1>, std::ranges::range_value_t<Base2>>;

    auto operator*() const -> reference {
      if (in_first_)
        return static_cast<reference>(*it1_);
      return static_cast<reference>(*it2_);
    }

    auto operator++() -> iterator& {
      if (in_first_) {
        ++it1_;
        if (it1_ == end1_)
          in_first_ = false;
      } else {
        ++it2_;
      }
      return *this;
    }
    void operator++(int) { ++*this; }
  };

  template <bool Const>
  struct sentinel {
    using Base2 = std::conditional_t<Const, const V2, V2>;
    using S2 = std::ranges::sentinel_t<Base2>;
    S2 end2_{};

    template <bool C>
    friend auto operator==(const iterator<C>& it, const sentinel& s) -> bool {
      // End only when we've switched to the second range and reached its end.
      return !it.in_first_ && it.it2_ == s.end2_;
    }
    template <bool C>
    friend auto operator==(const sentinel& s, const iterator<C>& it) -> bool {
      return it == s;
    }
  };

public:
  chain_view() = default;
  chain_view(V1 v1, V2 v2) : v1_(std::move(v1)), v2_(std::move(v2)) {}

  auto begin() {
    auto b1 = std::ranges::begin(v1_);
    auto e1 = std::ranges::end(v1_);
    auto b2 = std::ranges::begin(v2_);
    auto e2 = std::ranges::end(v2_);
    bool in_first = (b1 != e1);
    if (!in_first) { // first is empty → start in second
      return iterator<false>{b1, e1, b2, e2, false};
    }
    return iterator<false>{b1, e1, b2, e2, true};
  }
  auto end() { return sentinel<false>{std::ranges::end(v2_)}; }

  auto begin() const
    requires std::ranges::range<const V1> && std::ranges::range<const V2>
  {
    auto b1 = std::ranges::begin(v1_);
    auto e1 = std::ranges::end(v1_);
    auto b2 = std::ranges::begin(v2_);
    auto e2 = std::ranges::end(v2_);
    bool in_first = (b1 != e1);
    if (!in_first) {
      return iterator<true>{b1, e1, b2, e2, false};
    }
    return iterator<true>{b1, e1, b2, e2, true};
  }
  auto end() const
    requires std::ranges::range<const V2>
  {
    return sentinel<true>{std::ranges::end(v2_)};
  }

  // size() available when both are sized
  auto size()
    requires std::ranges::sized_range<V1> && std::ranges::sized_range<V2>
  {
    return std::ranges::size(v1_) + std::ranges::size(v2_);
  }
  auto size() const
    requires std::ranges::sized_range<const V1> &&
             std::ranges::sized_range<const V2>
  {
    return std::ranges::size(v1_) + std::ranges::size(v2_);
  }
};

namespace views {
struct chain_fn {
  template <std::ranges::viewable_range R1, std::ranges::viewable_range R2>
    requires requires {
      typename std::common_reference_t<
        std::ranges::range_reference_t<std::views::all_t<R1>>,
        std::ranges::range_reference_t<std::views::all_t<R2>>>;
    }
  auto operator()(R1&& r1, R2&& r2) const {
    using V1 = std::views::all_t<R1>;
    using V2 = std::views::all_t<R2>;
    return chain_view<V1, V2>(
      std::views::all(std::forward<R1>(r1)),
      std::views::all(std::forward<R2>(r2)));
  }
};
inline constexpr chain_fn chain;
} // namespace views
} // namespace reticula
