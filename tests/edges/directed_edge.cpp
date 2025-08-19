#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/edges/directed_edge.hpp>

#include "../stringify.hpp"

TEST_CASE("directed edges", "[reticula::directed_edge]") {
  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(reticula::static_network_edge<reticula::directed_edge>);
  }

  SECTION("have correct properties") {
    reticula::directed_edge edge(1, 2);

    REQUIRE_THAT(
      edge.mutated_verts(),
      RangeEquals(std::vector<reticula::VertexType>({2})));
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(
      edge.mutator_verts(),
      RangeEquals(std::vector<reticula::VertexType>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(
      edge.incident_verts(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(2));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(reticula::adjacent(edge, reticula::directed_edge(2, 3)));
    REQUIRE_FALSE(reticula::adjacent(edge, reticula::directed_edge(3, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge, reticula::directed_edge(3, 4)));

    REQUIRE_NOTHROW(reticula::hash<reticula::directed_edge>{}(edge));
    REQUIRE_NOTHROW(std::hash<reticula::directed_edge>{}(edge));
    REQUIRE_NOTHROW(
      hll::hash<reticula::directed_edge>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::directed_edge(1, 2) == reticula::directed_edge(1, 2));
    REQUIRE(reticula::directed_edge(1, 2) != reticula::directed_edge(2, 1));
    REQUIRE(reticula::directed_edge(1, 2) != reticula::directed_edge(2, 3));
  }

  SECTION("format correctly") {
    REQUIRE(std::format("{}", reticula::directed_edge(1, 2)) == "(1 -> 2)");
    REQUIRE(std::format("{}", reticula::directed_edge(2, 3)) == "(2 -> 3)");
    REQUIRE(std::format("{}", reticula::directed_edge(3, 4)) == "(3 -> 4)");
  }
}
