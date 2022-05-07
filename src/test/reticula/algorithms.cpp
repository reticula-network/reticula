#include <vector>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Contains;
using Catch::Matchers::Equals;

#include <reticula/utils.hpp>
#include <reticula/temporal_edges.hpp>
#include <reticula/networks.hpp>
#include <reticula/algorithms.hpp>

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

    REQUIRE(std::ranges::all_of(graph.edges(),
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

    REQUIRE(std::ranges::all_of(graph.edges(),
          [&topo_pos](const auto e) {
            for (auto in: e.mutator_verts())
              for (auto out: e.mutated_verts())
                if (topo_pos[in] >= topo_pos[out])
                  return false;
            return true;
          }));
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


TEST_CASE("cartesian product", "[reticula::cartesian_product]") {
    reticula::undirected_network<int> graph1({{1, 2}, {1, 3}});
    reticula::undirected_network<int> graph2({{1, 2}, {2, 3}});

    reticula::undirected_network<std::pair<int, int>> prod =
      reticula::cartesian_product(graph1, graph2);

    REQUIRE(prod.vertices().size() ==
        graph1.vertices().size()*graph2.vertices().size());
    REQUIRE(prod.edges().size() ==
        graph1.vertices().size()*graph2.edges().size() +
        graph2.vertices().size()*graph1.edges().size());

    REQUIRE_THAT(prod.edges(),
        UnorderedEquals(
          std::vector<reticula::undirected_edge<std::pair<int, int>>>{
            {{1, 1}, {2, 1}}, {{1, 1}, {3, 1}}, {{1, 1}, {1, 2}},
            {{1, 2}, {2, 2}}, {{1, 2}, {3, 2}}, {{1, 2}, {1, 3}},
            {{1, 3}, {2, 3}}, {{1, 3}, {3, 3}},
            {{2, 1}, {2, 2}},
            {{2, 2}, {2, 3}},
            {{3, 1}, {3, 2}},
            {{3, 2}, {3, 3}}}));
}

TEST_CASE("relabel nodes", "[reticula::relabel_nodes]") {
  // let's make a graph with more to it than integral vertices
  reticula::undirected_network<int> graph1({{1, 2}, {1, 3}});
  reticula::undirected_network<int> graph2({{1, 2}, {1, 3}});
  reticula::undirected_network<std::pair<int, int>> orig =
    reticula::cartesian_product(graph1, graph2);

  reticula::undirected_network<std::size_t> relabeled =
    reticula::relabel_nodes<std::size_t>(orig);

  REQUIRE(orig.vertices().size() == relabeled.vertices().size());
  REQUIRE(orig.edges().size() == relabeled.edges().size());
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
}

TEST_CASE("vertex induced subgraph", "[reticula::vertex_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with components") {
    reticula::component<typename EdgeType::VertexType> comp({1, 2, 3, 4, 9});
    auto res = reticula::vertex_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::vertex_induced_subgraph(network, {1, 2, 3, 4, 9});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedEquals(res.edges()));
  }
}

TEST_CASE("edge induced subgraph", "[reticula::edge_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with component") {
    reticula::component<EdgeType> comp({
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});
    auto res = reticula::edge_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::edge_induced_subgraph(network, {
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedEquals(res.edges()));
  }
}

TEST_CASE("graph union", "[reticula::graph_union]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  reticula::network<EdgeType> n2(
      {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = reticula::graph_union(n1, n2);
  auto res2 = reticula::graph_union(n1, n2);
  REQUIRE(res1.edges() == res2.edges());
  REQUIRE_THAT(res1.edges(), Contains(n1.edges()));
  REQUIRE_THAT(res1.edges(), Contains(n2.edges()));

  REQUIRE(res1.vertices() == res2.vertices());
  REQUIRE_THAT(res1.vertices(), Contains(n1.vertices()));
  REQUIRE_THAT(res1.vertices(), Contains(n2.vertices()));
}

TEST_CASE("with edges", "[reticula::with_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_edges(n1, {
      {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2},
          {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}));
}

TEST_CASE("with vertices", "[reticula::with_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_vertices(n1, {3, 4, 5, 6});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 5, 6, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
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
