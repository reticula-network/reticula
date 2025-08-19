#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/algorithms/dag.hpp>
#include <reticula/networks.hpp>

TEST_CASE("is acyclic", "[reticula::is_acyclic]") {
  REQUIRE(
    reticula::is_acyclic(
      reticula::directed_network({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}})));

  REQUIRE(
    reticula::is_acyclic(
      reticula::directed_hypernetwork(
        {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}})));

  REQUIRE_FALSE(
    reticula::is_acyclic(
      reticula::directed_network(
        {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}})));

  REQUIRE_FALSE(
    reticula::is_acyclic(
      reticula::directed_hypernetwork(
        {{{7, 1, 2}, {3}},
         {{3}, {5}},
         {{5}, {6, 1}},
         {{5}, {4}},
         {{4}, {2, 3}}})));
}

TEST_CASE("try topological ordering", "[reticula::try_topological_order]") {
  SECTION("returns nullopt on a cyclic graph") {
    REQUIRE_FALSE(
      reticula::try_topological_order(
        reticula::directed_network(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}})));

    REQUIRE_FALSE(
      reticula::try_topological_order(
        reticula::directed_network({{0, 1}, {1, 2}, {2, 1}}, {0, 1, 2, 3})));

    // self-loop
    REQUIRE_FALSE(
      reticula::try_topological_order(
        reticula::directed_network({{0, 0}, {1, 2}}, {0, 1, 2})));

    REQUIRE_FALSE(
      reticula::try_topological_order(
        reticula::directed_hypernetwork(
          {{{7, 1, 2}, {3}},
           {{3}, {5}},
           {{5}, {6, 1}},
           {{5}, {4}},
           {{4}, {2, 3}}})));
  }

  SECTION("returns a correct answer on an acyclic graph") {
    auto maybe_topo = reticula::try_topological_order(
      reticula::directed_network({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(
      maybe_topo.value(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 5, 4, 6})) ||
        RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 5, 6, 4})));
  }

  SECTION("returns a correct answer on an acyclic graph with isolated node") {
    auto maybe_topo = reticula::try_topological_order(
      reticula::directed_network({{1, 2}, {2, 3}}, {0, 1, 2, 3}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(
      maybe_topo.value(),
      RangeEquals(std::vector<reticula::VertexType>({0, 1, 2, 3})) ||
        RangeEquals(std::vector<reticula::VertexType>({1, 0, 2, 3})) ||
        RangeEquals(std::vector<reticula::VertexType>({1, 2, 0, 3})) ||
        RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 0})));
  }

  SECTION("returns a correct answer on an acyclic hypergraph") {
    auto maybe_topo = reticula::try_topological_order(
      reticula::directed_hypernetwork(
        {{{1}, {2, 3}}, {{2}, {4}}, {{3}, {4}}, {{4}, {5}}}));
    REQUIRE(maybe_topo);
    REQUIRE_THAT(
      maybe_topo.value(),
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4, 5})) ||
        RangeEquals(std::vector<reticula::VertexType>({1, 3, 2, 4, 5})));
  }

  SECTION("gives a correct answer on a larger acyclic ") {
    reticula::directed_hypernetwork graph(
      {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});

    auto maybe_topo = reticula::try_topological_order(graph);
    REQUIRE(maybe_topo);

    std::unordered_map<reticula::VertexType, std::size_t> topo_pos;
    for (std::size_t pos = 0; auto i : *maybe_topo)
      topo_pos[i] = pos++;

    REQUIRE(std::ranges::all_of(graph.edges(), [&topo_pos](const auto e) {
      for (auto in : e.mutator_verts())
        for (auto out : e.mutated_verts())
          if (topo_pos[in] >= topo_pos[out])
            return false;
      return true;
    }));
  }
}

TEST_CASE("topological ordering", "[reticula::topological_order]") {
  SECTION("throws on a cyclic graph") {
    reticula::directed_network graph(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE_THROWS_AS(
      reticula::topological_order(graph), reticula::not_acyclic_error);
  }

  SECTION("gives correct answer on acyclic graphs") {
    reticula::directed_network graph({{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto topo = reticula::topological_order(graph);
    REQUIRE_THAT(
      topo,
      RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 5, 4, 6})) ||
        RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 5, 6, 4})));
  }

  SECTION("returns a correct answer on an acyclic hypergraph") {
    auto topo = reticula::topological_order(
      reticula::directed_hypernetwork(
        {{{1}, {2, 3}}, {{2}, {4}}, {{3}, {4}}, {{4}, {5}}}));
    REQUIRE_THAT(
      topo, RangeEquals(std::vector<reticula::VertexType>({1, 2, 3, 4, 5})) ||
              RangeEquals(std::vector<reticula::VertexType>({1, 3, 2, 4, 5})));
  }

  SECTION("gives a correct answer on a larger acyclic ") {
    reticula::directed_hypernetwork graph(
      {{{1}, {2}}, {{7, 2}, {3}}, {{3}, {5, 6}}, {{5}, {6}}, {{5}, {4}}});

    auto topo = reticula::topological_order(graph);
    std::unordered_map<reticula::VertexType, std::size_t> topo_pos;
    for (std::size_t pos = 0; auto i : topo)
      topo_pos[i] = pos++;

    REQUIRE(std::ranges::all_of(graph.edges(), [&topo_pos](const auto e) {
      for (auto in : e.mutator_verts())
        for (auto out : e.mutated_verts())
          if (topo_pos[in] >= topo_pos[out])
            return false;
      return true;
    }));
  }
}
