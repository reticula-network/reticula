#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/networks.hpp>
#include <reticula/operations/removal.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("without edges", "[reticula::without_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::without_edges(
    n1, {{{3}, {3}, 1}, {{3}, {3}, 1}, {{2}, {7, 1}, 2}});
  REQUIRE_THAT(
    res.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 4, 7}));
  REQUIRE_THAT(
    res.edges(), UnorderedRangeEquals(
                   std::vector<EdgeType>{{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}));
}

TEST_CASE("without vertices", "[reticula::without_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::without_vertices(n1, {3, 4, 8});
  REQUIRE_THAT(
    res.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 7}));
  REQUIRE_THAT(
    res.edges(), UnorderedRangeEquals(
                   std::vector<EdgeType>{{{7, 1}, {2}, 5}, {{2}, {1, 7}, 2}}));
}

TEST_CASE(
  "without isolated vertices", "[reticula::without_isolated_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{15}, {15}, 1}},
    {12});
  auto res = reticula::without_isolated_vertices(n1);
  REQUIRE_THAT(
    res.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 4, 7, 15}));
  REQUIRE_THAT(
    res.edges(), UnorderedRangeEquals(
                   std::vector<EdgeType>{
                     {{1}, {2, 4}, 1},
                     {{2}, {1, 7}, 2},
                     {{7, 1}, {2}, 5},
                     {{15}, {15}, 1}}));
}
