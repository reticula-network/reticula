#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Approx;
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/algorithms/edge_degree.hpp>
#include <reticula/edges.hpp>
#include <reticula/networks.hpp>

TEST_CASE(
  "edge degree", "[reticula::edge_in_degree][reticula::edge_out_degree]"
                 "[reticula::edge_incident_degree][reticula::edge_degree]") {
  REQUIRE(reticula::edge_in_degree(reticula::directed_edge(0, 0)) == 1);
  REQUIRE(reticula::edge_in_degree(reticula::directed_edge(0, 1)) == 1);
  REQUIRE(
    reticula::edge_in_degree(reticula::directed_hyperedge({0, 1}, {1, 2, 3})) ==
    2);

  REQUIRE(reticula::edge_out_degree(reticula::directed_edge(0, 0)) == 1);
  REQUIRE(reticula::edge_out_degree(reticula::directed_edge(0, 1)) == 1);
  REQUIRE(
    reticula::edge_out_degree(
      reticula::directed_hyperedge({0, 1}, {1, 2, 3})) == 3);

  REQUIRE(reticula::edge_incident_degree(reticula::directed_edge(0, 0)) == 1);
  REQUIRE(reticula::edge_incident_degree(reticula::directed_edge(0, 1)) == 2);
  REQUIRE(
    reticula::edge_incident_degree(
      reticula::directed_hyperedge({0, 1}, {1, 2, 3})) == 4);

  REQUIRE(reticula::edge_degree(reticula::undirected_edge(0, 0)) == 1);
  REQUIRE(reticula::edge_degree(reticula::undirected_edge(0, 1)) == 2);
  REQUIRE(
    reticula::edge_degree(reticula::undirected_hyperedge({0, 1, 2, 3})) == 4);
}

TEST_CASE(
  "edge degree sequences",
  "[reticula::edge_in_degree_sequence][reticula::edge_out_degree_sequence]"
  "[reticula::edge_incident_degree_sequence][reticula::edge_degree_sequence]") {
  reticula::undirected_network graph(
    {{1, 2},
     {1, 5},
     {5, 2},
     {4, 5},
     {3, 2},
     {4, 3},
     {4, 6},
     {1, 2},
     {2, 1},
     {5, 2}},
    {0});

  REQUIRE(
    reticula::edge_in_degree_sequence(graph) ==
    std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
  REQUIRE(
    reticula::edge_out_degree_sequence(graph) ==
    std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
  REQUIRE(
    reticula::edge_incident_degree_sequence(graph) ==
    std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
  REQUIRE(
    reticula::edge_degree_sequence(graph) ==
    std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
  REQUIRE(
    reticula::edge_in_out_degree_pair_sequence(graph) ==
    std::vector<std::pair<std::size_t, std::size_t>>{
      {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}});
}
