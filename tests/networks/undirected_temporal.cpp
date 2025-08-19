#include <ranges>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Matchers::Contains;
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include "../stringify.hpp"

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

TEST_CASE(
  "undirected temporal network complies with the concepts",
  "[reticula::network][reticula::undirected_temporal_network]") {
  STATIC_REQUIRE(reticula::network_like<reticula::undirected_temporal_network>);
  STATIC_REQUIRE(reticula::dense_id_network<reticula::undirected_temporal_network>);
}

TEST_CASE(
  "constructing network from views",
  "[reticula::network][reticula::undirected_temporal_network]") {
  std::uint64_t n = 5;
  auto verts = std::ranges::iota_view{0uz, n};
  auto edges = verts | std::views::transform([n](std::uint64_t v) {
                 return reticula::undirected_temporal_edge(v, (v + 1) % n, static_cast<double>(v));
               });
  auto graph = reticula::undirected_temporal_network(edges, verts);
  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(std::vector<int>({0, 1, 2, 3, 4})));
  REQUIRE_THAT(
    graph.edges(), UnorderedRangeEquals(
                     std::vector<reticula::undirected_temporal_edge>(
                       {{0, 1, 0}, {1, 2, 1}, {2, 3, 2}, {3, 4, 3}, {4, 0, 4}})));
}

TEST_CASE(
  "undirected temporal network edge list properties",
  "[reticula::network][reticula::undirected_temporal_network]") {
  reticula::undirected_temporal_network graph(
    {{1, 2, 10},
     {1, 5, 15},
     {5, 2, 25},
     {4, 5, 45},
     {3, 2, 32},
     {4, 3, 43},
     {4, 6, 46},
     {1, 2, 10},
     {2, 1, 10},
     {1, 2, 10},
     {5, 2, 25}},
    {0});

  std::vector<reticula::undirected_temporal_edge> edges(
    {{1, 2, 10}, {1, 5, 15}, {2, 5, 25}, {3, 2, 32}, {3, 4, 43}, {4, 5, 45}, {4, 6, 46}});
  std::ranges::sort(edges);

  REQUIRE_THAT(graph.edges(), RangeEquals(edges));
  REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));
  REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));
}

TEST_CASE(
  "undirected temporal network incidence list properties",
  "[reticula::network][reticula::undirected_temporal_network]") {
  using vert_vec_t = std::vector<std::uint64_t>;

  reticula::undirected_temporal_network graph(
    {{1, 2, 10},
     {1, 5, 15},
     {5, 2, 25},
     {4, 5, 45},
     {3, 2, 32},
     {4, 3, 43},
     {4, 6, 46},
     {1, 2, 10},
     {2, 1, 10},
     {5, 2, 25}},
    {0});

  REQUIRE_THAT(graph.successors(3), UnorderedRangeEquals(vert_vec_t({2, 4})));
  REQUIRE_THAT(graph.predecessors(3), UnorderedRangeEquals(vert_vec_t({2, 4})));
  REQUIRE_THAT(graph.neighbours(3), UnorderedRangeEquals(vert_vec_t({2, 4})));

  REQUIRE_THAT(
    graph.out_edges(3),
    UnorderedRangeEquals(
      std::vector<reticula::undirected_temporal_edge>({{3, 4, 43}, {3, 2, 32}})));
  REQUIRE_THAT(
    graph.in_edges(3),
    UnorderedRangeEquals(
      std::vector<reticula::undirected_temporal_edge>({{3, 4, 43}, {3, 2, 32}})));
  REQUIRE_THAT(
    graph.incident_edges(3),
    UnorderedRangeEquals(
      std::vector<reticula::undirected_temporal_edge>({{3, 4, 43}, {3, 2, 32}})));

  REQUIRE(graph.out_degree(3) == 2);
  REQUIRE(graph.in_degree(3) == 2);
  REQUIRE(graph.degree(3) == 2);

  std::vector<reticula::undirected_temporal_edge> edges(
    {{1, 2, 10}, {1, 5, 15}, {5, 2, 25}, {4, 5, 45}, {3, 2, 32}, {4, 3, 43}, {4, 6, 46}});
  vert_vec_t verts({0, 1, 2, 3, 4, 5, 6});

  REQUIRE(graph == graph);
  REQUIRE(graph == reticula::undirected_temporal_network(edges, verts));

  REQUIRE(
    graph != reticula::undirected_temporal_network(
               std::vector<reticula::undirected_temporal_edge>{{1, 2, 10}}, verts));
  REQUIRE(graph != reticula::undirected_temporal_network(edges, vert_vec_t{15}));

  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(vert_vec_t({0, 1, 2, 3, 4, 5, 6})));
}

TEST_CASE(
  "undirected temporal network temporal properties",
  "[reticula::network][reticula::undirected_temporal_network]") {
  reticula::undirected_temporal_network graph(
    {{1, 2, 10}, {2, 3, 20}, {3, 5, 30}, {5, 6, 40}, {5, 4, 35}});

  auto edges = graph.edges();
  
  // Test that edges are sorted by time (which is the default ordering)
  for (std::size_t i = 1; i < edges.size(); ++i) {
    REQUIRE(edges[i-1].cause_time() <= edges[i].cause_time());
  }

  // Test static projections
  std::vector<reticula::undirected_edge> static_edges;
  for (const auto& e : edges) {
    static_edges.push_back(e.static_projection());
  }
  
  REQUIRE_THAT(static_edges, UnorderedRangeEquals(
    std::vector<reticula::undirected_edge>({{1, 2}, {2, 3}, {3, 5}, {4, 5}, {5, 6}})));
}

TEST_CASE(
  "undirected temporal network has_vertex and has_edge methods",
  "[reticula::network][reticula::undirected_temporal_network]") {
  reticula::undirected_temporal_network graph(
    {{1, 2, 10}, {1, 5, 15}, {2, 5, 25}, {3, 2, 32}, {3, 4, 43}, {4, 5, 45}, {4, 6, 46}},
    {0});

  REQUIRE(graph.has_vertex(0));
  REQUIRE(graph.has_vertex(1));
  REQUIRE(graph.has_vertex(2));
  REQUIRE(graph.has_vertex(3));
  REQUIRE(graph.has_vertex(4));
  REQUIRE(graph.has_vertex(5));
  REQUIRE(graph.has_vertex(6));
  REQUIRE_FALSE(graph.has_vertex(7));
  REQUIRE_FALSE(graph.has_vertex(10));

  REQUIRE(graph.has_edge({1, 2, 10}));
  REQUIRE(graph.has_edge({2, 1, 10}));
  REQUIRE(graph.has_edge({1, 5, 15}));
  REQUIRE(graph.has_edge({5, 1, 15}));
  REQUIRE(graph.has_edge({2, 5, 25}));
  REQUIRE(graph.has_edge({5, 2, 25}));
  REQUIRE(graph.has_edge({3, 2, 32}));
  REQUIRE(graph.has_edge({2, 3, 32}));
  REQUIRE(graph.has_edge({3, 4, 43}));
  REQUIRE(graph.has_edge({4, 3, 43}));
  REQUIRE(graph.has_edge({4, 5, 45}));
  REQUIRE(graph.has_edge({5, 4, 45}));
  REQUIRE(graph.has_edge({4, 6, 46}));
  REQUIRE(graph.has_edge({6, 4, 46}));
  REQUIRE_FALSE(graph.has_edge({1, 3, 20}));
  REQUIRE_FALSE(graph.has_edge({1, 2, 15}));
  REQUIRE_FALSE(graph.has_edge({7, 8, 50}));
}
