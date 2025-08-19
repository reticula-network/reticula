#include <catch2/catch_test_macros.hpp>

#include <ranges>
#include <vector>

#include <reticula/views/cartesian_product.hpp>

TEST_CASE("cartesian_product_view: basic iteration and values") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b{10, 20};

  auto v = reticula::views::cartesian_product(a, b);

  STATIC_REQUIRE(std::ranges::input_range<decltype(v)>);
  STATIC_REQUIRE_FALSE(std::ranges::forward_range<decltype(v)>);

  REQUIRE(v.size() == a.size() * b.size());

  std::vector<std::tuple<int, int>> got;
  for (auto [x, y] : v)
    got.emplace_back(x, y);

  std::vector<std::tuple<int, int>> expected{{1, 10}, {1, 20}, {2, 10},
                                             {2, 20}, {3, 10}, {3, 20}};
  REQUIRE(got == expected);
}

TEST_CASE("cartesian_product_view: empty handling") {
  std::vector<int> a;
  std::vector<int> b{7, 8};

  auto v1 = reticula::views::cartesian_product(a, b);
  REQUIRE(std::ranges::begin(v1) == std::ranges::end(v1));
  std::vector<int> c{1};
  std::vector<int> d;
  auto v2 = reticula::views::cartesian_product(c, d);
  REQUIRE(std::ranges::begin(v2) == std::ranges::end(v2));
}

TEST_CASE("cartesian_product_view: tuple of references and write-through") {
  std::vector<int> a{1, 2};
  std::vector<int> b{10, 20};

  auto v = reticula::views::cartesian_product(a, b);
  auto it = std::ranges::begin(v);

  using Ref = decltype(*it);
  STATIC_REQUIRE(std::same_as<Ref, std::tuple<int&, int&>>);

  auto t = *it;
  std::get<0>(t) = 42;
  std::get<1>(t) = 99;

  REQUIRE(a[0] == 42);
  REQUIRE(b[0] == 99);
}

TEST_CASE("cartesian_product_view: const iteration works") {
  const std::vector<int> a{1, 2};
  const std::vector<int> b{3};

  auto v = reticula::views::cartesian_product(a, b);
  STATIC_REQUIRE(std::ranges::range<const decltype(v)>);

  std::vector<std::tuple<int, int>> got;
  for (const auto [x, y] : v)
    got.emplace_back(x, y);

  REQUIRE(got == std::vector<std::tuple<int, int>>{{1, 3}, {2, 3}});
}

TEST_CASE("cartesian_product_view: laziness with iota and take") {
  auto r1 = std::views::iota(0, 2);
  auto r2 = std::views::iota(10, 13);

  auto v = reticula::views::cartesian_product(r1, r2);

  std::vector<std::tuple<int, int>> got;
  for (auto [x, y] : v)
    got.emplace_back(x, y);

  std::vector<std::tuple<int, int>> expected{{0, 10}, {0, 11}, {0, 12},
                                             {1, 10}, {1, 11}, {1, 12}};
  REQUIRE(got == expected);

  STATIC_REQUIRE(std::ranges::sized_range<decltype(v)>);
  REQUIRE(v.size() == 6);
}

TEST_CASE("cartesian_product_view: not sized if one side unsized") {
  auto r1 = std::views::iota(0, 5) |
            std::views::filter([](int) { return true; }); // not a sized_range
  auto r2 = std::views::iota(0, 3);                       // sized

  auto v = reticula::views::cartesian_product(r1, r2);
  STATIC_REQUIRE_FALSE(std::ranges::sized_range<decltype(v)>);
}
