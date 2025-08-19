#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/networks.hpp>
#include <reticula/operations/projections.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("static projection", "[reticula::static_projection]") {
  reticula::directed_delayed_temporal_network network(
    {{1, 2, 1.0, 5.0},
     {2, 1, 2.0, 3.0},
     {1, 2, 5.0, 5.0},
     {2, 3, 6.0, 7.0},
     {3, 4, 8.0, 9.0},
     {5, 6, 1.0, 3.0}});

  auto static_proj = reticula::static_projection(network);
  REQUIRE_THAT(static_proj.vertices(), RangeEquals(network.vertices()));
  REQUIRE_THAT(
    static_proj.edges(), UnorderedRangeEquals(
                           std::vector<reticula::directed_edge>{
                             {1, 2}, {2, 1}, {2, 3}, {3, 4}, {5, 6}}));
}

TEST_CASE("directed projection", "[reticula::directed_projection]") {
  reticula::undirected_network network({{1, 2}, {2, 3}, {3, 4}, {4, 5}}, {12});

  auto directed_proj = reticula::directed_projection(network);
  REQUIRE_THAT(directed_proj.vertices(), RangeEquals(network.vertices()));
  REQUIRE_THAT(
    directed_proj.edges(),
    UnorderedRangeEquals(
      std::vector<reticula::directed_edge>{
        {1, 2}, {2, 1}, {2, 3}, {3, 2}, {3, 4}, {4, 3}, {4, 5}, {5, 4}}));
}

TEST_CASE("clique expansion", "[reticula::clique_expansion]") {
  SECTION("undirected networks") {
    reticula::undirected_network undirected_net(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}}, {12});

    reticula::undirected_hypernetwork h(
      {{1, 2, 3}, {2, 3, 4}, {3, 4}, {5, 3, 11}}, {12});
    auto clique_exp = reticula::clique_expansion(h);
    REQUIRE_THAT(clique_exp.vertices(), RangeEquals(h.vertices()));
    for (auto v : h.vertices()) {
      REQUIRE_THAT(
        clique_exp.successors(v), UnorderedRangeEquals(h.successors(v)));
      REQUIRE_THAT(
        clique_exp.predecessors(v), UnorderedRangeEquals(h.predecessors(v)));
    }
  }

  SECTION("directed networks") {
    reticula::directed_network directed_net(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}}, {12});

    reticula::directed_hypernetwork dh(
      {{{1, 2, 3}, {2, 4}}, {{2, 6}, {3}}, {{3, 3}, {4, 3}}, {{4}, {5}}}, {12});
    auto clique_exp = reticula::clique_expansion(dh);
    REQUIRE_THAT(clique_exp.vertices(), RangeEquals(dh.vertices()));
    for (auto v : dh.vertices()) {
      REQUIRE_THAT(
        clique_exp.successors(v), UnorderedRangeEquals(dh.successors(v)));
      REQUIRE_THAT(
        clique_exp.predecessors(v), UnorderedRangeEquals(dh.predecessors(v)));
    }
  }
}
