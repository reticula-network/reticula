#include <cstddef>
#include <ranges>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Matchers::RangeEquals;

#include <reticula/views/enumerate.hpp>

TEST_CASE("enumerate_view: enumerates a range") {
  std::vector<int> v{1, 2, 3, 4};

  auto ev = reticula::views::enumerate(v);

  STATIC_REQUIRE(std::ranges::input_range<decltype(ev)>);

  REQUIRE(ev.size() == v.size());

  std::size_t idx = 0;
  for (auto&& [i, x] : ev) {
    REQUIRE(x == v[idx]);
    REQUIRE(i == static_cast<ptrdiff_t>(idx++));
  }
}

TEST_CASE("enumerate_view: enumerates an empty range") {
  std::vector<int> v;

  auto ev = reticula::views::enumerate(v);

  STATIC_REQUIRE(std::ranges::input_range<decltype(ev)>);

  REQUIRE(ev.size() == v.size());
  REQUIRE(std::ranges::empty(ev));
}

TEST_CASE("enumerate_view: pipable") {
  std::vector<int> v{5, 6, 7};

  auto ev = v | reticula::views::enumerate;

  STATIC_REQUIRE(std::ranges::input_range<decltype(ev)>);

  REQUIRE(ev.size() == v.size());

  std::size_t idx = 0;
  for (auto&& [i, x] : ev) {
    REQUIRE(x == v[idx]);
    REQUIRE(i == static_cast<ptrdiff_t>(idx++));
  }
}
