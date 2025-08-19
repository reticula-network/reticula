#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Approx;
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/algorithms/properties.hpp>
#include <reticula/networks.hpp>

TEST_CASE("density", "[reticula::density]") {
  SECTION("undirected edge") {
    reticula::undirected_network ug(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    REQUIRE(reticula::density(ug) == Approx(0.25));
  }

  SECTION("directed edge") {
    reticula::directed_network dg(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE(reticula::density(dg) == Approx(0.2));
  }
}

TEST_CASE(
  "time window", "[reticula::time_window]"
                 "[reticula::cause_time_window]"
                 "[reticula::effect_time_window]") {
  reticula::directed_delayed_temporal_network network(
    {{1, 2, 1.0, 5.0},
     {2, 1, 2.0, 3.0},
     {1, 2, 5.0, 5.0},
     {2, 3, 6.0, 7.0},
     {3, 4, 8.0, 9.0}});
  REQUIRE(reticula::cause_time_window(network) == reticula::window{1.0, 8.0});
  REQUIRE(reticula::effect_time_window(network) == reticula::window{3.0, 9.0});
  REQUIRE(reticula::time_window(network) == reticula::window{1.0, 9.0});

  reticula::directed_delayed_temporal_network empty_network;
  REQUIRE_THROWS_AS(
    reticula::cause_time_window(empty_network), std::invalid_argument);
  REQUIRE_THROWS_AS(
    reticula::effect_time_window(empty_network), std::invalid_argument);
  REQUIRE_THROWS_AS(
    reticula::time_window(empty_network), std::invalid_argument);
}

TEST_CASE("link timeline", "[reticula::link_timeline]") {
  reticula::directed_delayed_temporal_network network(
    {{1, 2, 1.0, 5.0},
     {2, 1, 2.0, 3.0},
     {1, 2, 5.0, 5.0},
     {2, 3, 6.0, 7.0},
     {3, 4, 8.0, 9.0},
     {5, 6, 1.0, 3.0}});

  auto link_timeline = reticula::link_timeline(network, {1, 2});
  REQUIRE(
    link_timeline == std::vector<reticula::directed_delayed_temporal_edge>{
                       {1, 2, 1, 5}, {1, 2, 5, 5}});
}
