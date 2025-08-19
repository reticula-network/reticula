#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/algorithms/static_reachability.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

TEST_CASE(
  "largest weakly connected component",
  "[reticula::largest_weakly_connected_component]") {
  SECTION("works for non-empty directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    reticula::component weak({1, 2, 3, 4, 5, 6});
    reticula::component comp =
      reticula::largest_weakly_connected_component(graph);
    REQUIRE(comp == weak);
  }

  SECTION("works for non-empty directed graph") {
    reticula::directed_network graph;
    reticula::component comp =
      reticula::largest_weakly_connected_component(graph);
    REQUIRE(comp.empty());
  }
}

TEST_CASE(
  "weakly connected components", "[reticula::weakly_connected_components]") {
  SECTION("works for directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5, 6}),
      weak2({7, 8, 9});

    auto comps = reticula::weakly_connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[0].begin(), comps[0].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[1].begin(), comps[1].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }

  SECTION("works for directed hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{1, 2}, {3}}, {{3}, {4, 5}}, {{6}, {7, 8}}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8});

    auto comps = reticula::weakly_connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[0].begin(), comps[0].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[1].begin(), comps[1].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }
}

TEST_CASE("is weakly connected?", "[reticula::is_weakly_connected]") {
  reticula::directed_network graph(
    {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});
  REQUIRE_FALSE(reticula::is_weakly_connected(graph));
  reticula::directed_network connected(
    {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(reticula::is_weakly_connected(connected));

  reticula::directed_network empty;
  REQUIRE(reticula::is_weakly_connected(empty));
}

TEST_CASE(
  "weakly connected component", "[reticula::weakly_connected_component]") {
  SECTION("works for directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5, 6}),
      weak2({7, 8, 9});

    for (auto i : weak1) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak1));
    }

    for (auto i : weak2) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak2));
    }
  }

  SECTION("works for directed hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{1, 2}, {3}}, {{3}, {4, 5}}, {{6}, {7, 8}}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8});

    for (auto i : weak1) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak1));
    }

    for (auto i : weak2) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak2));
    }
  }
}

TEST_CASE("out-component", "[reticula::out_component]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));

    auto c6 = reticula::out_component(graph, 6);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c6.begin(), c6.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({6})));
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}}, {{4}, {2, 3}}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));

    auto c6 = reticula::out_component(graph, 6);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c6.begin(), c6.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({6})));

    auto c7 = reticula::out_component(graph, 7);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c7.begin(), c7.end()),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6, 7})));
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network graph({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({5, 4, 6})));
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork graph(
      {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({5, 4, 6})));

    auto c7 = reticula::out_component(graph, 7);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c7.begin(), c7.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({7, 3, 4, 5, 6})));
  }
}

TEST_CASE("in-component", "[reticula::in_component]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));

    auto c1 = reticula::in_component(graph, 1);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c1.begin(), c1.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1})));
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}}, {{4}, {2, 3}}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 7})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 7})));

    auto c1 = reticula::in_component(graph, 7);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c1.begin(), c1.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({7})));
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network graph({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 1})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 5})));
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork graph(
      {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c2.begin(), c2.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 1})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(c5.begin(), c5.end()),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 5, 7})));
  }
}

TEST_CASE("out-components", "[reticula::out_components]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    for (auto& [v, c] : reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({3, 4, 5, 6, 2})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({4, 2, 3, 5, 6})));
      else if (v == 5)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({4, 2, 3, 5, 6})));
      else if (v == 6)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({6})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 6}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 1}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on another cyclic graph") {
    reticula::directed_network graph({{1, 2}, {2, 3}, {3, 2}}, {4, 1, 2, 3});

    for (auto& [v, c] : reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({4})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 3}, {2, 2}, {3, 2}, {4, 1}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on another cyclic graph with self-loops") {
    reticula::directed_network graph({{1, 1}, {2, 3}}, {1, 2, 3, 4});

    for (auto& [v, c] : reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 3})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({3})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({4})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 1}, {2, 2}, {3, 1}, {4, 1}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}}, {{4}, {2, 3}}});

    for (auto& [v, c] : reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 3, 4, 5, 6, 2})));
      else if (v == 7)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 4, 2, 3, 5, 6, 7})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 1}, {7, 7}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network graph({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    for (auto& [v, c] : reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({3, 4, 5, 6})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({4})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 6}, {2, 5}, {3, 4}, {4, 1}, {5, 3}, {6, 1}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork graph(
      {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    for (auto& [v, c] : reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({3, 4, 5, 6})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({4})));
      else if (v == 7)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({7, 3, 4, 5, 6})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 6}, {2, 5}, {3, 4}, {4, 1}, {5, 3}, {6, 1}, {7, 5}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }
}

TEST_CASE("in-components", "[reticula::in_components]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    for (auto& [v, c] : reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));
      else if (v == 5)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));
      else if (v == 6)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 6})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 1}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 6}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}}, {{4}, {2, 3}}});

    for (auto& [v, c] : reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 7})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 7})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 7})));
      else if (v == 7)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({7})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 7}, {7, 1}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network graph({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    for (auto& [v, c] : reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2, 3})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 1}, {2, 2}, {3, 3}, {4, 5}, {5, 4}, {6, 5}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork graph(
      {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    for (auto& [v, c] : reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1})));
      else if (v == 2)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({1, 2})));
      else if (v == 3)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 7})));
      else if (v == 4)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 4, 5, 7})));
      else if (v == 6)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(
            std::vector<reticula::VertexType>({1, 2, 3, 5, 6, 7})));
      else if (v == 7)
        REQUIRE_THAT(
          std::vector<reticula::VertexType>(c.begin(), c.end()),
          UnorderedRangeEquals(std::vector<reticula::VertexType>({7})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<reticula::VertexType, std::size_t> comp_map;
    for (auto [v, c] : comp_sizes)
      comp_map[v] = c;

    std::unordered_map<reticula::VertexType, std::size_t> res(
      {{1, 1}, {2, 2}, {3, 4}, {4, 6}, {5, 5}, {6, 6}, {7, 1}});

    REQUIRE(comp_map == res);

    // auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    // REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }
}

TEST_CASE(
  "strongly_connected_components",
  "[reticula::strongly_connected_components]") {
  SECTION("works for directed graph") {
    reticula::directed_network graph(
      {{1, 2},
       {2, 3},
       {3, 1},
       {3, 5},
       {5, 6},
       {5, 4},
       {4, 2},
       {7, 8},
       {8, 9},
       {9, 1}});

    std::vector<reticula::component> res{{6}, {1, 2, 3, 4, 5}, {9}, {8}, {7}};

    auto comps = reticula::strongly_connected_components(graph);
    REQUIRE(comps == res);
  }

  SECTION("works for directed hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{1, 2}, {3}},
       {{3}, {4, 5}},
       {{4, 1, 5}, {1, 2, 3}},
       {{1}, {6}},
       {{6}, {7, 8}},
       {{8, 9}, {10}}});
    std::vector<reticula::component> res{{10}, {8}, {7}, {6}, {1, 2, 3, 4, 5},
                                         {9}};

    auto comps = reticula::strongly_connected_components(graph);
    REQUIRE(comps == res);
  }
}

TEST_CASE(
  "largest connected component", "[reticula::largest_connected_component]") {
  SECTION("works for non-empty undirected graph") {
    reticula::undirected_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    reticula::component lcc({1, 2, 3, 4, 5, 6});
    reticula::component comp = reticula::largest_connected_component(graph);
    REQUIRE(comp == lcc);
  }

  SECTION("works for non-empty undirected graph") {
    reticula::undirected_network graph;
    reticula::component comp = reticula::largest_connected_component(graph);
    REQUIRE(comp.empty());
  }
}

TEST_CASE("connected components", "[reticula::connected_components]") {
  SECTION("works for undirected graph") {
    reticula::undirected_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5, 6}),
      weak2({7, 8, 9});

    auto comps = reticula::connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[0].begin(), comps[0].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[1].begin(), comps[1].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }

  SECTION("works for undirected hypergraph") {
    reticula::undirected_hypernetwork graph(
      {{1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5}),
      weak2({6, 7, 8, 9});

    auto comps = reticula::connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[0].begin(), comps[0].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE_THAT(
      std::vector<reticula::VertexType>(comps[1].begin(), comps[1].end()),
      UnorderedRangeEquals(weak1) || UnorderedRangeEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }
}

TEST_CASE("is connected?", "[reticula::is_connected]") {
  reticula::undirected_network graph(
    {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});
  REQUIRE_FALSE(reticula::is_connected(graph));
  reticula::undirected_network connected(
    {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(reticula::is_connected(connected));

  reticula::undirected_network empty;
  REQUIRE(reticula::is_connected(empty));
}

TEST_CASE("is reachable (static networks)", "[reticula::is_reachable]") {
  SECTION("works for undirected graph") {
    reticula::undirected_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});
    REQUIRE(reticula::is_reachable(graph, 1, 1));
    REQUIRE(reticula::is_reachable(graph, 1, 6));
    REQUIRE(reticula::is_reachable(graph, 6, 1));
    REQUIRE(reticula::is_reachable(graph, 7, 9));

    REQUIRE_FALSE(reticula::is_reachable(graph, 1, 7));
    REQUIRE_FALSE(reticula::is_reachable(graph, 7, 1));
  }

  SECTION("works for undirected hypergraph") {
    reticula::undirected_hypernetwork graph(
      {{1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    REQUIRE(reticula::is_reachable(graph, 1, 1));
    REQUIRE(reticula::is_reachable(graph, 1, 5));
    REQUIRE(reticula::is_reachable(graph, 5, 1));
    REQUIRE(reticula::is_reachable(graph, 7, 9));

    REQUIRE_FALSE(reticula::is_reachable(graph, 1, 7));
    REQUIRE_FALSE(reticula::is_reachable(graph, 7, 1));
  }

  SECTION("gives correct answer on a directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    REQUIRE(reticula::is_reachable(graph, 2, 2));
    REQUIRE(reticula::is_reachable(graph, 2, 5));
    REQUIRE(reticula::is_reachable(graph, 2, 6));
    REQUIRE(reticula::is_reachable(graph, 5, 6));

    REQUIRE_FALSE(reticula::is_reachable(graph, 2, 1));
    REQUIRE_FALSE(reticula::is_reachable(graph, 6, 2));
  }

  SECTION("gives correct answer on a directed hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}}, {{4}, {2, 3}}});

    REQUIRE(reticula::is_reachable(graph, 2, 2));
    REQUIRE(reticula::is_reachable(graph, 2, 5));
    REQUIRE(reticula::is_reachable(graph, 2, 6));
    REQUIRE(reticula::is_reachable(graph, 5, 6));

    REQUIRE_FALSE(reticula::is_reachable(graph, 5, 7));
    REQUIRE_FALSE(reticula::is_reachable(graph, 6, 2));
  }
}

TEST_CASE("connected component", "[reticula::connected_component]") {
  SECTION("works for undirected graph") {
    reticula::undirected_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5, 6}),
      weak2({7, 8, 9});

    for (auto i : weak1) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak1));
    }

    for (auto i : weak2) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak2));
    }
  }

  SECTION("works for undirected hypergraph") {
    reticula::undirected_hypernetwork graph(
      {{1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    std::vector<reticula::VertexType> weak1({1, 2, 3, 4, 5}),
      weak2({6, 7, 8, 9});

    for (auto i : weak1) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak1));
    }

    for (auto i : weak2) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(weak2));
    }
  }
}

TEST_CASE(
  "largest strongly connected component",
  "[reticula::largest_strongly_connected_component]") {
  SECTION("works for non-empty directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    reticula::component largest_scc({1, 2, 3, 4, 5});
    reticula::component comp =
      reticula::largest_strongly_connected_component(graph);
    REQUIRE(comp == largest_scc);
  }

  SECTION("works for empty directed graph") {
    reticula::directed_network graph;
    reticula::component comp =
      reticula::largest_strongly_connected_component(graph);
    REQUIRE(comp.empty());
  }
}

TEST_CASE(
  "strongly connected component", "[reticula::strongly_connected_component]") {
  SECTION("works for directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    // Vertices 1, 2, 3, 4, 5 form the largest SCC
    std::vector<reticula::VertexType> scc_vertices({1, 2, 3, 4, 5});
    for (auto v : scc_vertices) {
      auto comp = reticula::strongly_connected_component(graph, v);
      REQUIRE_THAT(
        std::vector<reticula::VertexType>(comp.begin(), comp.end()),
        UnorderedRangeEquals(scc_vertices));
    }

    // Vertex 6 is in its own SCC
    auto comp6 = reticula::strongly_connected_component(graph, 6);
    REQUIRE(comp6.size() == 1);
    REQUIRE(comp6.contains(6));

    // Vertices 7, 8, 9 are each in their own SCC
    for (auto v : {7uz, 8uz, 9uz}) {
      auto comp = reticula::strongly_connected_component(graph, v);
      REQUIRE(comp.size() == 1);
      REQUIRE(comp.contains(v));
    }
  }
}

TEST_CASE("is strongly connected?", "[reticula::is_strongly_connected]") {
  SECTION("works for strongly connected directed graph") {
    reticula::directed_network strongly_connected({{1, 2}, {2, 3}, {3, 1}});
    REQUIRE(reticula::is_strongly_connected(strongly_connected));
  }

  SECTION("works for non-strongly connected directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});
    REQUIRE_FALSE(reticula::is_strongly_connected(graph));
  }

  SECTION("works for empty directed graph") {
    reticula::directed_network empty;
    REQUIRE(reticula::is_strongly_connected(empty));
  }

  SECTION("works for single vertex directed graph") {
    reticula::directed_network single({}, {1});
    REQUIRE(reticula::is_strongly_connected(single));
  }
}

TEST_CASE("is strongly reachable?", "[reticula::is_strongly_reachable]") {
  SECTION("works for directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    // Within the strongly connected component {1, 2, 3, 4, 5}
    REQUIRE(reticula::is_strongly_reachable(graph, 1, 2));
    REQUIRE(reticula::is_strongly_reachable(graph, 2, 1));
    REQUIRE(reticula::is_strongly_reachable(graph, 1, 3));
    REQUIRE(reticula::is_strongly_reachable(graph, 3, 1));
    REQUIRE(reticula::is_strongly_reachable(graph, 2, 4));
    REQUIRE(reticula::is_strongly_reachable(graph, 4, 2));
    REQUIRE(reticula::is_strongly_reachable(graph, 3, 5));
    REQUIRE(reticula::is_strongly_reachable(graph, 5, 3));

    // Self-reachability
    REQUIRE(reticula::is_strongly_reachable(graph, 1, 1));
    REQUIRE(reticula::is_strongly_reachable(graph, 6, 6));

    // Between different SCCs
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 5, 6));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 6, 5));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 1, 7));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 7, 1));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 7, 8));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 8, 7));
  }

  SECTION("works for directed hypergraph") {
    reticula::directed_hypernetwork graph(
      {{{1, 2}, {3}},
       {{3}, {4, 5}},
       {{4, 1, 5}, {1, 2, 3}},
       {{1}, {6}},
       {{6}, {7, 8}},
       {{8, 9}, {10}}});

    // Within the strongly connected component {1, 2, 3, 4, 5}
    REQUIRE(reticula::is_strongly_reachable(graph, 1, 2));
    REQUIRE(reticula::is_strongly_reachable(graph, 2, 1));
    REQUIRE(reticula::is_strongly_reachable(graph, 1, 3));
    REQUIRE(reticula::is_strongly_reachable(graph, 3, 1));
    REQUIRE(reticula::is_strongly_reachable(graph, 3, 4));
    REQUIRE(reticula::is_strongly_reachable(graph, 4, 3));

    // Between different SCCs
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 1, 6));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 6, 1));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 6, 7));
    REQUIRE_FALSE(reticula::is_strongly_reachable(graph, 7, 6));
  }
}

TEST_CASE("is weakly reachable?", "[reticula::is_weakly_reachable]") {
  SECTION("works for directed graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    // Within the same weakly connected component
    REQUIRE(reticula::is_weakly_reachable(graph, 1, 6));
    REQUIRE(reticula::is_weakly_reachable(graph, 6, 1));
    REQUIRE(reticula::is_weakly_reachable(graph, 2, 5));
    REQUIRE(reticula::is_weakly_reachable(graph, 5, 2));

    // Self-reachability
    REQUIRE(reticula::is_weakly_reachable(graph, 1, 1));
    REQUIRE(reticula::is_weakly_reachable(graph, 7, 7));

    // Between different weakly connected components
    REQUIRE_FALSE(reticula::is_weakly_reachable(graph, 1, 7));
    REQUIRE_FALSE(reticula::is_weakly_reachable(graph, 7, 1));
    REQUIRE_FALSE(reticula::is_weakly_reachable(graph, 6, 8));
    REQUIRE_FALSE(reticula::is_weakly_reachable(graph, 8, 6));

    // Within the second weakly connected component
    REQUIRE(reticula::is_weakly_reachable(graph, 7, 9));
    REQUIRE(reticula::is_weakly_reachable(graph, 9, 7));
  }
}
