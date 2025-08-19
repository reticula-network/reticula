#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/edges/directed_delayed_temporal_hyperedge.hpp>

#include "../stringify.hpp"

TEST_CASE(
  "directed delayed temporal hyperedges",
  "[reticula::directed_delayed_temporal_hyperedge]") {
  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(
      reticula::temporal_network_edge<
        reticula::directed_delayed_temporal_hyperedge>);
  }

  SECTION("have correct properties") {
    reticula::directed_delayed_temporal_hyperedge edge(
      {1, 2}, {3, 4}, 5.0, 7.0);

    REQUIRE_THAT(
      edge.mutated_verts(),
      RangeEquals(std::vector<reticula::VertexType>({3, 4})));
    REQUIRE_THAT(
      edge.mutator_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2})));
    REQUIRE_THAT(
      edge.incident_verts(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4})));

    REQUIRE_THAT(
      edge.tails(), RangeEquals(std::vector<reticula::VertexType>({1, 2})));
    REQUIRE_THAT(
      edge.heads(), RangeEquals(std::vector<reticula::VertexType>({3, 4})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE(edge.is_out_incident(2));
    REQUIRE_FALSE(edge.is_out_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(4));
    REQUIRE_FALSE(edge.is_out_incident(5));

    REQUIRE(edge.is_in_incident(3));
    REQUIRE(edge.is_in_incident(4));
    REQUIRE_FALSE(edge.is_in_incident(1));
    REQUIRE_FALSE(edge.is_in_incident(2));
    REQUIRE_FALSE(edge.is_in_incident(5));

    REQUIRE(edge.is_incident(1));
    REQUIRE(edge.is_incident(2));
    REQUIRE(edge.is_incident(3));
    REQUIRE(edge.is_incident(4));
    REQUIRE_FALSE(edge.is_incident(5));

    REQUIRE(edge.cause_time() == 5.0);
    REQUIRE(edge.effect_time() == 7.0);

    REQUIRE(
      reticula::adjacent(
        edge, reticula::directed_delayed_temporal_hyperedge(
                {3, 4}, {5, 6}, 8.0, 10.0)));
    REQUIRE(
      reticula::adjacent(
        edge,
        reticula::directed_delayed_temporal_hyperedge({4}, {5, 6}, 8.0, 10.0)));
    REQUIRE_FALSE(
      reticula::adjacent(
        edge, reticula::directed_delayed_temporal_hyperedge(
                {3, 4}, {5, 6}, 6.0, 8.0)));
    REQUIRE(
      reticula::adjacent(
        reticula::directed_delayed_temporal_hyperedge({5, 6}, {1, 2}, 3.0, 4.0),
        edge));
    REQUIRE_FALSE(
      reticula::adjacent(
        edge, reticula::directed_delayed_temporal_hyperedge(
                {3, 4}, {5, 6}, 7.0, 9.0)));
    REQUIRE_FALSE(
      reticula::adjacent(
        reticula::directed_delayed_temporal_hyperedge({3, 4}, {5, 6}, 7.0, 9.0),
        edge));
    REQUIRE_FALSE(
      reticula::adjacent(
        edge, reticula::directed_delayed_temporal_hyperedge(
                {5, 6}, {7, 8}, 8.0, 10.0)));

    REQUIRE_NOTHROW(
      reticula::hash<reticula::directed_delayed_temporal_hyperedge>{}(edge));
    REQUIRE_NOTHROW(
      std::hash<reticula::directed_delayed_temporal_hyperedge>{}(edge));
    REQUIRE_NOTHROW(
      hll::hash<reticula::directed_delayed_temporal_hyperedge>{}(
        edge, std::uint64_t{}));
  }

  SECTION("handle large hyperedges correctly") {
    reticula::directed_delayed_temporal_hyperedge large_edge(
      {1, 2, 3}, {4, 5, 6, 7}, 10.0, 15.0);

    REQUIRE_THAT(
      large_edge.tails(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
    REQUIRE_THAT(
      large_edge.heads(),
      RangeEquals(std::vector<reticula::VertexType>({4, 5, 6, 7})));
    REQUIRE_THAT(
      large_edge.incident_verts(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6, 7})));

    REQUIRE(large_edge.is_out_incident(1));
    REQUIRE(large_edge.is_out_incident(3));
    REQUIRE_FALSE(large_edge.is_out_incident(4));
    REQUIRE_FALSE(large_edge.is_out_incident(8));

    REQUIRE(large_edge.is_in_incident(4));
    REQUIRE(large_edge.is_in_incident(7));
    REQUIRE_FALSE(large_edge.is_in_incident(1));
    REQUIRE_FALSE(large_edge.is_in_incident(8));

    reticula::directed_delayed_temporal_hyperedge other_large(
      {6, 7}, {8, 9, 10}, 16.0, 20.0);
    REQUIRE(reticula::adjacent(large_edge, other_large));

    reticula::directed_delayed_temporal_hyperedge non_adjacent(
      {11, 12}, {13, 14}, 16.0, 20.0);
    REQUIRE_FALSE(reticula::adjacent(large_edge, non_adjacent));

    reticula::directed_delayed_temporal_hyperedge earlier(
      {6, 7}, {8, 9, 10}, 14.0, 18.0);
    REQUIRE_FALSE(reticula::adjacent(large_edge, earlier));
  }

  SECTION("compare correctly") {
    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 3.0, 4.0) ==
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 3.0, 4.0));
    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge({1, 2}, {3}, 4.0, 5.0) ==
      reticula::directed_delayed_temporal_hyperedge({2, 1}, {3}, 4.0, 5.0));
    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 3.0, 4.0) !=
      reticula::directed_delayed_temporal_hyperedge({2}, {3}, 3.0, 4.0));
    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 3.0, 4.0) !=
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 2.0, 4.0));
    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 3.0, 4.0) !=
      reticula::directed_delayed_temporal_hyperedge({1}, {2}, 3.0, 5.0));
  }

  SECTION("format correctly") {
    REQUIRE(
      std::format(
        "{}", reticula::directed_delayed_temporal_hyperedge(
                {1, 2}, {3, 4}, 5.0, 7.0)) ==
      "({1, 2} -> {3, 4} t_cause=5 t_effect=7)");
    REQUIRE(
      std::format(
        "{}", reticula::directed_delayed_temporal_hyperedge(
                {5}, {6, 7}, 8.5, 10.2)) ==
      "({5} -> {6, 7} t_cause=8.5 t_effect=10.2)");
  }

  SECTION("static projection works correctly") {
    reticula::directed_delayed_temporal_hyperedge edge(
      {1, 2}, {3, 4}, 5.0, 7.0);
    auto static_proj = edge.static_projection();

    REQUIRE_THAT(
      static_proj.tails(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2})));
    REQUIRE_THAT(
      static_proj.heads(),
      RangeEquals(std::vector<reticula::VertexType>({3, 4})));
  }

  SECTION("effect_cmp works correctly") {
    reticula::directed_delayed_temporal_hyperedge edge1({1}, {2}, 3.0, 4.0);
    reticula::directed_delayed_temporal_hyperedge edge2({1}, {2}, 3.0, 5.0);
    reticula::directed_delayed_temporal_hyperedge edge3({1}, {2}, 4.0, 4.0);
    reticula::directed_delayed_temporal_hyperedge edge4({1}, {3}, 3.0, 4.0);
    reticula::directed_delayed_temporal_hyperedge edge5({2}, {2}, 3.0, 4.0);

    REQUIRE(reticula::effect_cmp(edge1, edge2));
    REQUIRE_FALSE(reticula::effect_cmp(edge2, edge1));
    REQUIRE(reticula::effect_cmp(edge1, edge3));
    REQUIRE_FALSE(reticula::effect_cmp(edge3, edge1));
    REQUIRE(reticula::effect_cmp(edge1, edge4));
    REQUIRE_FALSE(reticula::effect_cmp(edge4, edge1));
    REQUIRE(reticula::effect_cmp(edge1, edge5));
    REQUIRE_FALSE(reticula::effect_cmp(edge5, edge1));
  }
}
