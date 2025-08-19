#pragma once

#include <ranges>

/**
  This class (and the rest of this file) is just a replacement for the
  `std::ranges::views::cartesian_product` that is not available in certain
  compilers.
*/

namespace reticula {
template <std::ranges::view V1, std::ranges::view V2>
class cartesian_product_view
    : public std::ranges::view_interface<cartesian_product_view<V1, V2>> {
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

    It1 it1_;
    S1 end1_;
    It2 it2_;
    It2 begin2_;
    S2 end2_;

    using iterator_concept = std::input_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = std::tuple<
      std::ranges::range_reference_t<Base1>,
      std::ranges::range_reference_t<Base2>>;
    using value_type = std::tuple<
      std::ranges::range_value_t<Base1>, std::ranges::range_value_t<Base2>>;

    auto operator*() const -> reference { return {*it1_, *it2_}; }

    auto operator++() -> iterator& {
      ++it2_;
      if (it2_ == end2_) {
        it2_ = begin2_;
        ++it1_;
      }
      return *this;
    }
    void operator++(int) { ++*this; }
  };

  template <bool Const>
  struct sentinel {
    using Base1 = std::conditional_t<Const, const V1, V1>;
    using S1 = std::ranges::sentinel_t<Base1>;
    S1 end1_{};

    template <bool C>
    friend auto operator==(const iterator<C>& it, const sentinel& s) -> bool {
      return it.it1_ == s.end1_ || it.begin2_ == it.end2_;
    }
    template <bool C>
    friend auto operator==(const sentinel& s, const iterator<C>& it) -> bool {
      return it == s;
    }
  };

public:
  cartesian_product_view() = default;
  cartesian_product_view(V1 v1, V2 v2)
      : v1_(std::move(v1)), v2_(std::move(v2)) {}

  auto begin() {
    auto b1 = std::ranges::begin(v1_);
    auto e1 = std::ranges::end(v1_);
    auto b2 = std::ranges::begin(v2_);
    auto e2 = std::ranges::end(v2_);
    return iterator<false>{b1, e1, b2, b2, e2};
  }
  auto end() { return sentinel<false>{std::ranges::end(v1_)}; }

  auto begin() const
    requires std::ranges::range<const V1> && std::ranges::range<const V2>
  {
    auto b1 = std::ranges::begin(v1_);
    auto e1 = std::ranges::end(v1_);
    auto b2 = std::ranges::begin(v2_);
    auto e2 = std::ranges::end(v2_);
    return iterator<true>{b1, e1, b2, b2, e2};
  }
  auto end() const
    requires std::ranges::range<const V1>
  {
    return sentinel<true>{std::ranges::end(v1_)};
  }

  auto size()
    requires std::ranges::sized_range<V1> && std::ranges::sized_range<V2>
  {
    return std::ranges::size(v1_) * std::ranges::size(v2_);
  }
  auto size() const
    requires std::ranges::sized_range<const V1> &&
             std::ranges::sized_range<const V2>
  {
    return std::ranges::size(v1_) * std::ranges::size(v2_);
  }
};

namespace views {
struct cartesian_product_fn {
  template <std::ranges::viewable_range R1, std::ranges::viewable_range R2>
  auto operator()(R1&& r1, R2&& r2) const {
    using V1 = std::views::all_t<R1>;
    using V2 = std::views::all_t<R2>;
    return cartesian_product_view<V1, V2>(
      std::views::all(std::forward<R1>(r1)),
      std::views::all(std::forward<R2>(r2)));
  }
};
inline constexpr cartesian_product_fn cartesian_product;
} // namespace views
} // namespace reticula
