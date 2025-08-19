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
  "undirected temporal hypernetwork complies with the concepts",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  STATIC_REQUIRE(
    reticula::network_like<reticula::undirected_temporal_hypernetwork>);
  STATIC_REQUIRE(
    reticula::dense_id_network<reticula::undirected_temporal_hypernetwork>);
}

TEST_CASE(
  "constructing network from views",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  std::uint64_t n = 5;
  auto verts = std::ranges::iota_view{0uz, n};
  auto edges = verts | std::views::transform([n](std::uint64_t v) {
                 return reticula::undirected_temporal_hyperedge(
                   {v, (v + 1) % n}, static_cast<double>(v));
               });
  auto graph = reticula::undirected_temporal_hypernetwork(edges, verts);
  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(std::vector<int>({0, 1, 2, 3, 4})));
  REQUIRE_THAT(
    graph.edges(), UnorderedRangeEquals(
                     std::vector<reticula::undirected_temporal_hyperedge>(
                       {{{0, 1}, 0.0},
                        {{1, 2}, 1.0},
                        {{2, 3}, 2.0},
                        {{3, 4}, 3.0},
                        {{4, 0}, 4.0}})));
}

TEST_CASE(
  "undirected temporal hypernetwork edge list properties",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  reticula::undirected_temporal_hypernetwork graph(
    {{{1, 2}, 10.0},
     {{1, 5}, 15.0},
     {{5, 2}, 25.0},
     {{4, 5}, 45.0},
     {{3, 2}, 32.0},
     {{4, 3}, 43.0},
     {{4, 6}, 46.0},
     {{1, 2}, 10.0},
     {{2, 1}, 10.0},
     {{1, 2}, 10.0},
     {{5, 2}, 25.0}},
    {0});

  std::vector<reticula::undirected_temporal_hyperedge> edges(
    {{{1, 2}, 10.0},
     {{1, 5}, 15.0},
     {{2, 5}, 25.0},
     {{3, 2}, 32.0},
     {{3, 4}, 43.0},
     {{4, 5}, 45.0},
     {{4, 6}, 46.0}});
  std::ranges::sort(edges);

  REQUIRE_THAT(graph.edges(), RangeEquals(edges));
  REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));
  REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));
}

TEST_CASE(
  "undirected temporal hypernetwork incidence list properties",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  using vert_vec_t = std::vector<std::uint64_t>;

  reticula::undirected_temporal_hypernetwork graph(
    {{{1, 2}, 10.0},
     {{1, 5}, 15.0},
     {{5, 2}, 25.0},
     {{4, 5}, 45.0},
     {{3, 2}, 32.0},
     {{4, 3}, 43.0},
     {{4, 6}, 46.0},
     {{1, 2}, 10.0},
     {{2, 1}, 10.0},
     {{5, 2}, 25.0}},
    {0});

  REQUIRE_THAT(graph.successors(3), UnorderedRangeEquals(vert_vec_t({2, 4})));
  REQUIRE_THAT(graph.predecessors(3), UnorderedRangeEquals(vert_vec_t({2, 4})));
  REQUIRE_THAT(graph.neighbours(3), UnorderedRangeEquals(vert_vec_t({2, 4})));

  REQUIRE_THAT(
    graph.out_edges(3), UnorderedRangeEquals(
                          std::vector<reticula::undirected_temporal_hyperedge>(
                            {{{3, 4}, 43.0}, {{3, 2}, 32.0}})));
  REQUIRE_THAT(
    graph.in_edges(3), UnorderedRangeEquals(
                         std::vector<reticula::undirected_temporal_hyperedge>(
                           {{{3, 4}, 43.0}, {{3, 2}, 32.0}})));
  REQUIRE_THAT(
    graph.incident_edges(3),
    UnorderedRangeEquals(
      std::vector<reticula::undirected_temporal_hyperedge>(
        {{{3, 4}, 43.0}, {{3, 2}, 32.0}})));

  REQUIRE(graph.out_degree(3) == 2);
  REQUIRE(graph.in_degree(3) == 2);
  REQUIRE(graph.degree(3) == 2);

  std::vector<reticula::undirected_temporal_hyperedge> edges(
    {{{1, 2}, 10.0},
     {{1, 5}, 15.0},
     {{5, 2}, 25.0},
     {{4, 5}, 45.0},
     {{3, 2}, 32.0},
     {{4, 3}, 43.0},
     {{4, 6}, 46.0}});
  vert_vec_t verts({0, 1, 2, 3, 4, 5, 6});

  REQUIRE(graph == graph);
  REQUIRE(graph == reticula::undirected_temporal_hypernetwork(edges, verts));

  REQUIRE(
    graph !=
    reticula::undirected_temporal_hypernetwork(
      std::vector<reticula::undirected_temporal_hyperedge>{{{1, 2}, 10.0}},
      verts));
  REQUIRE(
    graph != reticula::undirected_temporal_hypernetwork(edges, vert_vec_t{15}));

  REQUIRE_THAT(
    graph.vertices(), UnorderedRangeEquals(vert_vec_t({0, 1, 2, 3, 4, 5, 6})));
}

TEST_CASE(
  "undirected temporal hypernetwork with true hyperedges",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  reticula::undirected_temporal_hypernetwork graph(
    {{{1, 2, 3}, 10.0},
     {{2, 3, 4, 5}, 20.0},
     {{4, 5, 6}, 30.0},
     {{1, 6, 7, 8, 9}, 40.0}});

  REQUIRE_THAT(
    graph.vertices(),
    UnorderedRangeEquals(std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9})));

  REQUIRE_THAT(
    graph.neighbours(2),
    UnorderedRangeEquals(std::vector<std::uint64_t>({1, 3, 4, 5})));
  REQUIRE_THAT(
    graph.neighbours(6),
    UnorderedRangeEquals(std::vector<std::uint64_t>({1, 4, 5, 7, 8, 9})));

  REQUIRE(graph.degree(2) == 2);
  REQUIRE(graph.degree(1) == 2);
  REQUIRE(graph.degree(5) == 2);

  REQUIRE_THAT(
    graph.incident_edges(2),
    UnorderedRangeEquals(
      std::vector<reticula::undirected_temporal_hyperedge>(
        {{{1, 2, 3}, 10.0}, {{2, 3, 4, 5}, 20.0}})));
}

TEST_CASE(
  "undirected temporal hypernetwork temporal properties",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  reticula::undirected_temporal_hypernetwork graph(
    {{{1, 2}, 10.0},
     {{2, 3}, 20.0},
     {{3, 5}, 30.0},
     {{5, 6}, 40.0},
     {{5, 4}, 35.0}});

  auto edges = graph.edges();

  // Test that edges are sorted by time (which is the default ordering)
  for (std::size_t i = 1; i < edges.size(); ++i) {
    REQUIRE(edges[i - 1].cause_time() <= edges[i].cause_time());
  }

  // Test static projections
  std::vector<reticula::undirected_hyperedge> static_edges;
  for (const auto& e : edges) {
    static_edges.push_back(e.static_projection());
  }

  REQUIRE_THAT(
    static_edges, UnorderedRangeEquals(
                    std::vector<reticula::undirected_hyperedge>(
                      {{1, 2}, {2, 3}, {3, 5}, {4, 5}, {5, 6}})));
}

TEST_CASE(
  "undirected temporal hypernetwork has_vertex and has_edge methods",
  "[reticula::network][reticula::undirected_temporal_hypernetwork]") {
  reticula::undirected_temporal_hypernetwork graph(
    {{{1, 2}, 10.0}, {{1, 5}, 15.0}, {{2, 5}, 25.0}, {{3, 2}, 32.0}, {{3, 4}, 43.0}, {{4, 5}, 45.0}, {{4, 6}, 46.0}},
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

  REQUIRE(graph.has_edge({{1, 2}, 10.0}));
  REQUIRE(graph.has_edge({{2, 1}, 10.0}));
  REQUIRE(graph.has_edge({{1, 5}, 15.0}));
  REQUIRE(graph.has_edge({{5, 1}, 15.0}));
  REQUIRE(graph.has_edge({{2, 5}, 25.0}));
  REQUIRE(graph.has_edge({{5, 2}, 25.0}));
  REQUIRE(graph.has_edge({{3, 2}, 32.0}));
  REQUIRE(graph.has_edge({{2, 3}, 32.0}));
  REQUIRE(graph.has_edge({{3, 4}, 43.0}));
  REQUIRE(graph.has_edge({{4, 3}, 43.0}));
  REQUIRE(graph.has_edge({{4, 5}, 45.0}));
  REQUIRE(graph.has_edge({{5, 4}, 45.0}));
  REQUIRE(graph.has_edge({{4, 6}, 46.0}));
  REQUIRE(graph.has_edge({{6, 4}, 46.0}));
  REQUIRE_FALSE(graph.has_edge({{1, 3}, 20.0}));
  REQUIRE_FALSE(graph.has_edge({{1, 2}, 15.0}));
  REQUIRE_FALSE(graph.has_edge({{7, 8}, 50.0}));

  reticula::undirected_temporal_hypernetwork hyper_graph(
    {{{1, 2, 3}, 10.0}, {{2, 3, 4, 5}, 20.0}, {{4, 5, 6}, 30.0}, {{1, 6, 7, 8, 9}, 40.0}});

  REQUIRE(hyper_graph.has_edge({{1, 2, 3}, 10.0}));
  REQUIRE(hyper_graph.has_edge({{2, 3, 4, 5}, 20.0}));
  REQUIRE(hyper_graph.has_edge({{4, 5, 6}, 30.0}));
  REQUIRE(hyper_graph.has_edge({{1, 6, 7, 8, 9}, 40.0}));
  REQUIRE_FALSE(hyper_graph.has_edge({{1, 2}, 10.0}));
  REQUIRE_FALSE(hyper_graph.has_edge({{1, 2, 3, 4}, 15.0}));
}
