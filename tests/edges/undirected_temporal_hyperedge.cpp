#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_temporal_hyperedge.hpp>

#include "../stringify.hpp"

TEST_CASE(
  "undirected temporal hyperedges",
  "[reticula::undirected_temporal_hyperedge]") {
  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(
      reticula::temporal_network_edge<reticula::undirected_temporal_hyperedge>);
  }

  SECTION("have correct properties") {
    reticula::undirected_temporal_hyperedge edge({1, 2, 3}, 4.0);

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

    REQUIRE(edge.cause_time() == 4.0);
    REQUIRE(edge.effect_time() == 4.0);

    REQUIRE(
      reticula::adjacent(
        edge, reticula::undirected_temporal_hyperedge({3, 4, 5}, 5.0)));
    REQUIRE_FALSE(
      reticula::adjacent(
        edge, reticula::undirected_temporal_hyperedge({3, 4, 5}, 3.0)));
    REQUIRE(
      reticula::adjacent(
        reticula::undirected_temporal_hyperedge({3, 4, 5}, 3.0), edge));
    REQUIRE_FALSE(
      reticula::adjacent(
        edge, reticula::undirected_temporal_hyperedge({3, 4, 5}, 4.0)));
    REQUIRE_FALSE(
      reticula::adjacent(
        reticula::undirected_temporal_hyperedge({3, 4, 5}, 4.0), edge));
    REQUIRE_FALSE(
      reticula::adjacent(
        edge, reticula::undirected_temporal_hyperedge({12, 45}, 5.0)));
    REQUIRE_FALSE(reticula::adjacent(edge, edge));

    REQUIRE_NOTHROW(
      reticula::hash<reticula::undirected_temporal_hyperedge>{}(edge));
    REQUIRE_NOTHROW(std::hash<reticula::undirected_temporal_hyperedge>{}(edge));
    REQUIRE_NOTHROW(
      hll::hash<reticula::undirected_temporal_hyperedge>{}(
        edge, std::uint64_t{}));
  }

  SECTION("handle large hyperedges correctly") {
    reticula::undirected_temporal_hyperedge large_edge(
      {1, 2, 3, 4, 5, 6, 7}, 10.0);

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

    reticula::undirected_temporal_hyperedge other_large({7, 8, 9, 10}, 11.0);
    REQUIRE(reticula::adjacent(large_edge, other_large));

    reticula::undirected_temporal_hyperedge non_adjacent({11, 12, 13}, 11.0);
    REQUIRE_FALSE(reticula::adjacent(large_edge, non_adjacent));

    reticula::undirected_temporal_hyperedge earlier({7, 8, 9, 10}, 9.0);
    REQUIRE_FALSE(reticula::adjacent(large_edge, earlier));
  }

  SECTION("compare correctly") {
    REQUIRE(
      reticula::undirected_temporal_hyperedge({1, 2}, 3.0) ==
      reticula::undirected_temporal_hyperedge({1, 2}, 3.0));
    REQUIRE(
      reticula::undirected_temporal_hyperedge({1, 2}, 3.0) ==
      reticula::undirected_temporal_hyperedge({2, 1}, 3.0));
    REQUIRE(
      reticula::undirected_temporal_hyperedge({1, 2}, 3.0) !=
      reticula::undirected_temporal_hyperedge({2, 3}, 3.0));
    REQUIRE(
      reticula::undirected_temporal_hyperedge({1, 2}, 3.0) !=
      reticula::undirected_temporal_hyperedge({1, 2}, 2.0));
  }

  SECTION("format correctly") {
    REQUIRE(
      std::format(
        "{}", reticula::undirected_temporal_hyperedge({1, 2, 3}, 4.0)) ==
      "({1, 2, 3} t=4)");
    REQUIRE(
      std::format("{}", reticula::undirected_temporal_hyperedge({5, 6}, 7.5)) ==
      "({5, 6} t=7.5)");
  }

  SECTION("static projection works correctly") {
    reticula::undirected_temporal_hyperedge edge({1, 2, 3}, 4.0);
    auto static_proj = edge.static_projection();

    REQUIRE_THAT(
      static_proj.incident_verts(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
  }

  SECTION("effect_cmp works correctly") {
    reticula::undirected_temporal_hyperedge edge1({1, 2}, 3.0);
    reticula::undirected_temporal_hyperedge edge2({1, 2}, 4.0);
    reticula::undirected_temporal_hyperedge edge3({2, 3}, 3.0);

    REQUIRE(reticula::effect_cmp(edge1, edge2));
    REQUIRE_FALSE(reticula::effect_cmp(edge2, edge1));
    REQUIRE(reticula::effect_cmp(edge1, edge3));
    REQUIRE_FALSE(reticula::effect_cmp(edge3, edge1));
  }
}
