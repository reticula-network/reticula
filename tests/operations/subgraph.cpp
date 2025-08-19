#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/networks.hpp>
#include <reticula/operations/subgraph.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("vertex induced subgraph", "[reticula::vertex_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> network(
    {{{1}, {2, 4}, 1},
     {{2}, {1, 7}, 2},
     {{7, 1}, {2}, 5},
     {{2}, {3, 1}, 6},
     {{3, 4}, {4}, 8}});

  SECTION("with vector") {
    std::vector<reticula::VertexType> comp({1, 2, 3, 4, 9});
    auto res = reticula::vertex_induced_subgraph(network, comp);

    REQUIRE_THAT(
      std::vector<reticula::VertexType>({1, 2, 3, 4}),
      UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(
      std::vector<EdgeType>(
        {{{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
      UnorderedRangeEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::vertex_induced_subgraph(network, {1, 2, 3, 4, 9});

    REQUIRE_THAT(
      std::vector<reticula::VertexType>({1, 2, 3, 4}),
      UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(
      std::vector<EdgeType>(
        {{{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
      UnorderedRangeEquals(res.edges()));
  }
}

TEST_CASE("edge induced subgraph", "[reticula::edge_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  reticula::network<EdgeType> network(
    {{{1}, {2, 4}, 1},
     {{2}, {1, 7}, 2},
     {{7, 1}, {2}, 5},
     {{2}, {3, 1}, 6},
     {{3, 4}, {4}, 8}});

  SECTION("with vector") {
    std::vector<EdgeType> comp(
      {{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});
    auto res = reticula::edge_induced_subgraph(network, comp);

    REQUIRE_THAT(
      std::vector<reticula::VertexType>({1, 2, 3, 4, 7}),
      UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(
      std::vector<EdgeType>({{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
      UnorderedRangeEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::edge_induced_subgraph(
      network, {{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});

    REQUIRE_THAT(
      std::vector<reticula::VertexType>({1, 2, 3, 4, 7}),
      UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(
      std::vector<EdgeType>({{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
      UnorderedRangeEquals(res.edges()));
  }
}
