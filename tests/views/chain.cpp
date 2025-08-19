#include <catch2/catch_test_macros.hpp>

#include <ranges>
#include <vector>

#include <reticula/views/chain.hpp>

TEST_CASE("chain_view: concatenates two ranges in order") {
  std::vector<int> a{1, 2};
  std::vector<int> b{3, 4};

  auto v = reticula::views::chain(a, b);

  STATIC_REQUIRE(std::ranges::input_range<decltype(v)>);
  STATIC_REQUIRE_FALSE(std::ranges::forward_range<decltype(v)>);

  REQUIRE(v.size() == a.size() + b.size());

  std::vector<int> got;
  for (auto x : v)
    got.push_back(x);

  REQUIRE(got == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("chain_view: empty-first behaves like second") {
  std::vector<int> a;
  std::vector<int> b{5, 6};

  auto v = reticula::views::chain(a, b);
  std::vector<int> got;
  for (auto x : v)
    got.push_back(x);

  REQUIRE(got == b);
  REQUIRE(v.size() == b.size());
}

TEST_CASE("chain_view: empty-second behaves like first") {
  std::vector<int> a{7, 8};
  std::vector<int> b;

  auto v = reticula::views::chain(a, b);
  std::vector<int> got;
  for (auto x : v)
    got.push_back(x);

  REQUIRE(got == a);
  REQUIRE(v.size() == a.size());
}

TEST_CASE("chain_view: common reference and write-through") {
  std::vector<int> a{1};
  std::vector<int> b{2};

  auto v = reticula::views::chain(a, b);
  auto it = std::ranges::begin(v);

  using Ref = decltype(*it);
  STATIC_REQUIRE(std::same_as<Ref, int&>);

  *it = 42;
  REQUIRE(a[0] == 42);

  ++it;
  *it = 99;
  REQUIRE(b[0] == 99);
}

TEST_CASE("chain_view: const iteration and reference type") {
  std::vector<int> a{1, 2};
  std::vector<int> b{3};

  const auto v = reticula::views::chain(a, b);
  STATIC_REQUIRE(std::ranges::range<const decltype(v)>);

  using CRef = std::ranges::range_reference_t<const decltype(v)>;
  STATIC_REQUIRE(std::is_reference_v<CRef>);
  STATIC_REQUIRE(std::same_as<std::remove_reference_t<CRef>, int>);

  std::vector<int> got;
  for (auto x : v)
    got.push_back(x);
  REQUIRE(got == std::vector<int>{1, 2, 3});
}

TEST_CASE("chain_view: not sized if one side unsized") {
  auto r1 = std::views::iota(0, 10) |
            std::views::filter([](int) { return true; });
  auto r2 = std::views::iota(0, 3);

  auto v = reticula::views::chain(r1, r2);
  STATIC_REQUIRE_FALSE(std::ranges::sized_range<decltype(v)>);
}
