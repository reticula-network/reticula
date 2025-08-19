#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/networks.hpp>
#include <reticula/operations/addition.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("with edges", "[reticula::with_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_edges(
    n1, {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});
  REQUIRE_THAT(
    res.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 3, 4, 7}));
  REQUIRE_THAT(
    res.edges(), UnorderedRangeEquals(
                   std::vector<EdgeType>{
                     {{1}, {2, 4}, 1},
                     {{2}, {1, 7}, 2},
                     {{7, 1}, {2}, 5},
                     {{2}, {3, 1}, 6},
                     {{3, 4}, {4}, 8}}));
}

TEST_CASE("with vertices", "[reticula::with_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_vertices(n1, {3, 4, 5, 6});
  REQUIRE_THAT(
    res.vertices(), UnorderedRangeEquals(
                      std::vector<reticula::VertexType>{1, 2, 3, 4, 5, 6, 7}));
  REQUIRE_THAT(
    res.edges(), UnorderedRangeEquals(
                   std::vector<EdgeType>{
                     {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}
