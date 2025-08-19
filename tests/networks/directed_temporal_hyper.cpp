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
  "directed temporal hypernetwork complies with the concepts",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  STATIC_REQUIRE(reticula::network_like<reticula::directed_temporal_hypernetwork>);
  STATIC_REQUIRE(reticula::dense_id_network<reticula::directed_temporal_hypernetwork>);
}

TEST_CASE(
  "constructing network from views",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  std::uint64_t n = 5;
  auto verts = std::ranges::iota_view{0uz, n};
  auto edges = verts | std::views::transform([n](std::uint64_t v) {
                 return reticula::directed_temporal_hyperedge({v}, {(v + 1) % n}, static_cast<double>(v));
               });
  auto graph = reticula::directed_temporal_hypernetwork(edges, verts);
  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(std::vector<int>({0, 1, 2, 3, 4})));
  REQUIRE_THAT(
    graph.edges(),
    UnorderedRangeEquals(
      std::vector<reticula::directed_temporal_hyperedge>(
        {{{0}, {1}, 0}, {{1}, {2}, 1}, {{2}, {3}, 2}, {{3}, {4}, 3}, {{4}, {0}, 4}})));
}

TEST_CASE(
  "directed temporal hypernetwork edge list properties",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  reticula::directed_temporal_hypernetwork graph(
    {{{1}, {2}, 10},
     {{2}, {3}, 20},
     {{3}, {5}, 30},
     {{5}, {6}, 40},
     {{5}, {4}, 35},
     {{4}, {2}, 25},
     {{1}, {2}, 10},
     {{2}, {3}, 20},
     {{3}, {5}, 30}},
    {0});

  std::vector<reticula::directed_temporal_hyperedge> edges(
    {{{1}, {2}, 10}, {{2}, {3}, 20}, {{4}, {2}, 25}, {{3}, {5}, 30}, {{5}, {4}, 35}, {{5}, {6}, 40}});
  std::ranges::sort(edges);

  REQUIRE_THAT(graph.edges(), RangeEquals(edges));
  REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

  std::ranges::sort(edges, reticula::effect_lt);
  REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));
}

TEST_CASE(
  "directed temporal hypernetwork incidence list properties",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  using vert_vec_t = std::vector<std::uint64_t>;

  reticula::directed_temporal_hypernetwork graph(
    {{{1}, {2}, 10}, {{2}, {3}, 20}, {{3}, {5}, 30}, {{5}, {6}, 40}, {{5}, {4}, 35}, {{4}, {2}, 25}},
    {0});

  REQUIRE_THAT(graph.successors(2), UnorderedRangeEquals(vert_vec_t({3})));
  REQUIRE_THAT(graph.predecessors(2), UnorderedRangeEquals(vert_vec_t({1, 4})));
  REQUIRE_THAT(
    graph.neighbours(2), UnorderedRangeEquals(vert_vec_t({1, 3, 4})));

  REQUIRE_THAT(
    graph.out_edges(2),
    UnorderedRangeEquals(
      std::vector<reticula::directed_temporal_hyperedge>({{{2}, {3}, 20}})));
  REQUIRE_THAT(
    graph.in_edges(2),
    UnorderedRangeEquals(
      std::vector<reticula::directed_temporal_hyperedge>({{{4}, {2}, 25}, {{1}, {2}, 10}})));
  REQUIRE_THAT(
    graph.incident_edges(2), UnorderedRangeEquals(
                               std::vector<reticula::directed_temporal_hyperedge>(
                                 {{{4}, {2}, 25}, {{1}, {2}, 10}, {{2}, {3}, 20}})));

  REQUIRE(graph.out_degree(2) == 1);
  REQUIRE(graph.in_degree(2) == 2);
  REQUIRE(graph.degree(2) == 3);

  std::vector<reticula::directed_temporal_hyperedge> edges(
    {{{5}, {6}, 40}, {{5}, {4}, 35}, {{4}, {2}, 25}, {{1}, {2}, 10}, {{2}, {3}, 20}, {{3}, {5}, 30}});
  vert_vec_t verts({0, 1, 2, 3, 4, 5, 6});

  REQUIRE(graph == graph);
  REQUIRE(graph == reticula::directed_temporal_hypernetwork(edges, verts));

  REQUIRE(
    graph !=
    reticula::directed_temporal_hypernetwork(
      std::vector<reticula::directed_temporal_hyperedge>{{{1}, {2}, 10}, {{2}, {3}, 20}},
      verts));
  REQUIRE(graph != reticula::directed_temporal_hypernetwork(edges, vert_vec_t{15}));

  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(vert_vec_t({0, 1, 2, 3, 4, 5, 6})));
}

TEST_CASE(
  "directed temporal hypernetwork with true hyperedges",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  reticula::directed_temporal_hypernetwork graph(
    {{{1, 2}, {3, 4}, 10},
     {{3, 4, 5}, {6, 7}, 20},
     {{6}, {8, 9, 10}, 30},
     {{8, 9}, {1, 2}, 40}});

  REQUIRE_THAT(
    graph.vertices(),
    UnorderedRangeEquals(std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})));

  REQUIRE_THAT(
    graph.successors(1),
    UnorderedRangeEquals(std::vector<std::uint64_t>({3, 4})));
  REQUIRE_THAT(
    graph.predecessors(1),
    UnorderedRangeEquals(std::vector<std::uint64_t>({8, 9})));

  REQUIRE_THAT(
    graph.successors(6),
    UnorderedRangeEquals(std::vector<std::uint64_t>({8, 9, 10})));
  REQUIRE_THAT(
    graph.predecessors(6),
    UnorderedRangeEquals(std::vector<std::uint64_t>({3, 4, 5})));

  REQUIRE(graph.out_degree(1) == 1);
  REQUIRE(graph.in_degree(1) == 1);
  REQUIRE(graph.degree(1) == 2);

  REQUIRE(graph.out_degree(6) == 1);
  REQUIRE(graph.in_degree(6) == 1);
  REQUIRE(graph.degree(6) == 2);

  REQUIRE_THAT(
    graph.out_edges(1),
    UnorderedRangeEquals(
      std::vector<reticula::directed_temporal_hyperedge>({{{1, 2}, {3, 4}, 10}})));

  REQUIRE_THAT(
    graph.in_edges(6),
    UnorderedRangeEquals(
      std::vector<reticula::directed_temporal_hyperedge>({{{3, 4, 5}, {6, 7}, 20}})));
}

TEST_CASE(
  "directed temporal hypernetwork temporal properties",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  reticula::directed_temporal_hypernetwork graph(
    {{{1}, {2}, 10}, {{2}, {3}, 20}, {{3}, {5}, 30}, {{5}, {6}, 40}, {{5}, {4}, 35}});

  auto edges = graph.edges();
  
  // Test that edges are sorted by cause time (which is the default ordering)
  for (std::size_t i = 1; i < edges.size(); ++i) {
    REQUIRE(edges[i-1].cause_time() <= edges[i].cause_time());
  }

  // Test static projections
  std::vector<reticula::directed_hyperedge> static_edges;
  for (const auto& e : edges) {
    static_edges.push_back(e.static_projection());
  }
  
  REQUIRE_THAT(static_edges, UnorderedRangeEquals(
    std::vector<reticula::directed_hyperedge>({{{1}, {2}}, {{2}, {3}}, {{3}, {5}}, {{5}, {4}}, {{5}, {6}}})));
}

TEST_CASE(
  "directed temporal hypernetwork has_vertex and has_edge methods",
  "[reticula::network][reticula::directed_temporal_hypernetwork]") {
  reticula::directed_temporal_hypernetwork graph(
    {{{1}, {2}, 10}, {{2}, {3}, 20}, {{3}, {5}, 30}, {{5}, {6}, 40}, {{5}, {4}, 35}, {{4}, {2}, 25}},
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

  REQUIRE(graph.has_edge({{1}, {2}, 10}));
  REQUIRE(graph.has_edge({{2}, {3}, 20}));
  REQUIRE(graph.has_edge({{3}, {5}, 30}));
  REQUIRE(graph.has_edge({{5}, {6}, 40}));
  REQUIRE(graph.has_edge({{5}, {4}, 35}));
  REQUIRE(graph.has_edge({{4}, {2}, 25}));
  REQUIRE_FALSE(graph.has_edge({{2}, {1}, 10}));
  REQUIRE_FALSE(graph.has_edge({{1}, {2}, 15}));
  REQUIRE_FALSE(graph.has_edge({{6}, {5}, 40}));
  REQUIRE_FALSE(graph.has_edge({{1}, {3}, 20}));
  REQUIRE_FALSE(graph.has_edge({{7}, {8}, 50}));

  reticula::directed_temporal_hypernetwork hyper_graph(
    {{{1, 2}, {3, 4}, 10}, {{3, 4, 5}, {6, 7}, 20}, {{6}, {8, 9, 10}, 30}, {{8, 9}, {1, 2}, 40}});

  REQUIRE(hyper_graph.has_edge({{1, 2}, {3, 4}, 10}));
  REQUIRE(hyper_graph.has_edge({{3, 4, 5}, {6, 7}, 20}));
  REQUIRE(hyper_graph.has_edge({{6}, {8, 9, 10}, 30}));
  REQUIRE(hyper_graph.has_edge({{8, 9}, {1, 2}, 40}));
  REQUIRE_FALSE(hyper_graph.has_edge({{3, 4}, {1, 2}, 10}));
  REQUIRE_FALSE(hyper_graph.has_edge({{1, 2, 3}, {4, 5}, 15}));
}
