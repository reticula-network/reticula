#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_temporal_edge.hpp>

#include "../stringify.hpp"

TEST_CASE("undirected edges", "[reticula::undirected_temporal_edge]") {
  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(
      reticula::temporal_network_edge<reticula::undirected_temporal_edge>);
  }

  SECTION("have correct properties") {
    reticula::undirected_temporal_edge edge(1, 2, 3.0);

    REQUIRE_THAT(
      edge.mutated_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2})));
    REQUIRE_THAT(
      edge.mutator_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2})));
    REQUIRE_THAT(
      edge.incident_verts(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE(edge.is_out_incident(2));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(edge.cause_time() == 3.0);
    REQUIRE(edge.effect_time() == 3.0);

    REQUIRE(
      reticula::adjacent(edge, reticula::undirected_temporal_edge(2, 3, 4.0)));
    REQUIRE_FALSE(
      reticula::adjacent(edge, reticula::undirected_temporal_edge(2, 3, 2.0)));
    REQUIRE(
      reticula::adjacent(reticula::undirected_temporal_edge(2, 3, 2.0), edge));
    REQUIRE_FALSE(
      reticula::adjacent(edge, reticula::undirected_temporal_edge(2, 3, 3.0)));
    REQUIRE_FALSE(
      reticula::adjacent(reticula::undirected_temporal_edge(2, 3, 3.0), edge));
    REQUIRE(
      reticula::adjacent(edge, reticula::undirected_temporal_edge(3, 2, 4.0)));
    REQUIRE_FALSE(
      reticula::adjacent(edge, reticula::undirected_temporal_edge(3, 4, 4.0)));
    REQUIRE_FALSE(reticula::adjacent(edge, edge));

    REQUIRE_NOTHROW(reticula::hash<reticula::undirected_temporal_edge>{}(edge));
    REQUIRE_NOTHROW(std::hash<reticula::undirected_temporal_edge>{}(edge));
    REQUIRE_NOTHROW(
      hll::hash<reticula::undirected_temporal_edge>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(
      reticula::undirected_temporal_edge(1, 2, 3.0) ==
      reticula::undirected_temporal_edge(1, 2, 3.0));
    REQUIRE(
      reticula::undirected_temporal_edge(1, 2, 3.0) ==
      reticula::undirected_temporal_edge(2, 1, 3.0));
    REQUIRE(
      reticula::undirected_temporal_edge(1, 2, 3.0) !=
      reticula::undirected_temporal_edge(2, 3, 3.0));
    REQUIRE(
      reticula::undirected_temporal_edge(1, 2, 3.0) !=
      reticula::undirected_temporal_edge(1, 2, 2.0));
  }

  SECTION("format correctly") {
    REQUIRE(
      std::format("{}", reticula::undirected_temporal_edge(1, 2, 3.0)) ==
      "(1 - 2 t=3)");
    REQUIRE(
      std::format("{}", reticula::undirected_temporal_edge(2, 3, 3.2)) ==
      "(2 - 3 t=3.2)");
  }
}
