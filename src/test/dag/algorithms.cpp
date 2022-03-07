#include <vector>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Contains;
using Catch::Matchers::Equals;

#include <dag/utils.hpp>
#include <dag/temporal_edges.hpp>
#include <dag/networks.hpp>
#include <dag/algorithms.hpp>

TEST_CASE("out component", "[dag::out_component]") {
  SECTION("gives correct answer on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    auto c2 = dag::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c5 = dag::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c6 = dag::out_component(graph, 6);
    REQUIRE_THAT(std::vector<int>(c6.begin(), c6.end()),
      UnorderedEquals(std::vector<int>({6})));
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});
    auto c2 = dag::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));

    auto c5 = dag::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6})));

    auto c6 = dag::out_component(graph, 6);
    REQUIRE_THAT(std::vector<int>(c6.begin(), c6.end()),
      UnorderedEquals(std::vector<int>({6})));

    auto c7 = dag::out_component(graph, 7);
    REQUIRE_THAT(std::vector<int>(c7.begin(), c7.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 6, 7})));
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto c2 = dag::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c5 = dag::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({5, 4, 6})));
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    dag::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    auto c2 = dag::out_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));

    auto c5 = dag::out_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({5, 4, 6})));

    auto c7 = dag::out_component(graph, 7);
    REQUIRE_THAT(std::vector<int>(c7.begin(), c7.end()),
      UnorderedEquals(std::vector<int>({7, 3, 4, 5, 6})));
  }
}

TEST_CASE("out components", "[dag::out_components]") {
  SECTION("gives correct answer on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    for (auto& [v, c]: dag::out_components(graph))
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

    auto comp_sizes = dag::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});

    for (auto& [v, c]: dag::out_components(graph))
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

    auto comp_sizes = dag::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 1}, {7, 7}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    for (auto& [v, c]: dag::out_components(graph))
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

    auto comp_sizes = dag::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 5}, {3, 4}, {4, 1}, {5, 3}, {6, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    dag::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    for (auto& [v, c]: dag::out_components(graph))
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

    auto comp_sizes = dag::out_component_sizes(graph);
    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 5}, {3, 4}, {4, 1}, {5, 3}, {6, 1}, {7, 5}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::out_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }
}

TEST_CASE("in component", "[dag::in_component]") {
  SECTION("gives correct answer on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    auto c2 = dag::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));

    auto c5 = dag::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5})));

    auto c1 = dag::in_component(graph, 1);
    REQUIRE_THAT(std::vector<int>(c1.begin(), c1.end()),
      UnorderedEquals(std::vector<int>({1})));
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});
    auto c2 = dag::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));

    auto c5 = dag::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 4, 5, 7})));

    auto c1 = dag::in_component(graph, 7);
    REQUIRE_THAT(std::vector<int>(c1.begin(), c1.end()),
      UnorderedEquals(std::vector<int>({7})));
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto c2 = dag::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 1})));

    auto c5 = dag::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 5})));
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    dag::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    auto c2 = dag::in_component(graph, 2);
    REQUIRE_THAT(std::vector<int>(c2.begin(), c2.end()),
      UnorderedEquals(std::vector<int>({2, 1})));

    auto c5 = dag::in_component(graph, 5);
    REQUIRE_THAT(std::vector<int>(c5.begin(), c5.end()),
      UnorderedEquals(std::vector<int>({1, 2, 3, 5, 7})));
  }
}

TEST_CASE("in components", "[dag::in_components]") {
  SECTION("gives correct answer on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    for (auto& [v, c]: dag::in_components(graph))
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


    auto comp_sizes = dag::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 6}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on a cyclic hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});

    for (auto& [v, c]: dag::in_components(graph))
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

    auto comp_sizes = dag::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 7}, {7, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }


  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    for (auto& [v, c]: dag::in_components(graph))
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

    auto comp_sizes = dag::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 2}, {3, 3}, {4, 5}, {5, 4}, {6, 5}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }

  SECTION("gives correct answer on acyclic hypergraphs") {
    dag::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});
    for (auto& [v, c]: dag::in_components(graph))
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

    auto comp_sizes = dag::in_component_sizes(graph);

    std::unordered_map<int, std::size_t> comp_map;
    for (auto [v, c]: comp_sizes)
      comp_map[v] = c.size();

    std::unordered_map<int, std::size_t> res(
          {{1, 1}, {2, 2}, {3, 4}, {4, 6}, {5, 5}, {6, 6}, {7, 1}});

    REQUIRE(comp_map == res);

    auto comp_size_ests = dag::in_component_size_estimates(graph, 0);
    REQUIRE(comp_size_ests.size() == graph.vertices().size());
  }
}

TEST_CASE("is acyclic", "[dag::is_acyclic]") {
  REQUIRE(dag::is_acyclic(
        dag::directed_network<int>(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}})));

  REQUIRE(dag::is_acyclic(
        dag::directed_hypernetwork<int>({
          {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}},
          {{5}, {4}}})));

  REQUIRE_FALSE(dag::is_acyclic(
        dag::directed_network<int>(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}})));

  REQUIRE_FALSE(dag::is_acyclic(
        dag::directed_hypernetwork<int>({
          {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}},
          {{4}, {2, 3}}})));
}

TEST_CASE("try topological ordering", "[dag::try_topological_order]") {
  SECTION("returns nullopt on a cyclic graph") {
    REQUIRE_FALSE(dag::try_topological_order(
          dag::directed_network<int>(
            {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}})));

    REQUIRE_FALSE(dag::try_topological_order(
        dag::directed_hypernetwork<int>({
          {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}}, {{5}, {4}},
          {{4}, {2, 3}}})));
  }

  SECTION("returns a correct answer on an acyclic graph") {
    auto maybe_topo = dag::try_topological_order(
          dag::directed_network<int>(
            {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(maybe_topo.value(),
        Equals(std::vector<int>({1, 2, 3, 5, 4, 6})) ||
        Equals(std::vector<int>({1, 2, 3, 5, 6, 4})));
  }

  SECTION("returns a correct answer on an acyclic hypergraph") {
    auto maybe_topo = dag::try_topological_order(
          dag::directed_hypernetwork<int>({
            {{1}, {2, 3}}, {{2}, {4}}, {{3}, {4}}, {{4}, {5}}}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(maybe_topo.value(),
        Equals(std::vector<int>({1, 2, 3, 4, 5})) ||
        Equals(std::vector<int>({1, 3, 2, 4, 5})));
  }


  SECTION("gives a correct answer on a larger acyclic ") {
    dag::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});

    auto maybe_topo = dag::try_topological_order(graph);
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

TEST_CASE("topological ordering", "[dag::topological_order]") {
  SECTION("throws on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE_THROWS_AS(dag::topological_order(graph),
        dag::utils::not_acyclic_error);
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto topo = dag::topological_order(graph);
    REQUIRE_THAT(topo, Equals(std::vector<int>({1, 2, 3, 5, 4, 6})) ||
                        Equals(std::vector<int>({1, 2, 3, 5, 6, 4})));
  }


  SECTION("returns a correct answer on an acyclic hypergraph") {
    auto topo = dag::topological_order(
          dag::directed_hypernetwork<int>({
            {{1}, {2, 3}}, {{2}, {4}}, {{3}, {4}}, {{4}, {5}}}));
    REQUIRE_THAT(topo,
        Equals(std::vector<int>({1, 2, 3, 4, 5})) ||
        Equals(std::vector<int>({1, 3, 2, 4, 5})));
  }

  SECTION("gives a correct answer on a larger acyclic ") {
    dag::directed_hypernetwork<int> graph({
        {{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});

    auto topo = dag::topological_order(graph);
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

TEST_CASE("weakly connected components", "[dag::weakly_connected_components]") {
  SECTION("works for directed graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    auto comps = dag::weakly_connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }

  SECTION("works for directed hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{1, 2}, {3}}, {{3}, {4, 5}}, {{6}, {7, 8}}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8});

    auto comps = dag::weakly_connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }
}

TEST_CASE("is weakly connected?", "[dag::is_weakly_connected]") {
  dag::directed_network<int> graph({
      {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});
  REQUIRE_FALSE(dag::is_weakly_connected(graph));
  dag::directed_network<int> connected({
          {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(dag::is_weakly_connected(connected));
}

TEST_CASE("weakly connected component", "[dag::weakly_connected_component]") {
  SECTION("works for directed graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    for (auto i: weak1) {
      auto comp = dag::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = dag::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }

  SECTION("works for directed hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{1, 2}, {3}}, {{3}, {4, 5}}, {{6}, {7, 8}}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8});

    for (auto i: weak1) {
      auto comp = dag::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = dag::weakly_connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }
}

TEST_CASE("connected components", "[dag::connected_components]") {
  SECTION("works for undirected graph") {
    dag::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    auto comps = dag::connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }

  SECTION("works for undirected hypergraph") {
    dag::undirected_hypernetwork<int> graph({
        {1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8, 9});

    auto comps = dag::connected_components(graph);
    REQUIRE(comps.size() == 2);
    REQUIRE_THAT(std::vector<int>(comps[0].begin(), comps[0].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE_THAT(std::vector<int>(comps[1].begin(), comps[1].end()),
        UnorderedEquals(weak1) || UnorderedEquals(weak2));
    REQUIRE(comps[0] != comps[1]);
  }
}

TEST_CASE("is connected?", "[dag::is_connected]") {
  dag::undirected_network<int> graph({
      {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
      {8, 9}});
  REQUIRE_FALSE(dag::is_connected(graph));
  dag::undirected_network<int> connected({
      {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
  REQUIRE(dag::is_connected(connected));
}

TEST_CASE("is reachable (static networks)", "[dag::is_reachable]") {
  SECTION("works for undirected graph") {
    dag::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});
    REQUIRE(dag::is_reachable(graph, 1, 1));
    REQUIRE(dag::is_reachable(graph, 1, 6));
    REQUIRE(dag::is_reachable(graph, 6, 1));
    REQUIRE(dag::is_reachable(graph, 7, 9));

    REQUIRE_FALSE(dag::is_reachable(graph, 1, 7));
    REQUIRE_FALSE(dag::is_reachable(graph, 7, 1));
  }

  SECTION("works for undirected hypergraph") {
    dag::undirected_hypernetwork<int> graph({
        {1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    REQUIRE(dag::is_reachable(graph, 1, 1));
    REQUIRE(dag::is_reachable(graph, 1, 5));
    REQUIRE(dag::is_reachable(graph, 5, 1));
    REQUIRE(dag::is_reachable(graph, 7, 9));

    REQUIRE_FALSE(dag::is_reachable(graph, 1, 7));
    REQUIRE_FALSE(dag::is_reachable(graph, 7, 1));
  }

  SECTION("gives correct answer on a directed graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});

    REQUIRE(dag::is_reachable(graph, 2, 2));
    REQUIRE(dag::is_reachable(graph, 2, 5));
    REQUIRE(dag::is_reachable(graph, 2, 6));
    REQUIRE(dag::is_reachable(graph, 5, 6));

    REQUIRE_FALSE(dag::is_reachable(graph, 2, 1));
    REQUIRE_FALSE(dag::is_reachable(graph, 6, 2));
  }

  SECTION("gives correct answer on a directed hypergraph") {
    dag::directed_hypernetwork<int> graph({
        {{7, 1, 2}, {3}}, {{3}, {5}}, {{5}, {6, 1}},
        {{5}, {4}}, {{4}, {2, 3}}});

    REQUIRE(dag::is_reachable(graph, 2, 2));
    REQUIRE(dag::is_reachable(graph, 2, 5));
    REQUIRE(dag::is_reachable(graph, 2, 6));
    REQUIRE(dag::is_reachable(graph, 5, 6));

    REQUIRE_FALSE(dag::is_reachable(graph, 5, 7));
    REQUIRE_FALSE(dag::is_reachable(graph, 6, 2));
  }
}

TEST_CASE("connected component", "[dag::connected_component]") {
  SECTION("works for undirected graph") {
    dag::undirected_network<int> graph({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

    for (auto i: weak1) {
      auto comp = dag::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = dag::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }

  SECTION("works for undirected hypergraph") {
    dag::undirected_hypernetwork<int> graph({
        {1, 2, 3}, {3, 4, 5}, {6, 7, 8}, {8, 9}});

    std::vector<int> weak1({1, 2, 3, 4, 5}), weak2({6, 7, 8, 9});

    for (auto i: weak1) {
      auto comp = dag::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak1));
    }

    for (auto i: weak2) {
      auto comp = dag::connected_component(graph, i);
      REQUIRE_THAT(std::vector<int>(comp.begin(), comp.end()),
          UnorderedEquals(weak2));
    }
  }
}


TEST_CASE("cartesian product", "[dag::cartesian_product]") {
    dag::undirected_network<int> graph1({{1, 2}, {1, 3}});
    dag::undirected_network<int> graph2({{1, 2}, {2, 3}});

    dag::undirected_network<std::pair<int, int>> prod =
      dag::cartesian_product(graph1, graph2);

    REQUIRE(prod.vertices().size() ==
        graph1.vertices().size()*graph2.vertices().size());
    REQUIRE(prod.edges().size() ==
        graph1.vertices().size()*graph2.edges().size() +
        graph2.vertices().size()*graph1.edges().size());

    REQUIRE_THAT(prod.edges(),
        UnorderedEquals(
          std::vector<dag::undirected_edge<std::pair<int, int>>>{
            {{1, 1}, {2, 1}}, {{1, 1}, {3, 1}}, {{1, 1}, {1, 2}},
            {{1, 2}, {2, 2}}, {{1, 2}, {3, 2}}, {{1, 2}, {1, 3}},
            {{1, 3}, {2, 3}}, {{1, 3}, {3, 3}},
            {{2, 1}, {2, 2}},
            {{2, 2}, {2, 3}},
            {{3, 1}, {3, 2}},
            {{3, 2}, {3, 3}}}));
}

TEST_CASE("relabel nodes", "[dag::relabel_nodes]") {
  // let's make a graph with more to it than integral vertices
  dag::undirected_network<int> graph1({{1, 2}, {1, 3}});
  dag::undirected_network<int> graph2({{1, 2}, {1, 3}});
  dag::undirected_network<std::pair<int, int>> orig =
    dag::cartesian_product(graph1, graph2);

  dag::undirected_network<std::size_t> relabeled =
    dag::relabel_nodes<std::size_t>(orig);

  REQUIRE(orig.vertices().size() == relabeled.vertices().size());
  REQUIRE(orig.edges().size() == relabeled.edges().size());
}

TEST_CASE("is graphic", "[dag::is_graphic]") {
  REQUIRE(dag::is_graphic(std::vector<int>({})));
  REQUIRE(dag::is_graphic(std::vector<int>({5, 3, 3, 3, 2, 2})));
  REQUIRE(dag::is_graphic(std::vector<int>({3, 3, 2, 1, 1, 0})));
  REQUIRE(dag::is_graphic(std::vector<int>({5, 1, 1, 1, 1, 1})));
  REQUIRE(dag::is_graphic(std::vector<int>(5, 2)));
  REQUIRE(dag::is_graphic(std::vector<int>(5, 4)));

  REQUIRE_FALSE(dag::is_graphic(std::vector<int>({-1, 3, 3, 3, 2, 2})));
  REQUIRE_FALSE(dag::is_graphic(std::vector<int>({4, 3, 3, 2, 2, 1})));
  REQUIRE_FALSE(dag::is_graphic(std::vector<int>({4, 3, 2, 1})));
}

TEST_CASE("is digraphic", "[dag::is_digraphic]") {
  REQUIRE(dag::is_digraphic(std::vector<std::pair<int, int>>({})));
  REQUIRE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 0}, {2, 1}, {0, 1}, {1, 1}, {0, 1}})));
  REQUIRE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}})));
  REQUIRE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{2, 0}, {2, 1}, {0, 1}, {0, 1}, {0, 1}})));
  REQUIRE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{0, 1}, {1, 0}})));
  REQUIRE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 1}, {1, 1}})));

  REQUIRE_FALSE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, -1}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{-1, 0}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 0}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 3}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(dag::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 0}, {0, 1}, {1, 1}, {3, 1}, {0, 1}})));
}

TEST_CASE("vertex induced subgraph", "[dag::vertex_induced_subgraph]") {
  using EdgeType = dag::directed_temporal_hyperedge<int, int>;
  dag::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with components") {
    dag::component<typename EdgeType::VertexType> comp({1, 2, 3, 4, 9});
    auto res = dag::vertex_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = dag::vertex_induced_subgraph(network, {1, 2, 3, 4, 9});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedEquals(res.edges()));
  }
}

TEST_CASE("edge induced subgraph", "[dag::edge_induced_subgraph]") {
  using EdgeType = dag::directed_temporal_hyperedge<int, int>;
  dag::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with component") {
    dag::component<EdgeType> comp({
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});
    auto res = dag::edge_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = dag::edge_induced_subgraph(network, {
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedEquals(res.edges()));
  }
}

TEST_CASE("graph union", "[dag::graph_union]") {
  using EdgeType = dag::directed_temporal_hyperedge<int, int>;
  dag::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  dag::network<EdgeType> n2(
      {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = dag::graph_union(n1, n2);
  auto res2 = dag::graph_union(n1, n2);
  REQUIRE(res1.edges() == res2.edges());
  REQUIRE_THAT(res1.edges(), Contains(n1.edges()));
  REQUIRE_THAT(res1.edges(), Contains(n2.edges()));

  REQUIRE(res1.vertices() == res2.vertices());
  REQUIRE_THAT(res1.vertices(), Contains(n1.vertices()));
  REQUIRE_THAT(res1.vertices(), Contains(n2.vertices()));
}

TEST_CASE("with edges", "[dag::with_edges]") {
  using EdgeType = dag::directed_temporal_hyperedge<int, int>;
  dag::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = dag::with_edges(n1, {
      {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2},
          {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}));
}

TEST_CASE("with vertices", "[dag::with_vertices]") {
  using EdgeType = dag::directed_temporal_hyperedge<int, int>;
  dag::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = dag::with_vertices(n1, {3, 4, 5, 6});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 5, 6, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}

TEST_CASE("network density", "[dag::density]") {
    dag::undirected_network<int> ug({
        {1, 2}, {2, 3}, {3, 1}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8},
        {8, 9}});

    REQUIRE(dag::density(ug) == Approx(0.25));

    dag::directed_network<int> dg({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE(dag::density(dg) == Approx(0.2));
}
