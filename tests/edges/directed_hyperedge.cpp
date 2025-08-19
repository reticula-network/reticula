#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/edges/directed_hyperedge.hpp>

#include "../stringify.hpp"

TEST_CASE("directed hyperedges", "[reticula::directed_hyperedge]") {
  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(reticula::static_network_edge<reticula::directed_hyperedge>);
  }

  SECTION("have correct properties") {
    reticula::directed_hyperedge edge({1, 2, 3}, {3, 4});

    REQUIRE_THAT(
      edge.mutated_verts(),
      RangeEquals(std::vector<reticula::VertexType>({3, 4})));
    REQUIRE_THAT(
      edge.heads(), RangeEquals(std::vector<reticula::VertexType>({3, 4})));
    REQUIRE_THAT(
      edge.mutator_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
    REQUIRE_THAT(
      edge.tails(), RangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
    REQUIRE_THAT(
      edge.incident_verts(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(4));

    REQUIRE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(5));
    REQUIRE_FALSE(edge.is_out_incident(5));

    REQUIRE(
      reticula::adjacent(edge, reticula::directed_hyperedge({4, 5}, {6})));
    REQUIRE_FALSE(
      reticula::adjacent(edge, reticula::directed_hyperedge({1, 2}, {3, 4})));
    REQUIRE_FALSE(
      reticula::adjacent(edge, reticula::directed_hyperedge({12}, {45})));

    REQUIRE_NOTHROW(reticula::hash<reticula::directed_hyperedge>{}(edge));
    REQUIRE_NOTHROW(std::hash<reticula::directed_hyperedge>{}(edge));
    REQUIRE_NOTHROW(
      hll::hash<reticula::directed_hyperedge>{}(edge, std::uint64_t{}));
  }

  SECTION("handle large directed hyperedges correctly") {
    reticula::directed_hyperedge large_edge({1, 2, 3, 4, 5}, {6, 7, 8, 9});

    REQUIRE_THAT(
      large_edge.tails(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));
    REQUIRE_THAT(
      large_edge.heads(),
      RangeEquals(std::vector<reticula::VertexType>({6, 7, 8, 9})));
    REQUIRE_THAT(
      large_edge.incident_verts(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6, 7, 8, 9})));

    REQUIRE(large_edge.is_out_incident(1));
    REQUIRE(large_edge.is_out_incident(5));
    REQUIRE_FALSE(large_edge.is_out_incident(6));

    REQUIRE(large_edge.is_in_incident(6));
    REQUIRE(large_edge.is_in_incident(9));
    REQUIRE_FALSE(large_edge.is_in_incident(1));

    reticula::directed_hyperedge adjacent_edge({6, 7}, {10, 11, 12});
    REQUIRE(reticula::adjacent(large_edge, adjacent_edge));

    reticula::directed_hyperedge non_adjacent({20, 21}, {22, 23});
    REQUIRE_FALSE(reticula::adjacent(large_edge, non_adjacent));
  }

  SECTION("compare correctly") {
    REQUIRE(
      reticula::directed_hyperedge({1}, {2}) ==
      reticula::directed_hyperedge({1}, {2}));
    REQUIRE(
      reticula::directed_hyperedge({1}, {2}) !=
      reticula::directed_hyperedge({2}, {1}));
    REQUIRE(
      reticula::directed_hyperedge({1}, {2}) !=
      reticula::directed_hyperedge({2}, {3}));
  }

  SECTION("format correctly") {
    REQUIRE(
      std::format("{}", reticula::directed_hyperedge({1, 2}, {3, 4})) ==
      "({1, 2} -> {3, 4})");
  }
}
