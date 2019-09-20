#include <vector>

#include "catch.hpp"
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include "../../../include/dag/utils.hpp"
#include "../../../include/dag/temporal_edges.hpp"
#include "../../../include/dag/networks.hpp"
#include "../../../include/dag/algorithms.hpp"
#include "../../../include/dag/adjacency_prob.hpp"

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
