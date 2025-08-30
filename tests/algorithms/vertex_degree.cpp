#include <catch2/catch_test_macros.hpp>

#include <reticula/algorithms/vertex_degree.hpp>

#include <reticula/networks.hpp>

TEST_CASE(
  "degree functions", "[reticula::in_degree][reticula::out_degree]"
                      "[reticula::incident_degree][reticula::degree]") {
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

  REQUIRE(reticula::incident_degree(graph, 3) == 2);
  REQUIRE(reticula::out_degree(graph, 3) == 2);
  REQUIRE(reticula::in_degree(graph, 3) == 2);
  REQUIRE(reticula::degree(graph, 3) == 2);

  REQUIRE(reticula::incident_degree(graph, 2) == 3);
  REQUIRE(reticula::out_degree(graph, 2) == 3);
  REQUIRE(reticula::in_degree(graph, 2) == 3);
  REQUIRE(reticula::degree(graph, 2) == 3);

  REQUIRE(reticula::incident_degree(graph, 1) == 2);
  REQUIRE(reticula::out_degree(graph, 0) == 0);
  REQUIRE(reticula::in_degree(graph, 0) == 0);
  REQUIRE(reticula::degree(graph, 0) == 0);

  REQUIRE(reticula::incident_degree(graph, 0) == 0);
  REQUIRE(reticula::out_degree(graph, 0) == 0);
  REQUIRE(reticula::in_degree(graph, 0) == 0);
  REQUIRE(reticula::degree(graph, 0) == 0);
}

TEST_CASE(
  "degree sequences", "[reticula::in_degree_sequence]"
                      "[reticula::out_degree_sequence]"
                      "[reticula::incident_degree_sequence]"
                      "[reticula::degree_sequence]"
                      "[reticula::in_out_degree_pair_sequence]") {
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
    reticula::in_degree_sequence(graph) ==
    std::vector<reticula::VertexType>{0, 2, 3, 2, 3, 3, 1});
  REQUIRE(
    reticula::out_degree_sequence(graph) ==
    std::vector<reticula::VertexType>{0, 2, 3, 2, 3, 3, 1});
  REQUIRE(
    reticula::incident_degree_sequence(graph) ==
    std::vector<reticula::VertexType>{0, 2, 3, 2, 3, 3, 1});
  REQUIRE(
    reticula::degree_sequence(graph) ==
    std::vector<reticula::VertexType>{0, 2, 3, 2, 3, 3, 1});
  REQUIRE(
    reticula::in_out_degree_pair_sequence(graph) ==
    std::vector<std::pair<reticula::VertexType, reticula::VertexType>>{
      {0, 0}, {2, 2}, {3, 3}, {2, 2}, {3, 3}, {3, 3}, {1, 1}});
}
