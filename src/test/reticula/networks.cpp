#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::Contains;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/static_edges.hpp>
#include <reticula/temporal_edges.hpp>
#include <reticula/generators.hpp>

#include <reticula/networks.hpp>

TEST_CASE("constructing networks from views", "[reticula::network]") {
  int n = 5;
  auto verts = reticula::views::iota(0, n);
  auto edges = verts | reticula::views::transform([n](int v) {
        return reticula::directed_edge(v, (v+1)%n);
      });
  auto graph = reticula::directed_network<int>(edges, verts);
  REQUIRE_THAT(graph.vertices(),
      UnorderedRangeEquals(std::vector<int>({0, 1, 2, 3, 4})));
  REQUIRE_THAT(graph.edges(),
      UnorderedRangeEquals(std::vector<reticula::directed_edge<int>>({
          {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}})));
}

TEST_CASE("undirected networks",
        "[reticula::undirected_network][reticula::network]") {
  SECTION("when given one") {
    reticula::undirected_network<int> graph(
        {{1, 2}, {1, 5}, {5, 2}, {4, 5}, {3, 2}, {4, 3}, {4, 6},
         {1, 2}, {2, 1}, {5, 2}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(3),
          UnorderedRangeEquals(std::vector<int>({2, 4})));
      REQUIRE_THAT(graph.predecessors(3),
          UnorderedRangeEquals(std::vector<int>({2, 4})));
      REQUIRE_THAT(graph.neighbours(3),
          UnorderedRangeEquals(std::vector<int>({2, 4})));

      REQUIRE_THAT(graph.out_edges(3),
          UnorderedRangeEquals(
            std::vector<reticula::undirected_edge<int>>({{3, 4}, {3, 2}})));
      REQUIRE_THAT(graph.in_edges(3),
          UnorderedRangeEquals(
            std::vector<reticula::undirected_edge<int>>({{3, 4}, {3, 2}})));
      REQUIRE_THAT(graph.incident_edges(3),
          UnorderedRangeEquals(
            std::vector<reticula::undirected_edge<int>>({{3, 4}, {3, 2}})));

      REQUIRE(graph.out_degree(3) == 2);
      REQUIRE(graph.in_degree(3) == 2);
      REQUIRE(graph.degree(3) == 2);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::undirected_network<int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::undirected_network<int>(
            std::vector<reticula::undirected_edge<int>>{{1, 2}},
            graph.vertices()));
      REQUIRE(graph != reticula::undirected_network<int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::undirected_edge<int>> edges({
          {1, 2}, {1, 5}, {5, 2}, {4, 5}, {3, 2}, {4, 3}, {4, 6}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::undirected_edge<int>& a,
            const reticula::undirected_edge<int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));

      // test sorted output as well
      REQUIRE_THAT(graph.vertices(),
          RangeEquals(std::vector<int>({0, 1, 2, 3, 4, 5, 6})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::undirected_network<int> other(
          {{1, 2}, {1, 5}, {15, 20}, {5, 2}, {7, 4}}, {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), RangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }

  SECTION("network construction benchmarks") {
    int n = 200;
    auto g = reticula::complete_graph<int>(n);
    auto edges = g.edges();
    std::vector<reticula::undirected_edge<int>> edge_vec(
      edges.begin(), edges.end());

    BENCHMARK("undirected_network construction from edges") {
      return reticula::undirected_network<int>(
        edge_vec);
    };

    BENCHMARK("undirected_network construction from edges and iota") {
      return reticula::undirected_network<int>(
        edge_vec,
        std::ranges::iota_view(0, n));
    };
  }
}

TEST_CASE("undirected hypernetworks",
        "[reticula::undirected_hypernetwork][reticula::network]") {
  SECTION("when given one") {
    using U = reticula::undirected_hyperedge<int>;
    reticula::undirected_hypernetwork<int> graph(
        {U{1, 2}, U{1, 5}, U{5, 2}, U{4, 5}, U{3, 2}, U{4, 3, 7}, U{4, 6},
         U{1, 2}, U{2, 1}, U{5, 2}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(3),
          UnorderedRangeEquals(std::vector<int>({2, 4, 7})));
      REQUIRE_THAT(graph.predecessors(3),
          UnorderedRangeEquals(std::vector<int>({2, 4, 7})));
      REQUIRE_THAT(graph.neighbours(3),
          UnorderedRangeEquals(std::vector<int>({2, 4, 7})));

      REQUIRE_THAT(graph.out_edges(3),
          UnorderedRangeEquals(
            std::vector<reticula::undirected_hyperedge<int>>({
              U{3, 4, 7}, U{3, 2}})));
      REQUIRE_THAT(graph.in_edges(3),
          UnorderedRangeEquals(
            std::vector<reticula::undirected_hyperedge<int>>({
              U{3, 4, 7}, U{3, 2}})));
      REQUIRE_THAT(graph.incident_edges(3),
          UnorderedRangeEquals(
            std::vector<reticula::undirected_hyperedge<int>>({
              U{3, 4, 7}, U{3, 2}})));

      REQUIRE(graph.out_degree(3) == 2);
      REQUIRE(graph.in_degree(3) == 2);
      REQUIRE(graph.degree(3) == 2);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::undirected_hypernetwork<int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::undirected_hypernetwork<int>(
            std::vector<reticula::undirected_hyperedge<int>>{{1, 2}},
            graph.vertices()));
      REQUIRE(graph != reticula::undirected_hypernetwork<int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::undirected_hyperedge<int>> edges({
          U{1, 2}, U{1, 5}, U{5, 2}, U{4, 5}, U{3, 2}, U{4, 3, 7}, U{4, 6}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::undirected_hyperedge<int>& a,
            const reticula::undirected_hyperedge<int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));

      // test sorted output as well
      REQUIRE_THAT(graph.vertices(),
          RangeEquals(std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::undirected_hypernetwork<int> other(
          {{1, 2}, {1, 5}, {15, 20}, {5, 2}, {7, 4}}, {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), RangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("directed networks",
        "[reticula::directed_network][reticula::network]") {
  SECTION("when given one") {
    reticula::directed_network<int> graph(
        {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2},
         {1, 2}, {2, 3}, {3, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1, 4})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3, 4})));

      REQUIRE_THAT(graph.out_edges(2),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<int>>({{2, 3}})));
      REQUIRE_THAT(graph.in_edges(2),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<int>>({{4, 2}, {1, 2}})));
      REQUIRE_THAT(graph.incident_edges(2),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<int>>(
              {{4, 2}, {1, 2}, {2, 3}})));

      REQUIRE(graph.out_degree(2) == 1);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 3);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::directed_network<int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::directed_network<int>(
            std::vector<reticula::directed_edge<int>>{{1, 2}, {2, 3}},
            graph.vertices()));
      REQUIRE(graph != reticula::directed_network<int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::directed_edge<int>> edges(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::directed_edge<int>& a,
            const reticula::directed_edge<int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));


      REQUIRE_THAT(graph.vertices(),
          RangeEquals(std::vector<int>({0, 1, 2, 3, 4, 5, 6})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::directed_network<int> other(
          {{1, 2}, {1, 5}, {15, 20}, {5, 2}, {7, 4}}, {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }

  SECTION("network construction benchmarks") {
    int n = 200;
    auto directed_g = reticula::complete_directed_graph<int>(n);
    auto directed_edges = directed_g.edges();
    std::vector<reticula::directed_edge<int>> directed_edge_vec(
      directed_edges.begin(), directed_edges.end());

    BENCHMARK("directed_network construction from edges") {
      return reticula::directed_network<int>(directed_edge_vec);
    };

    BENCHMARK("directed_network construction from edges and iota") {
      return reticula::directed_network<int>(
        directed_edge_vec,
        std::ranges::iota_view(0, n));
    };
  }
}

TEST_CASE("directed hypernetworks",
        "[reticula::directed_hypernetwork][reticula::network]") {
  SECTION("when given one") {
    reticula::directed_hypernetwork<int> graph(
        {{{1}, {2}}, {{2}, {3, 7}}, {{2}, {3, 7}}, {{3}, {5}}, {{5}, {6}},
        {{5}, {4}}, {{4}, {2}}, {{1}, {2}}, {{2}, {3}}, {{3}, {5}}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({3, 7})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1, 4})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3, 4, 7})));

      REQUIRE_THAT(graph.out_edges(2),
          UnorderedRangeEquals(
            std::vector<reticula::directed_hyperedge<int>>(
              {{{2}, {3, 7}}, {{2}, {3}}})));
      REQUIRE_THAT(graph.in_edges(2),
          UnorderedRangeEquals(
            std::vector<reticula::directed_hyperedge<int>>(
              {{{4}, {2}}, {{1}, {2}}})));
      REQUIRE_THAT(graph.incident_edges(2),
          UnorderedRangeEquals(
            std::vector<reticula::directed_hyperedge<int>>(
              {{{4}, {2}}, {{1}, {2}}, {{2}, {3}}, {{2}, {3, 7}}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::directed_hypernetwork<int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::directed_hypernetwork<int>(
            std::vector<reticula::directed_hyperedge<int>>{
              {{1}, {2}}, {{2}, {3}}},
            graph.vertices()));
      REQUIRE(graph != reticula::directed_hypernetwork<int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::directed_hyperedge<int>> edges(
          {{{1}, {2}}, {{2}, {3, 7}}, {{2}, {3}}, {{3}, {5}}, {{5}, {6}},
          {{5}, {4}}, {{4}, {2}}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::directed_hyperedge<int>& a,
            const reticula::directed_hyperedge<int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));


      REQUIRE_THAT(graph.vertices(),
          RangeEquals(std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::directed_hypernetwork<int> other(
          {{{1}, {2}}, {{1}, {5}}, {{15}, {20}}, {{5}, {2}}, {{7}, {4}}},
          {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("undirected temporal networks",
    "[reticula::undirected_temporal_network][reticula::network]") {
  SECTION("when given one") {
    reticula::undirected_temporal_network<int, int> graph(
        {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
         {2, 1, 2}, {2, 1, 2}, {1, 2, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          RangeEquals(std::vector<reticula::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          RangeEquals(std::vector<reticula::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));
      REQUIRE_THAT(graph.incident_edges(2),
          RangeEquals(std::vector<reticula::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));

      REQUIRE(graph.out_degree(2) == 4);
      REQUIRE(graph.in_degree(2) == 4);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::undirected_temporal_network<int, int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::undirected_temporal_network<int, int>(
            std::vector<reticula::undirected_temporal_edge<int, int>>{
              {1, 2, 1}}, graph.vertices()));
      REQUIRE(graph != reticula::undirected_temporal_network<int, int>(
            graph.edges(), std::vector<int>{15}));

      REQUIRE_THAT(graph.edges(),
          RangeEquals(
            std::vector<reticula::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));

      REQUIRE_THAT(graph.edges_cause(),
          RangeEquals(
            std::vector<reticula::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));

      REQUIRE_THAT(graph.edges_effect(),
          RangeEquals(
            std::vector<reticula::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));

      REQUIRE_THAT(graph.vertices(),
          UnorderedRangeEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::undirected_temporal_network<int, int> other(
          {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1}, {7, 4, 6}, {20, 10, 1}},
          {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("undirected temporal hypernetworks",
    "[reticula::undirected_temporal_hypernetwork][reticula::network]") {
  SECTION("when given one") {
    reticula::undirected_temporal_hypernetwork<int, int> graph(
        {{{2, 3}, 6}, {{2, 3}, 6}, {{3, 4}, 8}, {{1, 2}, 1},
         {{2, 1}, 2}, {{2, 1}, 2}, {{1, 2}, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          RangeEquals(std::vector<
                        reticula::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          RangeEquals(std::vector<
                        reticula::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6}})));
      REQUIRE_THAT(graph.incident_edges(2),
          RangeEquals(std::vector<
                        reticula::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6}})));

      REQUIRE(graph.out_degree(2) == 4);
      REQUIRE(graph.in_degree(2) == 4);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::undirected_temporal_hypernetwork<int, int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::undirected_temporal_hypernetwork<int, int>(
            std::vector<reticula::undirected_temporal_hyperedge<int, int>>{
              {{1, 2}, 1}}, graph.vertices()));
      REQUIRE(graph != reticula::undirected_temporal_hypernetwork<int, int>(
            graph.edges(), std::vector<int>{15}));

      REQUIRE_THAT(graph.edges(),
          RangeEquals(
            std::vector<reticula::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6},
              {{3, 4}, 8}})));

      REQUIRE_THAT(graph.edges_cause(),
          RangeEquals(
            std::vector<reticula::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6},
              {{3, 4}, 8}})));

      REQUIRE_THAT(graph.edges_effect(),
          RangeEquals(
            std::vector<reticula::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6},
              {{3, 4}, 8}})));

      REQUIRE_THAT(graph.vertices(),
          UnorderedRangeEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::undirected_temporal_hypernetwork<int, int> other(
          {{{2, 3}, 6}, {{2, 3}, 6}, {{3, 4}, 8}, {{1, 2}, 1}, {{7, 4}, 6},
          {{20, 10}, 1}}, {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("directed temporal networks",
    "[reticula::directed_temporal_network][reticula::network]") {
  SECTION("when given one") {
    reticula::directed_temporal_network<int, int> graph(
        {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
         {2, 1, 2}, {2, 1, 2}, {1, 2, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          RangeEquals(std::vector<reticula::directed_temporal_edge<int, int>>(
              {{2, 1, 2}, {2, 3, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          RangeEquals(std::vector<reticula::directed_temporal_edge<int, int>>(
              {{1, 2, 1}, {1, 2, 5}})));
      REQUIRE_THAT(graph.incident_edges(2),
          RangeEquals(std::vector<reticula::directed_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::directed_temporal_network<int, int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::directed_temporal_network<int, int>(
            std::vector<reticula::directed_temporal_edge<int, int>>{
              {{1, 2}, 1}}, graph.vertices()));
      REQUIRE(graph != reticula::directed_temporal_network<int, int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::directed_temporal_edge<int, int>> edges(
          {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::directed_temporal_edge<int, int>& a,
            const reticula::directed_temporal_edge<int, int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedRangeEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::directed_temporal_network<int, int> other(
          {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1}, {7, 4, 6}, {20, 10, 1}},
          {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("directed temporal hypernetworks",
    "[reticula::directed_temporal_hypernetwork][reticula::network]") {
  SECTION("when given one") {
    reticula::directed_temporal_hypernetwork<int, int> graph(
        {{{2}, {3}, 6}, {{2}, {3}, 6}, {{3}, {4}, 8}, {{1}, {2}, 1},
         {{2}, {1}, 2}, {{2}, {1}, 2}, {{1}, {2}, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          RangeEquals(std::vector<
                        reticula::directed_temporal_hyperedge<int, int>>(
                      {{{2}, {1}, 2}, {{2}, {3}, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          RangeEquals(std::vector<
                        reticula::directed_temporal_hyperedge<int, int>>(
                      {{{1}, {2}, 1}, {{1}, {2}, 5}})));
      REQUIRE_THAT(graph.incident_edges(2),
          RangeEquals(std::vector<
                        reticula::directed_temporal_hyperedge<int, int>>(
                      {{{1}, {2}, 1},
                      {{2}, {1}, 2},
                      {{1}, {2}, 5},
                      {{2}, {3}, 6}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::directed_temporal_hypernetwork<int, int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::directed_temporal_hypernetwork<int, int>(
            std::vector<reticula::directed_temporal_hyperedge<int, int>>{
              {{1}, {2}, 1}}, graph.vertices()));
      REQUIRE(graph != reticula::directed_temporal_hypernetwork<int, int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::directed_temporal_hyperedge<int, int>> edges(
          {{{1}, {2}, 1}, {{2}, {1}, 2}, {{1}, {2}, 5}, {{2}, {3}, 6},
          {{3}, {4}, 8}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::directed_temporal_hyperedge<int, int>& a,
            const reticula::directed_temporal_hyperedge<int, int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedRangeEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::directed_temporal_hypernetwork<int, int> other(
          {{{2}, {3}, 6}, {{2}, {3}, 6}, {{3}, {4}, 8}, {{1}, {2}, 1},
          {{7}, {4}, 6}, {{20}, {10}, 1}},
          {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}


TEST_CASE("directed delayed temporal networks",
    "[reticula::directed_delayed_temporal_network][reticula::network]") {
  SECTION("when given one") {
    reticula::directed_delayed_temporal_network<int, int> graph(
        {{2, 3, 6, 7}, {3, 4, 8, 9}, {1, 2, 1, 9}, {2, 1, 2, 3}, {1, 2, 5, 8}},
        {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          RangeEquals(std::vector<
            reticula::directed_delayed_temporal_edge<int, int>>(
              {{2, 1, 2, 3}, {2, 3, 6, 7}})));
      REQUIRE_THAT(graph.in_edges(2),
          RangeEquals(std::vector<
            reticula::directed_delayed_temporal_edge<int, int>>(
              {{1, 2, 5, 8}, {1, 2, 1, 9}})));
      REQUIRE_THAT(graph.incident_edges(2),
          RangeEquals(std::vector<
            reticula::directed_delayed_temporal_edge<int, int>>(
              {{1, 2, 1, 9}, {2, 1, 2, 3}, {1, 2, 5, 8}, {2, 3, 6, 7}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph == reticula::directed_delayed_temporal_network<int, int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph != reticula::directed_delayed_temporal_network<int, int>(
            std::vector<reticula::directed_delayed_temporal_edge<int, int>>{
              {1, 2, 1, 9}}, graph.vertices()));
      REQUIRE(graph != reticula::directed_delayed_temporal_network<int, int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<reticula::directed_delayed_temporal_edge<int, int>> edges(
          {{1, 2, 1, 9}, {2, 1, 2, 3}, {1, 2, 5, 8}, {2, 3, 6, 7},
               {3, 4, 8, 9}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::directed_delayed_temporal_edge<int, int>& a,
            const reticula::directed_delayed_temporal_edge<int, int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedRangeEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::directed_delayed_temporal_network<int, int> other(
          {{2, 3, 6, 7}, {3, 4, 8, 9}, {1, 2, 1, 9}, {2, 1, 2, 3},
          {20, 10, 5, 8}},
          {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e,
                                  [](auto e1, auto e2) {
                                    return effect_lt(e1, e2);
                                  }))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e,
                                  [](auto e1, auto e2) {
                                    return effect_lt(e1, e2);
                                  }))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("directed delayed temporal hypernetworks",
    "[reticula::directed_delayed_temporal_hypernetwork][reticula::network]") {
  SECTION("when given one") {
    reticula::directed_delayed_temporal_hypernetwork<int, int> graph(
        {{{2}, {3}, 6, 7}, {{3}, {4}, 8, 9}, {{1}, {2}, 1, 9}, {{2}, {1}, 2, 3},
        {{1}, {2}, 5, 8}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedRangeEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedRangeEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          RangeEquals(std::vector<
            reticula::directed_delayed_temporal_hyperedge<int, int>>(
              {{{2}, {1}, 2, 3}, {{2}, {3}, 6, 7}})));
      REQUIRE_THAT(graph.in_edges(2),
          RangeEquals(std::vector<
            reticula::directed_delayed_temporal_hyperedge<int, int>>(
              {{{1}, {2}, 5, 8}, {{1}, {2}, 1, 9}})));
      REQUIRE_THAT(graph.incident_edges(2),
          RangeEquals(std::vector<
            reticula::directed_delayed_temporal_hyperedge<int, int>>(
              {{{1}, {2}, 1, 9}, {{2}, {1}, 2, 3},
              {{1}, {2}, 5, 8}, {{2}, {3}, 6, 7}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE(graph == graph);
      REQUIRE(graph ==
          reticula::directed_delayed_temporal_hypernetwork<int, int>(
            graph.edges(), graph.vertices()));
      REQUIRE(graph !=
          reticula::directed_delayed_temporal_hypernetwork<int, int>(
            std::vector<
              reticula::directed_delayed_temporal_hyperedge<int, int>>{
                {{1}, {2}, 1, 9}}, graph.vertices()));
      REQUIRE(graph !=
          reticula::directed_delayed_temporal_hypernetwork<int, int>(
            graph.edges(), std::vector<int>{15}));

      std::vector<
        reticula::directed_delayed_temporal_hyperedge<int, int>> edges(
          {{{1}, {2}, 1, 9}, {{2}, {1}, 2, 3}, {{1}, {2}, 5, 8},
          {{2}, {3}, 6, 7}, {{3}, {4}, 8, 9}});
      std::ranges::sort(edges);

      REQUIRE_THAT(graph.edges(), RangeEquals(edges));
      REQUIRE_THAT(graph.edges_cause(), RangeEquals(edges));

      std::ranges::sort(edges,
          [](const reticula::directed_delayed_temporal_hyperedge<int, int>& a,
            const reticula::directed_delayed_temporal_hyperedge<int, int>& b) {
            return reticula::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), RangeEquals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedRangeEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }

    SECTION("union operation is correct") {
      REQUIRE_THAT(graph.union_with(graph).edges_cause(),
                   RangeEquals(graph.edges_cause()));
      REQUIRE_THAT(graph.union_with(graph).vertices(),
                   RangeEquals(graph.vertices()));

      reticula::directed_delayed_temporal_hypernetwork<int, int> other(
          {{{2}, {3}, 6, 7}, {{3}, {4}, 8, 9}, {{1}, {2}, 1, 9},
          {{2}, {1}, 2, 3}, {{20}, {10}, 5, 8}},
          {10, 12});
      auto u = graph.union_with(other);

      REQUIRE_THAT(u.edges_cause(), UnorderedRangeEquals(u.edges_effect()));

      for (const auto& vertex : graph.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : graph.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : graph.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e,
                                  [](auto e1, auto e2) {
                                    return effect_lt(e1, e2);
                                  }))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(graph.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));

      for (const auto& vertex : other.vertices())
        REQUIRE_THAT(u.vertices(), Contains(vertex));
      for (const auto& edge : other.edges_cause())
        REQUIRE_THAT(u.edges_cause(), Contains(edge));
      for (const auto& edge : other.edges_effect())
        REQUIRE_THAT(u.edges_effect(), Contains(edge));

      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutated_verts())
                          if (!std::ranges::binary_search(
                                u.in_edges(v), e,
                                  [](auto e1, auto e2) {
                                    return effect_lt(e1, e2);
                                  }))
                              return false;
                      return true;
                  }));
      REQUIRE(std::ranges::all_of(other.edges(),
                  [&u](const auto& e) {
                      for (auto& v: e.mutator_verts())
                          if (!std::ranges::binary_search(
                                u.out_edges(v), e))
                              return false;
                      return true;
                  }));
    }
  }
}

TEST_CASE("higher-order networks", "[reticula::network]") {
  using HyperEventGraph =
    reticula::network<
      reticula::directed_edge<
        reticula::directed_temporal_hyperedge<int, float>>>;
  REQUIRE_NOTHROW(HyperEventGraph({}, {}));
}
