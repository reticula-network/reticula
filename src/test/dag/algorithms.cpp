#include <vector>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include <dag/utils.hpp>
#include <dag/temporal_edges.hpp>
#include <dag/networks.hpp>
#include <dag/algorithms.hpp>
#include <dag/adjacency_prob.hpp>

TEST_CASE("event graph", "[dag::event_graph]") {
  SECTION("undirected temporal network") {
    using EdgeType = dag::undirected_temporal_edge<int, int>;
    dag::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
    SECTION("small delta-t") {
      dag::adjacency_prob::deterministic<EdgeType> prob(3);
      dag::directed_network<EdgeType> eg = event_graph(network, prob, 0ul);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
    SECTION("large delta-t") {
      dag::adjacency_prob::deterministic<EdgeType> prob(6);
      dag::directed_network<EdgeType> eg = event_graph(network, prob, 0ul);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 1}, {1, 2, 5}},
              {{1, 2, 1}, {2, 3, 6}},
              {{2, 1, 2}, {1, 2, 5}},
              {{2, 1, 2}, {2, 3, 6}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
  }

  SECTION("directed temporal network") {
    using EdgeType = dag::directed_temporal_edge<int, int>;
    dag::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
    SECTION("small delta-t") {
      dag::adjacency_prob::deterministic<EdgeType> prob(3);
      dag::directed_network<EdgeType> eg = event_graph(network, prob, 0ul);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
    SECTION("large delta-t") {
      dag::adjacency_prob::deterministic<EdgeType> prob(6);
      dag::directed_network<EdgeType> eg = event_graph(network, prob, 0ul);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 1}, {2, 3, 6}},
              {{2, 1, 2}, {1, 2, 5}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
    dag::network<EdgeType> network(
        {{1, 2, 1, 4}, {2, 1, 2, 1}, {1, 2, 5, 0}, {2, 3, 6, 1}, {3, 4, 8, 1}});
    SECTION("small delta-t") {
      dag::adjacency_prob::deterministic<EdgeType> prob(3);
      dag::directed_network<EdgeType> eg = event_graph(network, prob, 0ul);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1, 4}, {2, 3, 6, 1}},
              {{2, 1, 2, 1}, {1, 2, 5, 0}},
              {{1, 2, 5, 0}, {2, 3, 6, 1}},
              {{2, 3, 6, 1}, {3, 4, 8, 1}}
            })));
    }
    SECTION("large delta-t") {
      dag::adjacency_prob::deterministic<EdgeType> prob(6);
      dag::directed_network<EdgeType> eg = event_graph(network, prob, 0ul);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1, 4}, {2, 3, 6, 1}},
              {{2, 1, 2, 1}, {1, 2, 5, 0}},
              {{1, 2, 5, 0}, {2, 3, 6, 1}},
              {{2, 3, 6, 1}, {3, 4, 8, 1}}
            })));
    }
  }
}

TEST_CASE("out component", "[dag::out_component]") {
  SECTION("throws on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE_THROWS_AS(dag::out_component(graph, 1),
        dag::utils::not_acyclic_error);
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    REQUIRE_THAT(dag::out_component(graph, 2),
      UnorderedEquals(std::vector<int>({2, 3, 4, 5, 6})));
    REQUIRE_THAT(dag::out_component(graph, 5),
      UnorderedEquals(std::vector<int>({5, 4, 6})));
  }
}

TEST_CASE("in component", "[dag::in_component]") {
  SECTION("throws on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE_THROWS_AS(dag::in_component(graph, 1),
        dag::utils::not_acyclic_error);
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    REQUIRE_THAT(dag::in_component(graph, 2),
      UnorderedEquals(std::vector<int>({2, 1})));
    REQUIRE_THAT(dag::in_component(graph, 5),
      UnorderedEquals(std::vector<int>({1, 2, 3, 5})));
  }
}

TEST_CASE("topological ordering", "[dag::topological_order]") {
  SECTION("throws on a cyclic graph") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
    REQUIRE_THROWS_AS(dag::in_component(graph, 1),
        dag::utils::not_acyclic_error);
  }

  SECTION("gives correct answer on acyclic graphs") {
    dag::directed_network<int> graph({
        {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}});
    auto topo = dag::topological_order(graph);
    REQUIRE_THAT(topo, Equals(std::vector<int>({1, 2, 3, 5, 4, 6})) ||
                        Equals(std::vector<int>({1, 2, 3, 5, 6, 4})));
  }
}

TEST_CASE("weakly connected components", "[dag::weakly_connected_components]") {
  dag::directed_network<int> graph({
      {1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}, {7, 8}, {8, 9}});

  std::vector<int> weak1({1, 2, 3, 4, 5, 6}), weak2({7, 8, 9});

  auto comps = dag::weakly_connected_components(graph);
  REQUIRE(comps.size() == 2);
  REQUIRE_THAT(comps[0], UnorderedEquals(weak1) || UnorderedEquals(weak2));
  REQUIRE_THAT(comps[1], UnorderedEquals(weak1) || UnorderedEquals(weak2));
  REQUIRE_THAT(comps[0], !UnorderedEquals(comps[1]));
}

#include <iostream>
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

TEST_CASE("is_graphic", "[dag::is_graphic]") {
  REQUIRE(dag::is_graphic(std::vector<int>({})));
  REQUIRE(dag::is_graphic(std::vector<int>({5, 3, 3, 3, 2, 2})));
  REQUIRE(dag::is_graphic(std::vector<int>({3, 3, 2, 1, 1, 0})));
  REQUIRE(dag::is_graphic(std::vector<int>({5, 1, 1, 1, 1, 1})));
  REQUIRE(dag::is_graphic(std::vector<int>(5, 2)));
  REQUIRE(dag::is_graphic(std::vector<int>(5, 4)));

  REQUIRE_FALSE(dag::is_graphic(std::vector<int>({4, 3, 3, 2, 2, 1})));
  REQUIRE_FALSE(dag::is_graphic(std::vector<int>({4, 3, 2, 1})));
}
