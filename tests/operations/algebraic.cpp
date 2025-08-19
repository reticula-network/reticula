#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/networks.hpp>
#include <reticula/operations/algebraic.hpp>

using Catch::Matchers::Contains;
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

// TEST_CASE("cartesian product", "[reticula::cartesian_product]") {
//   using EdgeType = reticula::directed_temporal_hyperedge;
//   reticula::network<EdgeType> n1(
//     {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
//
//   reticula::network<EdgeType> n2(
//     {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});
//
//   auto res1 = reticula::cartesian_product(n1, n2);
//   auto res2 = reticula::cartesian_product(n1, n2);
//
//   REQUIRE(res1.edges().size() == n1.edges().size() * n2.edges().size());
//   REQUIRE(res2.edges().size() == n1.edges().size() * n2.edges().size());
// }

TEST_CASE("graph intersection", "[reticula::graph_intersection]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  reticula::network<EdgeType> n2(
    {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = reticula::graph_intersection(n1, n2);
  auto res2 = reticula::graph_intersection(n1, n2);

  REQUIRE(res1 == res2);
  REQUIRE_THAT(
    res1.edges(),
    UnorderedRangeEquals(std::vector<EdgeType>{{{7, 1}, {2}, 5}}));
  REQUIRE_THAT(
    res1.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 4, 7}));
}

TEST_CASE("disjoint union", "[reticula::disjoint_union]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  reticula::network<EdgeType> n2(
    {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = reticula::disjoint_union(n1, n2);
  auto res2 = reticula::disjoint_union(n1, n2);

  REQUIRE(res1.edges().size() == n1.edges().size() + n2.edges().size());
  REQUIRE(res2.edges().size() == n1.edges().size() + n2.edges().size());
  REQUIRE(
    res1.vertices().size() == n1.vertices().size() + n2.vertices().size());
  REQUIRE(
    res2.vertices().size() == n1.vertices().size() + n2.vertices().size());
}

TEST_CASE("graph union", "[reticula::graph_union]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  reticula::network<EdgeType> n2(
    {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = reticula::graph_union(n1, n2);
  auto res2 = reticula::graph_union(n1, n2);
  REQUIRE_THAT(res1.edges(), RangeEquals(res2.edges()));
  for (const auto& edge : n1.edges())
    REQUIRE_THAT(res1.edges(), Contains(edge));
  for (const auto& edge : n2.edges())
    REQUIRE_THAT(res1.edges(), Contains(edge));

  REQUIRE_THAT(res1.vertices(), RangeEquals(res2.vertices()));
  for (const auto& vertex : n1.vertices())
    REQUIRE_THAT(res1.vertices(), Contains(vertex));
  for (const auto& vertex : n2.vertices())
    REQUIRE_THAT(res1.vertices(), Contains(vertex));
}
