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
  "directed delayed temporal hypernetwork complies with the concepts",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  STATIC_REQUIRE(reticula::network_like<reticula::directed_delayed_temporal_hypernetwork>);
  STATIC_REQUIRE(reticula::dense_id_network<reticula::directed_delayed_temporal_hypernetwork>);
}

TEST_CASE(
  "constructing network from views",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  std::uint64_t n = 5;
  auto verts = std::ranges::iota_view{0uz, n};
  auto edges = verts | std::views::transform([n](std::uint64_t v) {
                 return reticula::directed_delayed_temporal_hyperedge({v}, {(v + 1) % n}, static_cast<double>(v), static_cast<double>(v + 10));
               });
  auto graph = reticula::directed_delayed_temporal_hypernetwork(edges, verts);
  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(std::vector<int>({0, 1, 2, 3, 4})));
  REQUIRE_THAT(
    graph.edges(),
    UnorderedRangeEquals(
      std::vector<reticula::directed_delayed_temporal_hyperedge>(
        {{{0}, {1}, 0, 10}, {{1}, {2}, 1, 11}, {{2}, {3}, 2, 12}, {{3}, {4}, 3, 13}, {{4}, {0}, 4, 14}})));
}

TEST_CASE(
  "directed delayed temporal hypernetwork edge list properties",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  reticula::directed_delayed_temporal_hypernetwork graph(
    {{{1}, {2}, 10, 15},
     {{2}, {3}, 20, 25},
     {{3}, {5}, 30, 35},
     {{5}, {6}, 40, 45},
     {{5}, {4}, 35, 40},
     {{4}, {2}, 25, 30},
     {{1}, {2}, 10, 15},
     {{2}, {3}, 20, 25},
     {{3}, {5}, 30, 35}},
    {0});

  std::vector<reticula::directed_delayed_temporal_hyperedge> edges(
    {{{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}, {{4}, {2}, 25, 30}, {{3}, {5}, 30, 35}, {{5}, {4}, 35, 40}, {{5}, {6}, 40, 45}});
  std::ranges::sort(edges);

  REQUIRE_THAT(graph.edges(), RangeEquals(edges));
  REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

  std::ranges::sort(edges, reticula::effect_lt);
  REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));
}

TEST_CASE(
  "directed delayed temporal hypernetwork incidence list properties",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  using vert_vec_t = std::vector<std::uint64_t>;

  reticula::directed_delayed_temporal_hypernetwork graph(
    {{{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}, {{3}, {5}, 30, 35}, {{5}, {6}, 40, 45}, {{5}, {4}, 35, 40}, {{4}, {2}, 25, 30}},
    {0});

  REQUIRE_THAT(graph.successors(2), UnorderedRangeEquals(vert_vec_t({3})));
  REQUIRE_THAT(graph.predecessors(2), UnorderedRangeEquals(vert_vec_t({1, 4})));
  REQUIRE_THAT(
    graph.neighbours(2), UnorderedRangeEquals(vert_vec_t({1, 3, 4})));

  REQUIRE_THAT(
    graph.out_edges(2),
    UnorderedRangeEquals(
      std::vector<reticula::directed_delayed_temporal_hyperedge>({{{2}, {3}, 20, 25}})));
  REQUIRE_THAT(
    graph.in_edges(2),
    UnorderedRangeEquals(
      std::vector<reticula::directed_delayed_temporal_hyperedge>({{{4}, {2}, 25, 30}, {{1}, {2}, 10, 15}})));
  REQUIRE_THAT(
    graph.incident_edges(2), UnorderedRangeEquals(
                               std::vector<reticula::directed_delayed_temporal_hyperedge>(
                                 {{{4}, {2}, 25, 30}, {{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}})));

  REQUIRE(graph.out_degree(2) == 1);
  REQUIRE(graph.in_degree(2) == 2);
  REQUIRE(graph.degree(2) == 3);

  std::vector<reticula::directed_delayed_temporal_hyperedge> edges(
    {{{5}, {6}, 40, 45}, {{5}, {4}, 35, 40}, {{4}, {2}, 25, 30}, {{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}, {{3}, {5}, 30, 35}});
  vert_vec_t verts({0, 1, 2, 3, 4, 5, 6});

  REQUIRE(graph == graph);
  REQUIRE(graph == reticula::directed_delayed_temporal_hypernetwork(edges, verts));

  REQUIRE(
    graph !=
    reticula::directed_delayed_temporal_hypernetwork(
      std::vector<reticula::directed_delayed_temporal_hyperedge>{{{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}},
      verts));
  REQUIRE(graph != reticula::directed_delayed_temporal_hypernetwork(edges, vert_vec_t{15}));

  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(vert_vec_t({0, 1, 2, 3, 4, 5, 6})));
}

TEST_CASE(
  "directed delayed temporal hypernetwork with true hyperedges",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  reticula::directed_delayed_temporal_hypernetwork graph(
    {{{1, 2}, {3, 4}, 10, 15},
     {{3, 4, 5}, {6, 7}, 20, 25},
     {{6}, {8, 9, 10}, 30, 35},
     {{8, 9}, {1, 2}, 40, 45}});

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
      std::vector<reticula::directed_delayed_temporal_hyperedge>({{{1, 2}, {3, 4}, 10, 15}})));

  REQUIRE_THAT(
    graph.in_edges(6),
    UnorderedRangeEquals(
      std::vector<reticula::directed_delayed_temporal_hyperedge>({{{3, 4, 5}, {6, 7}, 20, 25}})));
}

TEST_CASE(
  "directed delayed temporal hypernetwork temporal properties",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  reticula::directed_delayed_temporal_hypernetwork graph(
    {{{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}, {{3}, {5}, 30, 35}, {{5}, {6}, 40, 45}, {{5}, {4}, 35, 40}});

  auto edges_cause = graph.edges_cause();
  auto edges_effect = graph.edges_effect();
  
  // Test that edges_cause are sorted by cause time
  for (std::size_t i = 1; i < edges_cause.size(); ++i) {
    REQUIRE(edges_cause[i-1].cause_time() <= edges_cause[i].cause_time());
  }

  // Test that edges_effect are sorted by effect time
  for (std::size_t i = 1; i < edges_effect.size(); ++i) {
    REQUIRE(edges_effect[i-1].effect_time() <= edges_effect[i].effect_time());
  }

  // Test that cause_time != effect_time for delayed edges
  for (const auto& e : edges_cause) {
    REQUIRE(e.cause_time() < e.effect_time());
  }

  // Test static projections
  std::vector<reticula::directed_hyperedge> static_edges;
  for (const auto& e : edges_cause) {
    static_edges.push_back(e.static_projection());
  }
  
  REQUIRE_THAT(static_edges, UnorderedRangeEquals(
    std::vector<reticula::directed_hyperedge>({{{1}, {2}}, {{2}, {3}}, {{3}, {5}}, {{5}, {4}}, {{5}, {6}}})));
}

TEST_CASE(
  "directed delayed temporal hypernetwork has_vertex and has_edge methods",
  "[reticula::network][reticula::directed_delayed_temporal_hypernetwork]") {
  reticula::directed_delayed_temporal_hypernetwork graph(
    {{{1}, {2}, 10, 15}, {{2}, {3}, 20, 25}, {{3}, {5}, 30, 35}, {{5}, {6}, 40, 45}, {{5}, {4}, 35, 40}, {{4}, {2}, 25, 30}},
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

  REQUIRE(graph.has_edge({{1}, {2}, 10, 15}));
  REQUIRE(graph.has_edge({{2}, {3}, 20, 25}));
  REQUIRE(graph.has_edge({{3}, {5}, 30, 35}));
  REQUIRE(graph.has_edge({{5}, {6}, 40, 45}));
  REQUIRE(graph.has_edge({{5}, {4}, 35, 40}));
  REQUIRE(graph.has_edge({{4}, {2}, 25, 30}));
  REQUIRE_FALSE(graph.has_edge({{2}, {1}, 10, 15}));
  REQUIRE_FALSE(graph.has_edge({{1}, {2}, 15, 20}));
  REQUIRE_FALSE(graph.has_edge({{1}, {2}, 10, 20}));
  REQUIRE_FALSE(graph.has_edge({{6}, {5}, 40, 45}));
  REQUIRE_FALSE(graph.has_edge({{1}, {3}, 20, 25}));
  REQUIRE_FALSE(graph.has_edge({{7}, {8}, 50, 55}));

  reticula::directed_delayed_temporal_hypernetwork hyper_graph(
    {{{1, 2}, {3, 4}, 10, 15}, {{3, 4, 5}, {6, 7}, 20, 25}, {{6}, {8, 9, 10}, 30, 35}, {{8, 9}, {1, 2}, 40, 45}});

  REQUIRE(hyper_graph.has_edge({{1, 2}, {3, 4}, 10, 15}));
  REQUIRE(hyper_graph.has_edge({{3, 4, 5}, {6, 7}, 20, 25}));
  REQUIRE(hyper_graph.has_edge({{6}, {8, 9, 10}, 30, 35}));
  REQUIRE(hyper_graph.has_edge({{8, 9}, {1, 2}, 40, 45}));
  REQUIRE_FALSE(hyper_graph.has_edge({{3, 4}, {1, 2}, 10, 15}));
  REQUIRE_FALSE(hyper_graph.has_edge({{1, 2, 3}, {4, 5}, 15, 20}));
}
