#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

using Catch::Approx;
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Contains;
using Catch::Matchers::Equals;

#include <reticula/ranges.hpp>
#include <reticula/utils.hpp>
#include <reticula/temporal_edges.hpp>
#include <reticula/networks.hpp>
#include <reticula/algorithms.hpp>
#include <reticula/generators.hpp>
#include <reticula/random_networks.hpp>

TEST_CASE("out component", "[reticula::out_component]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c6 = reticula::out_component(graph, 6);
    REQUIRE_THAT(std::vector<int>(c6.begin(), c6.end()),
      UnorderedEquals(std::vector<int>({6})));
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));

    auto c6 = reticula::out_component(graph, 6);
    REQUIRE_THAT(std::vector<int>(c6.begin(), c6.end()),
      UnorderedEquals(std::vector<int>({6})));

    auto c7 = reticula::out_component(graph, 7);
    REQUIRE_THAT(std::vector<int>(c7.begin(), c7.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6, 7})));
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({5, 4, 6})));
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    auto c2 = reticula::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c5 = reticula::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({5, 4, 6})));

    auto c7 = reticula::out_component(graph, 7);
    REQUIRE_THAT(std::vector<int>(c7.begin(), c7.end()),
      UnorderedEquals(std::vector<int>({7, 3, 4, 5, 6})));
  }
}

TEST_CASE("out components", "[reticula::out_components]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    for (auto& [v, c]: reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({3, 4, 5, 6, 2})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({4, 2, 3, 5, 6})));
      else if (v == 5)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({4, 2, 3, 5, 6})));
      else if (v == 6)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({6})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on another cyclic graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 2}}, {4, 1, 2, 3});

    for (auto& [v, c]: reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({2, 3})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({2, 3})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({4})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 3}, {2, 2}, {3, 2}, {4, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on another cyclic graph with self-loops") {
    reticula::directed_network<int> graph({
        {1, 1}, {2, 3}}, {1, 2, 3, 4});

    for (auto& [v, c]: reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({2, 3})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({3})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({4})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 2}, {3, 1}, {4, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});

    for (auto& [v, c]: reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 3, 4, 5, 6, 2})));
      else if (v == 7)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 4, 2, 3, 5, 6, 7})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 1}, {7, 7}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    for (auto& [v, c]: reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({3, 4, 5, 6})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({4})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 5}, {3, 4}, {4, 1}, {5, 3}, {6, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    for (auto& [v, c]: reticula::out_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({3, 4, 5, 6})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({4})));
      else if (v == 7)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({7, 3, 4, 5, 6})));

    auto comp_sizes = reticula::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 5}, {3, 4}, {4, 1}, {5, 3}, {6, 1}, {7, 5}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }
}

TEST_CASE("in component", "[reticula::in_component]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));

    auto c1 = reticula::in_component(graph, 1);
    REQUIRE_THAT(std::vector<int>(c1.begin(), c1.end()),
      UnorderedEquals(std::vector<int>({1})));
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));

    auto c1 = reticula::in_component(graph, 7);
    REQUIRE_THAT(std::vector<int>(c1.begin(), c1.end()),
      UnorderedEquals(std::vector<int>({7})));
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 1})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 5})));
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    auto c2 = reticula::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 1})));

    auto c5 = reticula::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 5, 7})));
  }
}

TEST_CASE("in components", "[reticula::in_components]") {
  SECTION("gives correct answer on a cyclic graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    for (auto& [v, c]: reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));
      else if (v == 5)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));
      else if (v == 6)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));


    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 6}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});

    for (auto& [v, c]: reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));
      else if (v == 7)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({7})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 7}, {7, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }


  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    for (auto& [v, c]: reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 2}, {3, 3}, {4, 5}, {5, 4}, {6, 5}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    reticula::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    for (auto& [v, c]: reticula::in_components(graph))
      if (v == 1)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1})));
      else if (v == 2)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2})));
      else if (v == 3)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 7})));
      else if (v == 4)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));
      else if (v == 6)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({1, 2, 3, 5, 6, 7})));
      else if (v == 7)
        REQUIRE_THAT(std::vector<int>(c.begin(), c.end()),
            UnorderedEquals(std::vector<int>({7})));

    auto comp_sizes = reticula::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 2}, {3, 4}, {4, 6}, {5, 5}, {6, 6}, {7, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = reticula::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }
}

TEST_CASE("is acyclic", "[reticula::is_acyclic]") {
  REQUIRE(reticula::is_acyclic(
        reticula::directed_network<int>(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}})));

  REQUIRE(reticula::is_acyclic(
        reticula::directed_hypernetwork<int>({
          {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}},
          {{5}, {4}}})));

  REQUIRE_FALSE(reticula::is_acyclic(
        reticula::directed_network<int>(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}})));

  REQUIRE_FALSE(reticula::is_acyclic(
        reticula::directed_hypernetwork<int>({
          {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}},
          {{4}, {2, 3}}})));
}

TEST_CASE("try topological ordering", "[reticula::try_topological_order]") {
  SECTION("returns nullopt on a cyclic graph") {
    REQUIRE_FALSE(reticula::try_topological_order(
          reticula::directed_network<int>(
            {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}})));

    REQUIRE_FALSE(reticula::try_topological_order(
          reticula::directed_network<int>(
            {{0, 1}, {1, 2}, {2, 1}}, {0, 1, 2, 3})));

    // self-loop
    REQUIRE_FALSE(reticula::try_topological_order(
          reticula::directed_network<int>(
            {{0, 0}, {1, 2}}, {0, 1, 2})));

    REQUIRE_FALSE(reticula::try_topological_order(
        reticula::directed_hypernetwork<int>({
          {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}},
          {{4}, {2, 3}}})));
  }

  SECTION("returns a correct answer on an acyclic graph") {
    auto maybe_topo = reticula::try_topological_order(
          reticula::directed_network<int>(
            {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(maybe_topo.value(),
        Equals(std::vector<int>({1, 2, 3, 5, 4, 6})) ||
        Equals(std::vector<int>({1, 2, 3, 5, 6, 4})));
  }

  SECTION("returns a correct answer on an acyclic graph with isolated node") {
    auto maybe_topo = reticula::try_topological_order(
          reticula::directed_network<int>(
            {{1, 2}, {2, 3}}, {0, 1, 2, 3}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(maybe_topo.value(),
        Equals(std::vector<int>({0, 1, 2, 3})) ||
        Equals(std::vector<int>({1, 0, 2, 3})) ||
        Equals(std::vector<int>({1, 2, 0, 3})) ||
        Equals(std::vector<int>({1, 2, 3, 0})));
  }

  SECTION("returns a correct answer on an acyclic hypergraph") {
    auto maybe_topo = reticula::try_topological_order(
          reticula::directed_hypernetwork<int>({
            {{1}, {2, 3}}, {{2}, {4}}, {{3}, {4}}, {{4}, {5}}}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(maybe_topo.value(),
        Equals(std::vector<int>({1, 2, 3, 4, 5})) ||
        Equals(std::vector<int>({1, 3, 2, 4, 5})));
  }


  SECTION("gives a correct answer on a larger acyclic ") {
    reticula::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});

    auto maybe_topo = reticula::try_topological_order(graph);
    REQUIRE(maybe_topo);

    std::unordered_map<int, std::size_t> topo_pos;
    for (std::size_t pos = 0; auto i: *maybe_topo)
      topo_pos[i] = pos++;

    REQUIRE(reticula::ranges::all_of(graph.edges(),
          [&topo_pos](const auto e) {
            for (auto in: e.mutator_verts())
              for (auto out: e.mutated_verts())
                if (topo_pos[in] >= topo_pos[out])
                  return false;
            return true;
          }));
  }
}

TEST_CASE("topological ordering", "[reticula::topological_order]") {
  SECTION("throws on a cyclic graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE_THROWS_AS(reticula::topological_order(graph),
        reticula::utils::not_acyclic_error);
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto topo = reticula::topological_order(graph);
    REQUIRE_THAT(topo, Equals(std::vector<int>({1, 2, 3, 5, 4, 6})) ||
                        Equals(std::vector<int>({1, 2, 3, 5, 6, 4})));
  }


  SECTION("returns a correct answer on an acyclic hypergraph") {
    auto topo = reticula::topological_order(
          reticula::directed_hypernetwork<int>({
            {{1}, {2, 3}}, {{2}, {4}}, {{3}, {4}}, {{4}, {5}}}));
    REQUIRE_THAT(topo,
        Equals(std::vector<int>({1, 2, 3, 4, 5})) ||
        Equals(std::vector<int>({1, 3, 2, 4, 5})));
  }

  SECTION("gives a correct answer on a larger acyclic ") {
    reticula::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});

    auto topo = reticula::topological_order(graph);
    std::unordered_map<int, std::size_t> topo_pos;
    for (std::size_t pos = 0; auto i: topo)
      topo_pos[i] = pos++;

    REQUIRE(reticula::ranges::all_of(graph.edges(),
          [&topo_pos](const auto e) {
            for (auto in: e.mutator_verts())
              for (auto out: e.mutated_verts())
                if (topo_pos[in] >= topo_pos[out])
                  return false;
            return true;
          }));
  }
}

TEST_CASE("largest weakly connected component",
    "[reticula::largest_weakly_connected_component]") {
  SECTION("works for non-empty directed graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    reticula::component<int> weak({1, 2, 3, 4, 5, 6});
    reticula::component<int> comp =
      reticula::largest_weakly_connected_component(graph);
    REQUIRE(comp == weak);
  }

  SECTION("works for non-empty directed graph") {
    reticula::directed_network<int> graph;
    reticula::component<int> comp =
      reticula::largest_weakly_connected_component(graph);
    REQUIRE(comp.empty());
  }
}

TEST_CASE("weakly connected components",
    "[reticula::weakly_connected_components]") {
  SECTION("works for directed graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    auto comps = reticula::weakly_connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }

  SECTION("works for directed hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{1, 2}, {3}}, {{3}, {4, 5}}, {{6}, {7, 8}}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8});

    auto comps = reticula::weakly_connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }
}

TEST_CASE("is weakly connected?", "[reticula::is_weakly_connected]") {
  reticula::directed_network<int> graph({
      {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});
  REQUIRE_FALSE(reticula::is_weakly_connected(graph));
  reticula::directed_network<int> connected({
          {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(reticula::is_weakly_connected(connected));

  reticula::directed_network<int> empty;
  REQUIRE(reticula::is_weakly_connected(empty));
}

TEST_CASE("weakly connected component",
    "[reticula::weakly_connected_component]") {
  SECTION("works for directed graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    for (auto i: weak1) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }

  SECTION("works for directed hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{1, 2}, {3}}, {{3}, {4, 5}}, {{6}, {7, 8}}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8});

    for (auto i: weak1) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = reticula::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }
}

TEST_CASE("largest connected component",
    "[reticula::largest_connected_component]") {
  SECTION("works for non-empty undirected graph") {
    reticula::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});

    reticula::component<int> lcc({1, 2, 3, 4, 5, 6});
    reticula::component<int> comp =
      reticula::largest_connected_component(graph);
    REQUIRE(comp == lcc);
  }

  SECTION("works for non-empty undirected graph") {
    reticula::undirected_network<int> graph;
    reticula::component<int> comp =
      reticula::largest_connected_component(graph);
    REQUIRE(comp.empty());
  }
}


TEST_CASE("connected components", "[reticula::connected_components]") {
  SECTION("works for undirected graph") {
    reticula::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    auto comps = reticula::connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);

    std::mt19937_64 gen(42);
    auto g = reticula::random_gnp_graph<int>(16384, 0.001, gen);
    BENCHMARK("connected components") {
      return reticula::connected_components(g);
    };
  }

  SECTION("works for undirected hypergraph") {
    reticula::undirected_hypernetwork<int> graph({
        {1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8, 9});

    auto comps = reticula::connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }
}

TEST_CASE("is connected?", "[reticula::is_connected]") {
  reticula::undirected_network<int> graph({
      {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
      {8, 9}});
  REQUIRE_FALSE(reticula::is_connected(graph));
  reticula::undirected_network<int> connected({
      {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(reticula::is_connected(connected));

  reticula::undirected_network<int> empty;
  REQUIRE(reticula::is_connected(empty));
}

TEST_CASE("is reachable (static networks)", "[reticula::is_reachable]") {
  SECTION("works for undirected graph") {
    reticula::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});
    REQUIRE(reticula::is_reachable(graph, 1, 1));
    REQUIRE(reticula::is_reachable(graph, 1, 6));
    REQUIRE(reticula::is_reachable(graph, 6, 1));
    REQUIRE(reticula::is_reachable(graph, 7, 9));

    REQUIRE_FALSE(reticula::is_reachable(graph, 1, 7));
    REQUIRE_FALSE(reticula::is_reachable(graph, 7, 1));
  }

  SECTION("works for undirected hypergraph") {
    reticula::undirected_hypernetwork<int> graph({
        {1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    REQUIRE(reticula::is_reachable(graph, 1, 1));
    REQUIRE(reticula::is_reachable(graph, 1, 5));
    REQUIRE(reticula::is_reachable(graph, 5, 1));
    REQUIRE(reticula::is_reachable(graph, 7, 9));

    REQUIRE_FALSE(reticula::is_reachable(graph, 1, 7));
    REQUIRE_FALSE(reticula::is_reachable(graph, 7, 1));
  }

  SECTION("gives correct answer on a directed graph") {
    reticula::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    REQUIRE(reticula::is_reachable(graph, 2, 2));
    REQUIRE(reticula::is_reachable(graph, 2, 5));
    REQUIRE(reticula::is_reachable(graph, 2, 6));
    REQUIRE(reticula::is_reachable(graph, 5, 6));

    REQUIRE_FALSE(reticula::is_reachable(graph, 2, 1));
    REQUIRE_FALSE(reticula::is_reachable(graph, 6, 2));
  }

  SECTION("gives correct answer on a directed hypergraph") {
    reticula::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});

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
    reticula::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    for (auto i: weak1) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }

  SECTION("works for undirected hypergraph") {
    reticula::undirected_hypernetwork<int> graph({
        {1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8, 9});

    for (auto i: weak1) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = reticula::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }
}

TEST_CASE("is graphic", "[reticula::is_graphic]") {
  REQUIRE(reticula::is_graphic(std::vector<int>({})));
  REQUIRE(reticula::is_graphic(std::vector<int>({5, 3, 3, 3, 2, 2})));
  REQUIRE(reticula::is_graphic(std::vector<int>({3, 3, 2, 1, 1, 0})));
  REQUIRE(reticula::is_graphic(std::vector<int>({5, 1, 1, 1, 1, 1})));
  REQUIRE(reticula::is_graphic(std::vector<int>(5, 2)));
  REQUIRE(reticula::is_graphic(std::vector<int>(5, 4)));

  REQUIRE_FALSE(reticula::is_graphic(std::vector<int>({-1, 3, 3, 3, 2, 2})));
  REQUIRE_FALSE(reticula::is_graphic(std::vector<int>({4, 3, 3, 2, 2, 1})));
  REQUIRE_FALSE(reticula::is_graphic(std::vector<int>({4, 3, 2, 1})));

  std::mt19937_64 gen(42);
  auto ds = reticula::degree_sequence(
    reticula::random_gnp_graph<int>(
      16384, 0.001, gen));
  BENCHMARK("is_graphic") {
    return reticula::is_graphic(ds);
  };
}

TEST_CASE("is digraphic", "[reticula::is_digraphic]") {
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>({})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 0}, {2, 1}, {0, 1}, {1, 1}, {0, 1}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{2, 0}, {2, 1}, {0, 1}, {0, 1}, {0, 1}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{0, 1}, {1, 0}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 1}, {1, 1}})));

  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, -1}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{-1, 0}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 0}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 3}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 0}, {0, 1}, {1, 1}, {3, 1}, {0, 1}})));


  std::mt19937_64 gen(42);
  auto dds = reticula::in_out_degree_pair_sequence(
    reticula::random_directed_gnp_graph<int>(
      16384, 0.001, gen));
  BENCHMARK("is_digraphic") {
    return reticula::is_digraphic(dds);
  };
}

TEST_CASE("shortest path from vert", "[reticula::shortest_path_lengths_from]") {
  reticula::directed_network<int> dg({
      {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

  REQUIRE(reticula::shortest_path_lengths_from(dg, 1) ==
      std::unordered_map<int, std::size_t, reticula::hash<int>>{
        {1, 0}, {2, 1}, {3, 2}, {5, 3}, {6, 4}, {4, 4}});
}

TEST_CASE("shortest path to vert", "[reticula::shortest_path_lengths_to]") {
  reticula::directed_network<int> dg({
      {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

  REQUIRE(reticula::shortest_path_lengths_to(dg, 4) ==
      std::unordered_map<int, std::size_t, reticula::hash<int>>{
        {1, 4}, {2, 3}, {3, 2}, {5, 1}, {4, 0}});
}

TEST_CASE("edge degree functions",
    "[reticula::edge_in_degree][reticula::edge_out_degree]"
    "[reticula::edge_incident_degree][reticula::edge_degree]") {
  REQUIRE(reticula::edge_in_degree(
    reticula::directed_edge<int>(0, 0)) == 1);
  REQUIRE(reticula::edge_in_degree(
    reticula::directed_edge<int>(0, 1)) == 1);
  REQUIRE(reticula::edge_in_degree(
    reticula::directed_hyperedge<int>(
      {0, 1}, {1, 2, 3})) == 2);

  REQUIRE(reticula::edge_out_degree(
    reticula::directed_edge<int>(0, 0)) == 1);
  REQUIRE(reticula::edge_out_degree(
    reticula::directed_edge<int>(0, 1)) == 1);
  REQUIRE(reticula::edge_out_degree(
    reticula::directed_hyperedge<int>(
      {0, 1}, {1, 2, 3})) == 3);

  REQUIRE(reticula::edge_incident_degree(
    reticula::directed_edge<int>(0, 0)) == 1);
  REQUIRE(reticula::edge_incident_degree(
    reticula::directed_edge<int>(0, 1)) == 2);
  REQUIRE(reticula::edge_incident_degree(
    reticula::directed_hyperedge<int>(
      {0, 1}, {1, 2, 3})) == 4);

  REQUIRE(reticula::edge_degree(
    reticula::undirected_edge<int>(0, 0)) == 1);
  REQUIRE(reticula::edge_degree(
    reticula::undirected_edge<int>(0, 1)) == 2);
  REQUIRE(reticula::edge_degree(
    reticula::undirected_hyperedge<int>(
      {0, 1, 2, 3})) == 4);
}

TEST_CASE("degree functions",
    "[reticula::in_degree][reticula::out_degree]"
    "[reticula::incident_degree][reticula::degree]") {
  SECTION("when given an undirected network") {
    reticula::undirected_network<int> graph(
        {{1, 2}, {1, 5}, {5, 2}, {4, 5}, {3, 2}, {4, 3}, {4, 6},
         {1, 2}, {2, 1}, {5, 2}}, {0});

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

    REQUIRE(reticula::in_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1});
    REQUIRE(reticula::out_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1});
    REQUIRE(reticula::incident_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1});
    REQUIRE(reticula::degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1});
    REQUIRE(reticula::in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {0, 0}, {2, 2}, {3, 3}, {2, 2}, {3, 3}, {3, 3}, {1, 1}});

    REQUIRE(reticula::edge_in_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_out_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_incident_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}});
  }

  SECTION("when given an undirected hypergraph") {
    using U = reticula::undirected_hyperedge<int>;
    reticula::undirected_hypernetwork<int> graph(
        {U{1, 2}, U{1, 5}, U{5, 2}, U{4, 5}, U{3, 2}, U{4, 3, 7}, U{4, 6},
         U{1, 2}, U{2, 1}, U{5, 2}}, {0});

    REQUIRE(reticula::incident_degree(graph, 3) == 2);
    REQUIRE(reticula::out_degree(graph, 3) == 2);
    REQUIRE(reticula::in_degree(graph, 3) == 2);
    REQUIRE(reticula::degree(graph, 3) == 2);

    REQUIRE(reticula::incident_degree(graph, 2) == 3);
    REQUIRE(reticula::out_degree(graph, 2) == 3);
    REQUIRE(reticula::in_degree(graph, 2) == 3);
    REQUIRE(reticula::degree(graph, 2) == 3);

    REQUIRE(reticula::incident_degree(graph, 1) == 2);
    REQUIRE(reticula::out_degree(graph, 1) == 2);
    REQUIRE(reticula::in_degree(graph, 1) == 2);
    REQUIRE(reticula::degree(graph, 1) == 2);

    REQUIRE(reticula::incident_degree(graph, 0) == 0);
    REQUIRE(reticula::out_degree(graph, 0) == 0);
    REQUIRE(reticula::in_degree(graph, 0) == 0);
    REQUIRE(reticula::degree(graph, 0) == 0);

    REQUIRE(reticula::in_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1, 1});
    REQUIRE(reticula::out_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1, 1});
    REQUIRE(reticula::incident_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1, 1});
    REQUIRE(reticula::degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 3, 2, 3, 3, 1, 1});
    REQUIRE(reticula::in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {0, 0}, {2, 2}, {3, 3}, {2, 2}, {3, 3}, {3, 3}, {1, 1}, {1, 1}});

    REQUIRE(reticula::edge_in_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 3, 2, 2});
    REQUIRE(reticula::edge_out_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 3, 2, 2});
    REQUIRE(reticula::edge_incident_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 3, 2, 2});
    REQUIRE(reticula::edge_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 3, 2, 2});
    REQUIRE(reticula::edge_in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {2, 2}, {2, 2}, {2, 2}, {2, 2}, {3, 3}, {2, 2}, {2, 2}});
  }

  SECTION("when given an undirected temporal network") {
    reticula::undirected_temporal_network<int, int> graph(
        {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
         {2, 1, 2}, {2, 1, 2}, {1, 2, 5}}, {0});

    REQUIRE(reticula::incident_degree(graph, 3) == 2);
    REQUIRE(reticula::out_degree(graph, 3) == 2);
    REQUIRE(reticula::in_degree(graph, 3) == 2);
    REQUIRE(reticula::degree(graph, 3) == 2);

    REQUIRE(reticula::incident_degree(graph, 2) == 4);
    REQUIRE(reticula::out_degree(graph, 2) == 4);
    REQUIRE(reticula::in_degree(graph, 2) == 4);
    REQUIRE(reticula::degree(graph, 2) == 4);

    REQUIRE(reticula::incident_degree(graph, 1) == 3);
    REQUIRE(reticula::out_degree(graph, 1) == 3);
    REQUIRE(reticula::in_degree(graph, 1) == 3);
    REQUIRE(reticula::degree(graph, 1) == 3);

    REQUIRE(reticula::incident_degree(graph, 0) == 0);
    REQUIRE(reticula::out_degree(graph, 0) == 0);
    REQUIRE(reticula::in_degree(graph, 0) == 0);
    REQUIRE(reticula::degree(graph, 0) == 0);

    REQUIRE(reticula::in_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 3, 4, 2, 1});
    REQUIRE(reticula::out_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 3, 4, 2, 1});
    REQUIRE(reticula::incident_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 3, 4, 2, 1});
    REQUIRE(reticula::degree_sequence(graph) ==
        std::vector<std::size_t>{0, 3, 4, 2, 1});
    REQUIRE(reticula::in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {0, 0}, {3, 3}, {4, 4}, {2, 2}, {1, 1}});

    REQUIRE(reticula::edge_in_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_out_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_incident_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}});
  }

  SECTION("when given a directed network") {
    reticula::directed_network<int> graph(
        {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2},
         {1, 2}, {2, 3}, {3, 5}}, {0});

    REQUIRE(reticula::incident_degree(graph, 3) == 2);
    REQUIRE(reticula::out_degree(graph, 3) == 1);
    REQUIRE(reticula::in_degree(graph, 3) == 1);

    REQUIRE(reticula::incident_degree(graph, 2) == 3);
    REQUIRE(reticula::out_degree(graph, 2) == 1);
    REQUIRE(reticula::in_degree(graph, 2) == 2);

    REQUIRE(reticula::incident_degree(graph, 1) == 1);
    REQUIRE(reticula::out_degree(graph, 1) == 1);
    REQUIRE(reticula::in_degree(graph, 1) == 0);

    REQUIRE(reticula::incident_degree(graph, 0) == 0);
    REQUIRE(reticula::out_degree(graph, 0) == 0);
    REQUIRE(reticula::in_degree(graph, 0) == 0);

    REQUIRE(reticula::incident_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 1, 3, 2, 2, 3, 1});
    REQUIRE(reticula::out_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 1, 1, 1, 1, 2, 0});
    REQUIRE(reticula::in_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 0, 2, 1, 1, 1, 1});
    REQUIRE(reticula::in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {0, 0}, {0, 1}, {2, 1}, {1, 1}, {1, 1}, {1, 2}, {1, 0}});

    REQUIRE(reticula::edge_in_degree_sequence(graph) ==
        std::vector<std::size_t>{1, 1, 1, 1, 1, 1});
    REQUIRE(reticula::edge_out_degree_sequence(graph) ==
        std::vector<std::size_t>{1, 1, 1, 1, 1, 1});
    REQUIRE(reticula::edge_incident_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1}});
  }

  SECTION("when given a directed hypernetwork") {
    reticula::directed_hypernetwork<int> graph(
        {{{1}, {2}}, {{2}, {3, 7}}, {{2}, {3, 7}}, {{3}, {5}}, {{5}, {6}},
        {{5}, {4}}, {{4}, {2}}, {{1}, {2}}, {{2}, {3}}, {{3}, {5}}}, {0});

    REQUIRE(reticula::incident_degree(graph, 3) == 3);
    REQUIRE(reticula::out_degree(graph, 3) == 1);
    REQUIRE(reticula::in_degree(graph, 3) == 2);

    REQUIRE(reticula::incident_degree(graph, 2) == 4);
    REQUIRE(reticula::out_degree(graph, 2) == 2);
    REQUIRE(reticula::in_degree(graph, 2) == 2);

    REQUIRE(reticula::incident_degree(graph, 1) == 1);
    REQUIRE(reticula::out_degree(graph, 1) == 1);
    REQUIRE(reticula::in_degree(graph, 1) == 0);

    REQUIRE(reticula::incident_degree(graph, 0) == 0);
    REQUIRE(reticula::out_degree(graph, 0) == 0);
    REQUIRE(reticula::in_degree(graph, 0) == 0);

    REQUIRE(reticula::incident_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 1, 4, 3, 2, 3, 1, 1});
    REQUIRE(reticula::out_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 1, 2, 1, 1, 2, 0, 0});
    REQUIRE(reticula::in_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 0, 2, 2, 1, 1, 1, 1});
    REQUIRE(reticula::in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {0, 0}, {0, 1}, {2, 2}, {2, 1}, {1, 1}, {1, 2}, {1, 0}, {1, 0}});

    REQUIRE(reticula::edge_in_degree_sequence(graph) ==
        std::vector<std::size_t>{1, 1, 1, 1, 1, 1, 1});
    REQUIRE(reticula::edge_out_degree_sequence(graph) ==
        std::vector<std::size_t>{1, 1, 2, 1, 1, 1, 1});
    REQUIRE(reticula::edge_incident_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 3, 2, 2, 2, 2});
    REQUIRE(reticula::edge_in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {1, 1}, {1, 1}, {1, 2}, {1, 1},
          {1, 1}, {1, 1}, {1, 1}});
  }

  SECTION("when given an directed temporal network") {
    reticula::directed_temporal_network<int, int> graph(
        {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
         {2, 1, 2}, {2, 1, 2}, {1, 2, 5}}, {0});

    REQUIRE(reticula::incident_degree(graph, 3) == 2);
    REQUIRE(reticula::out_degree(graph, 3) == 1);
    REQUIRE(reticula::in_degree(graph, 3) == 1);

    REQUIRE(reticula::incident_degree(graph, 2) == 4);
    REQUIRE(reticula::out_degree(graph, 2) == 2);
    REQUIRE(reticula::in_degree(graph, 2) == 2);

    REQUIRE(reticula::incident_degree(graph, 1) == 3);
    REQUIRE(reticula::out_degree(graph, 1) == 2);
    REQUIRE(reticula::in_degree(graph, 1) == 1);

    REQUIRE(reticula::incident_degree(graph, 0) == 0);
    REQUIRE(reticula::out_degree(graph, 0) == 0);
    REQUIRE(reticula::in_degree(graph, 0) == 0);

    REQUIRE(reticula::incident_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 3, 4, 2, 1});
    REQUIRE(reticula::out_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 2, 2, 1, 0});
    REQUIRE(reticula::in_degree_sequence(graph) ==
        std::vector<std::size_t>{0, 1, 2, 1, 1});
    REQUIRE(reticula::in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {0, 0}, {1, 2}, {2, 2}, {1, 1}, {1, 0}});

    REQUIRE(reticula::edge_in_degree_sequence(graph) ==
        std::vector<std::size_t>{1, 1, 1, 1, 1});
    REQUIRE(reticula::edge_out_degree_sequence(graph) ==
        std::vector<std::size_t>{1, 1, 1, 1, 1});
    REQUIRE(reticula::edge_incident_degree_sequence(graph) ==
        std::vector<std::size_t>{2, 2, 2, 2, 2});
    REQUIRE(reticula::edge_in_out_degree_pair_sequence(graph) ==
        std::vector<std::pair<std::size_t, std::size_t>>{
          {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1}});
  }
}

TEST_CASE("degree assortativity", "[reticula::degree_assortativity]") {
  SECTION("undirected") {
    auto g = reticula::complete_graph(10);
    REQUIRE(std::isnan(reticula::degree_assortativity(g)));

    std::vector<int> vert_degs(10000), edge_degs(5000);
    reticula::ranges::fill(vert_degs, 4);
    reticula::ranges::fill(edge_degs, 8);
    std::mt19937 gen(42);

    auto net = reticula::random_expected_degree_sequence_hypergraph<int>(
        vert_degs, edge_degs, gen);
    REQUIRE(reticula::degree_assortativity(net) == Approx(0.0).margin(1e-2));

    reticula::undirected_network<int> karate_club({
        {1, 2}, {1, 3}, {2, 3}, {1, 4}, {2, 4}, {3, 4}, {1, 5}, {1, 6}, {1, 7},
        {5, 7}, {6, 7}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {1, 9}, {3, 9}, {3, 10},
        {1, 11}, {5, 11}, {6, 11}, {1, 12}, {1, 13}, {4, 13}, {1, 14}, {2, 14},
        {3, 14}, {4, 14}, {6, 17}, {7, 17}, {1, 18}, {2, 18}, {1, 20}, {2, 20},
        {1, 22}, {2, 22}, {24, 26}, {25, 26}, {3, 28}, {24, 28}, {25, 28},
        {3, 29}, {24, 30}, {27, 30}, {2, 31}, {9, 31}, {1, 32}, {25, 32},
        {26, 32}, {29, 32}, {3, 33}, {9, 33}, {15, 33}, {16, 33}, {19, 33},
        {21, 33}, {23, 33}, {24, 33}, {30, 33}, {31, 33}, {32, 33}, {9, 34},
        {10, 34}, {14, 34}, {15, 34}, {16, 34}, {19, 34}, {20, 34}, {21, 34},
        {23, 34}, {24, 34}, {27, 34}, {28, 34}, {29, 34}, {30, 34}, {31, 34},
        {32, 34}, {33, 34}});

    REQUIRE(reticula::degree_assortativity(karate_club) ==
        Approx(-0.4756).margin(1e-4));

    reticula::undirected_network<int> zebra({
        {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10},
        {1, 11}, {1, 12}, {1, 13}, {1, 14}, {2, 3}, {2, 4}, {2, 6}, {2, 7},
        {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 12}, {2, 13}, {2, 14}, {2, 15},
        {3, 4}, {3, 6}, {3, 7}, {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12},
        {3, 13}, {3, 14}, {3, 15}, {4, 5}, {4, 6}, {4, 7}, {4, 8}, {4, 9},
        {4, 10}, {4, 11}, {4, 12}, {4, 14}, {4, 15}, {5, 8}, {6, 7}, {6, 8},
        {6, 9}, {6, 10}, {6, 11}, {6, 12}, {6, 13}, {6, 15}, {7, 9}, {7, 10},
        {7, 11}, {7, 12}, {7, 13}, {7, 14}, {7, 15}, {8, 9}, {8, 10}, {8, 11},
        {8, 12}, {8, 13}, {8, 14}, {8, 15}, {9, 10}, {9, 11}, {9, 12}, {9, 13},
        {9, 14}, {9, 15}, {10, 11}, {10, 12}, {10, 13}, {10, 15}, {10, 16},
        {11, 12}, {11, 13}, {11, 14}, {11, 15}, {12, 13}, {12, 14}, {12, 15},
        {13, 15}, {13, 17}, {13, 22}, {13, 23}, {14, 15}, {14, 16}, {16, 17},
        {16, 18}, {16, 19}, {16, 20}, {16, 21}, {17, 18}, {17, 22}, {17, 23},
        {19, 20}, {19, 21}, {20, 21}, {22, 23}, {24, 25}, {24, 26}, {24, 27},
        {25, 26}, {25, 27}, {26, 27}});
    REQUIRE(reticula::degree_assortativity(zebra) ==
        Approx(0.7177).margin(1e-4));

    BENCHMARK("undirected degree assortativity") {
      return reticula::degree_assortativity(net);
    };
  }

  SECTION("directed") {
    auto g = reticula::complete_directed_graph(10);
    REQUIRE(std::isnan(reticula::out_in_degree_assortativity(g)));

    reticula::directed_network<int> rhesus_macaques({
        {1, 2}, {1, 3}, {4, 2}, {5, 6}, {5, 7}, {5, 8}, {5, 3}, {5, 9}, {5, 10},
        {5, 11}, {5, 12}, {5, 13}, {5, 14}, {6, 1}, {6, 5}, {6, 8}, {6, 3},
        {6, 10}, {6, 12}, {6, 15}, {7, 5}, {7, 8}, {7, 2}, {7, 9}, {7, 14},
        {16, 2}, {16, 3}, {16, 9}, {16, 11}, {16, 12}, {16, 15}, {8, 6}, {8, 7},
        {2, 1}, {2, 4}, {2, 16}, {2, 3}, {2, 9}, {2, 10}, {2, 11}, {2, 12},
        {2, 15}, {3, 1}, {3, 2}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 15},
        {3, 13}, {9, 2}, {9, 3}, {9, 11}, {9, 15}, {9, 13}, {9, 14}, {10, 1},
        {10, 5}, {10, 6}, {10, 2}, {10, 3}, {10, 9}, {10, 11}, {10, 12},
        {10, 15}, {10, 13}, {11, 1}, {11, 5}, {11, 16}, {11, 2}, {11, 3},
        {11, 9}, {11, 10}, {11, 15}, {11, 13}, {12, 5}, {12, 6}, {12, 8},
        {12, 2}, {12, 3}, {12, 10}, {12, 11}, {12, 15}, {12, 13}, {12, 14},
        {15, 4}, {15, 16}, {15, 2}, {15, 3}, {15, 10}, {15, 11}, {15, 12},
        {15, 14}, {13, 1}, {13, 5}, {13, 3}, {13, 9}, {13, 10}, {13, 11},
        {13, 12}, {13, 15}, {14, 4}, {14, 7}, {14, 8}, {14, 2}, {14, 3},
        {14, 9}, {14, 10}, {14, 12}, {14, 15}, {14, 13}});
    REQUIRE(reticula::in_in_degree_assortativity(rhesus_macaques) ==
        Approx(-0.0046).margin(1e-4));
    REQUIRE(reticula::in_out_degree_assortativity(rhesus_macaques) ==
        Approx(0.0203).margin(1e-4));
    REQUIRE(reticula::out_in_degree_assortativity(rhesus_macaques) ==
        Approx(-0.0914).margin(1e-4));
    REQUIRE(reticula::out_out_degree_assortativity(rhesus_macaques) ==
        Approx(-0.0583).margin(1e-4));
  }
}

TEST_CASE("attribute assortativity", "[reticula::attribute_assortativity]") {
  SECTION("undirected") {
    reticula::undirected_network<int> net({{0, 1}, {2, 3}});
    reticula::undirected_network<int> net2({{0, 2}, {1, 3}});

    SECTION("with function") {
      REQUIRE(reticula::attribute_assortativity(net,
            [](int v){ return v < 2 ? 1 : 2; }) == Approx(1.0));

      REQUIRE(reticula::attribute_assortativity(net2,
            [](int v){ return v < 2 ? 1 : 2; }) == Approx(-1.0));

      REQUIRE(reticula::attribute_assortativity(net,
            [](int v){ return v; }) == Approx(0.6));

      REQUIRE(reticula::attribute_assortativity(net2,
            [](int v){ return v; }) == Approx(-0.6));
    }

    SECTION("with mapping") {
      std::unordered_map<int, double> m{{1, 1}, {2, 2}, {3, 2}};
      REQUIRE(reticula::attribute_assortativity(net, m, 1.0) ==
          Approx(1.0));

      REQUIRE(reticula::attribute_assortativity(net2, m, 1.0) ==
          Approx(-1.0));

      std::unordered_map<int, double> m2{{1, 1}, {2, 2}, {3, 3}};
      REQUIRE(reticula::attribute_assortativity(net, m2, 0.0) ==
          Approx(0.6));

      REQUIRE(reticula::attribute_assortativity(net2, m2, 0.0) ==
          Approx(-0.6));
    }
  }

  SECTION("directed") {
    reticula::directed_network<int> net({{0, 1}, {2, 3}, {0, 3}});
    reticula::directed_network<int> net2({{0, 2}, {3, 1}, {0, 3}});

    SECTION("with function") {
      REQUIRE(reticula::attribute_assortativity(net,
            [](int v){ return v < 2 ? 1 : 2; },
            [](int v){ return v < 2 ? 2 : 3; }) == Approx(0.5));

      REQUIRE(reticula::attribute_assortativity(net2,
            [](int v){ return v < 2 ? 1 : 2; },
            [](int v){ return v < 2 ? 2 : 3; }) == Approx(-1.0));

      REQUIRE(reticula::attribute_assortativity(net,
            [](int v){ return v; },
            [](int v){ return v + 1; }) == Approx(0.5));
      REQUIRE(reticula::attribute_assortativity(net2,
            [](int v){ return v; },
            [](int v){ return v + 1; }) == Approx(-std::sqrt(3)/2));
    }

    SECTION("with mapping") {
      std::unordered_map<int, double> m_in{{1, 1}, {2, 2}, {3, 2}};
      std::unordered_map<int, double> m_out{{1, 2}, {2, 3}, {3, 3}};
      REQUIRE(reticula::attribute_assortativity(net, m_in, m_out, 1.0, 2.0) ==
          Approx(0.5));

      REQUIRE(reticula::attribute_assortativity(net2, m_in, m_out, 1.0, 2.0) ==
          Approx(-1.0));

      std::unordered_map<int, double> m2_in{{1, 1}, {2, 2}, {3, 3}};
      std::unordered_map<int, double> m2_out{{1, 2}, {2, 3}, {3, 4}};
      REQUIRE(reticula::attribute_assortativity(
            net, m2_in, m2_out, 0.0, 1.0) == Approx(0.5));

      REQUIRE(reticula::attribute_assortativity(
            net2, m2_in, m2_out, 0.0, 1.0) == Approx(-std::sqrt(3)/2));
    }
  }
}

TEST_CASE("network density", "[reticula::density]") {
  reticula::undirected_network<int> ug({
      {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
      {8, 9}});

  REQUIRE(reticula::density(ug) == Approx(0.25));

  reticula::directed_network<int> dg({
      {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(reticula::density(dg) == Approx(0.2));
}
