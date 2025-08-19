#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_hyperedge.hpp>

#include "../stringify.hpp"

TEST_CASE("undirected hyperedges", "[reticula::undirected_hyperedge]") {
  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(
      reticula::static_network_edge<reticula::undirected_hyperedge>);
  }

  SECTION("have correct properties") {
    reticula::undirected_hyperedge edge({1, 2, 3});

    REQUIRE_THAT(
      edge.mutated_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
    REQUIRE_THAT(
      edge.mutator_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
    REQUIRE_THAT(
      edge.incident_verts(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(4));

    REQUIRE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_in_incident(12));

    REQUIRE_FALSE(edge.is_in_incident(5));
    REQUIRE_FALSE(edge.is_out_incident(5));

    REQUIRE(
      reticula::adjacent(edge, reticula::undirected_hyperedge({3, 4, 5})));
    REQUIRE_FALSE(
      reticula::adjacent(edge, reticula::undirected_hyperedge({12, 45})));

    REQUIRE_NOTHROW(reticula::hash<reticula::undirected_hyperedge>{}(edge));
    REQUIRE_NOTHROW(std::hash<reticula::undirected_hyperedge>{}(edge));
    REQUIRE_NOTHROW(
      hll::hash<reticula::undirected_hyperedge>{}(edge, std::uint64_t{}));
  }

  SECTION("handle large hyperedges correctly") {
    reticula::undirected_hyperedge large_edge({1, 2, 3, 4, 5, 6, 7});

    REQUIRE_THAT(
      large_edge.incident_verts(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6, 7})));

    REQUIRE(large_edge.is_out_incident(1));
    REQUIRE(large_edge.is_out_incident(7));
    REQUIRE_FALSE(large_edge.is_out_incident(8));

    REQUIRE(large_edge.is_in_incident(3));
    REQUIRE(large_edge.is_in_incident(6));
    REQUIRE_FALSE(large_edge.is_in_incident(10));

    reticula::undirected_hyperedge other_large({7, 8, 9, 10});
    REQUIRE(reticula::adjacent(large_edge, other_large));

    reticula::undirected_hyperedge non_adjacent({11, 12, 13});
    REQUIRE_FALSE(reticula::adjacent(large_edge, non_adjacent));
  }

  SECTION("compare correctly") {
    REQUIRE(
      reticula::undirected_hyperedge({1, 2}) ==
      reticula::undirected_hyperedge({1, 2}));
    REQUIRE(
      reticula::undirected_hyperedge({1, 2}) ==
      reticula::undirected_hyperedge({2, 1}));
    REQUIRE(
      reticula::undirected_hyperedge({1, 2}) !=
      reticula::undirected_hyperedge({2, 3}));
  }

  SECTION("format correctly") {
    REQUIRE(
      std::format("{}", reticula::undirected_hyperedge({1, 2, 3, 4})) ==
      "({1, 2, 3, 4})");
  }
}
