#include <vector>
#include <set>
#include <unordered_set>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include <dag/temporal_components.hpp>
#include <dag/temporal_edges.hpp>
#include <dag/estimators.hpp>

TEST_CASE("temporal components", "[dag::temporal_component]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  dag::temporal_component<EdgeType,
    dag::unordered_set_cardinality_estimator> comp(0ul);

  EdgeType a(1, 2, 1, 3), b(2, 1, 2, 1), c(2, 3, 6, 1);

  comp.insert(a, a.mutated_verts());
  comp.insert(b, b.mutated_verts());

  SECTION("correct basic properties") {
    REQUIRE(comp.node_set().underlying_estimator() ==
        std::unordered_set<int, dag::hash<int>>({1, 2}));
    REQUIRE(comp.edge_set().underlying_estimator() ==
        std::unordered_set<EdgeType, dag::hash<EdgeType>>({a, b}));
    REQUIRE(comp.cause_lifetime() == std::make_pair(1, 2));
    REQUIRE(comp.effect_lifetime() == std::make_pair(3, 4));
  }

  SECTION("insertion") {
    comp.insert(c, c.mutated_verts());
    REQUIRE(comp.node_set().underlying_estimator() ==
        std::unordered_set<int, dag::hash<int>>({1, 2, 3}));
    REQUIRE(comp.edge_set().underlying_estimator() ==
        std::unordered_set<EdgeType, dag::hash<EdgeType>>({a, b, c}));
    REQUIRE(comp.cause_lifetime() == std::make_pair(1, 6));
    REQUIRE(comp.effect_lifetime() == std::make_pair(3, 7));
  }

  SECTION("merging") {
    dag::temporal_component<EdgeType,
      dag::unordered_set_cardinality_estimator> comp2(0ul);
    comp2.insert(c, c.mutated_verts());

    comp.merge(comp2);
    REQUIRE(comp.node_set().underlying_estimator() ==
        std::unordered_set<int, dag::hash<int>>({1, 2, 3}));
    REQUIRE(comp.edge_set().underlying_estimator() ==
        std::unordered_set<EdgeType, dag::hash<EdgeType>>({a, b, c}));
    REQUIRE(comp.cause_lifetime() == std::make_pair(1, 6));
    REQUIRE(comp.effect_lifetime() == std::make_pair(3, 7));
  }
}


#include <dag/implicit_event_graph.hpp>

TEST_CASE("in- and out- components for implicit event graph",
    "[dag::out_component][dag::in_component]"
    "[dag::out_components][dag::in_components]") {
  SECTION("undirected temporal network") {
    using EdgeType = dag::undirected_temporal_edge<int, int>;
    using ProbType = dag::adjacency_prob::deterministic<EdgeType>;
    using CompType = dag::temporal_component<EdgeType,
          dag::unordered_set_cardinality_estimator>;

    dag::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    ProbType prob(3);
    dag::implicit_event_graph<EdgeType, ProbType> eg(network, prob, 0ul);

    SECTION("out component") {
      CompType c = dag::out_component<EdgeType, ProbType,
               dag::unordered_set_cardinality_estimator>(
          eg, {2, 3, 6}, 0ul);
      REQUIRE(c.edge_set().underlying_estimator() ==
                std::unordered_set<EdgeType, dag::hash<EdgeType>>(
                  {{2, 3, 6}, {3, 4, 8}}));
      REQUIRE(c.node_set().underlying_estimator() ==
                std::unordered_set<int, dag::hash<int>>({2, 3, 4}));
    }

    SECTION("in component") {
      CompType c = dag::in_component<EdgeType, ProbType,
               dag::unordered_set_cardinality_estimator>(
          eg, {2, 3, 6}, 0ul);
      REQUIRE(c.edge_set().underlying_estimator() ==
                std::unordered_set<EdgeType, dag::hash<EdgeType>>(
                  {{2, 3, 6}, {1, 2, 5}}));
      REQUIRE(c.node_set().underlying_estimator() ==
                std::unordered_set<int, dag::hash<int>>({2, 3, 1}));
    }

    SECTION("all out components") {
      std::unordered_map<EdgeType,
          std::unordered_set<EdgeType, dag::hash<EdgeType>>> true_oc({
        {{1, 2, 1}, {{1, 2, 1}, {2, 1, 2}}},
        {{2, 1, 2}, {{2, 1, 2}}},
        {{1, 2, 5}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{2, 3, 6}, {{2, 3, 6}, {3, 4, 8}}},
        {{3, 4, 8}, {{3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      SECTION("root out components") {
        auto out_comps = dag::out_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul, true);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: out_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_oc[e]);
        }
        REQUIRE_THAT(keys,
            UnorderedEquals(
              std::vector<EdgeType>({{1, 2, 1}, {1, 2, 5}, {5, 6, 1}})));
      }

      SECTION("root and non-root out components") {
        auto out_comps = dag::out_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: out_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_oc[e]);
        }
        REQUIRE_THAT(keys, UnorderedEquals(network.edges_cause()));
      }
    }

    SECTION("all in components") {
      std::unordered_map<EdgeType,
          std::unordered_set<EdgeType, dag::hash<EdgeType>>> true_ic({
        {{2, 1, 2}, {{2, 1, 2}, {1, 2, 1}}},
        {{1, 2, 1}, {{1, 2, 1}}},
        {{1, 2, 5}, {{1, 2, 5}}},
        {{2, 3, 6}, {{1, 2, 5}, {2, 3, 6}}},
        {{3, 4, 8}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      SECTION("root in components") {
        auto in_comps = dag::in_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul, true);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: in_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_ic[e]);
        }
        REQUIRE_THAT(keys,
            UnorderedEquals(
              std::vector<EdgeType>({{2, 1, 2}, {3, 4, 8}, {5, 6, 1}})));
      }

      SECTION("root and non-root in components") {
        auto in_comps = dag::in_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: in_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_ic[e]);
        }
        REQUIRE_THAT(keys, UnorderedEquals(network.edges_cause()));
      }
    }

    SECTION("weakly connected components") {
      std::set<EdgeType>
        w1({{1, 2, 1}, {2, 1, 2}}),
        w2({{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}}),
        w3({{5, 6, 1}});

      SECTION("with singletons") {
        auto weakly_comps = dag::weakly_connected_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator>(eg, 0ul);

        std::set<std::set<EdgeType>> results;
        for (auto&& c: weakly_comps)
          results.emplace(
              c.edge_set().underlying_estimator().begin(),
              c.edge_set().underlying_estimator().end());

        REQUIRE(results ==
            std::set<std::set<EdgeType>>({w1, w2, w3}));
      }

      SECTION("without singletons") {
        auto weakly_comps = dag::weakly_connected_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator>(eg, 0ul, false);

        std::set<std::set<EdgeType>> results;
        for (auto&& c: weakly_comps)
          results.emplace(
              c.edge_set().underlying_estimator().begin(),
              c.edge_set().underlying_estimator().end());

        REQUIRE(results ==
            std::set<std::set<EdgeType>>({w1, w2}));
      }
    }
  }

  SECTION("directed temporal network") {
    using EdgeType = dag::directed_temporal_edge<int, int>;
    using ProbType = dag::adjacency_prob::deterministic<EdgeType>;
    using CompType = dag::temporal_component<EdgeType,
          dag::unordered_set_cardinality_estimator>;

    dag::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    ProbType prob(3);
    dag::implicit_event_graph<EdgeType, ProbType> eg(network, prob, 0ul);

    SECTION("out component") {
      CompType c = dag::out_component<EdgeType, ProbType,
               dag::unordered_set_cardinality_estimator>(
          eg, {2, 3, 6}, 0ul);
      REQUIRE(c.edge_set().underlying_estimator() ==
                std::unordered_set<EdgeType, dag::hash<EdgeType>>(
                  {{2, 3, 6}, {3, 4, 8}}));
      REQUIRE(c.node_set().underlying_estimator() ==
                std::unordered_set<int, dag::hash<int>>({3, 4}));
    }

    SECTION("in component") {
      CompType c = dag::in_component<EdgeType, ProbType,
               dag::unordered_set_cardinality_estimator>(
          eg, {2, 3, 6}, 0ul);
      REQUIRE(c.edge_set().underlying_estimator() ==
                std::unordered_set<EdgeType, dag::hash<EdgeType>>(
                  {{2, 3, 6}, {1, 2, 5}}));
      REQUIRE(c.node_set().underlying_estimator() ==
                std::unordered_set<int, dag::hash<int>>({2, 1}));
    }

    SECTION("all out components") {
      std::unordered_map<EdgeType,
          std::unordered_set<EdgeType, dag::hash<EdgeType>>> true_oc({
        {{1, 2, 1}, {{1, 2, 1}, {2, 1, 2}}},
        {{2, 1, 2}, {{2, 1, 2}}},
        {{1, 2, 5}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{2, 3, 6}, {{2, 3, 6}, {3, 4, 8}}},
        {{3, 4, 8}, {{3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      SECTION("root out components") {
        auto out_comps = dag::out_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul, true);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: out_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_oc[e]);
        }
        REQUIRE_THAT(keys,
            UnorderedEquals(
              std::vector<EdgeType>({{1, 2, 1}, {1, 2, 5}, {5, 6, 1}})));
      }

      SECTION("root and non-root out components") {
        auto out_comps = dag::out_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: out_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_oc[e]);
        }
        REQUIRE_THAT(keys, UnorderedEquals(network.edges_cause()));
      }
    }

    SECTION("all in components") {
      std::unordered_map<EdgeType,
          std::unordered_set<EdgeType, dag::hash<EdgeType>>> true_ic({
        {{2, 1, 2}, {{2, 1, 2}, {1, 2, 1}}},
        {{1, 2, 1}, {{1, 2, 1}}},
        {{1, 2, 5}, {{1, 2, 5}}},
        {{2, 3, 6}, {{1, 2, 5}, {2, 3, 6}}},
        {{3, 4, 8}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      SECTION("root in components") {
        auto in_comps = dag::in_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul, true);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: in_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_ic[e]);
        }
        REQUIRE_THAT(keys,
            UnorderedEquals(
              std::vector<EdgeType>({{2, 1, 2}, {3, 4, 8}, {5, 6, 1}})));
      }

      SECTION("root and non-root in components") {
        auto in_comps = dag::in_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: in_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_ic[e]);
        }
        REQUIRE_THAT(keys, UnorderedEquals(network.edges_cause()));
      }
    }

    SECTION("weakly connected components") {
      std::set<EdgeType>
        w1({{1, 2, 1}, {2, 1, 2}}),
        w2({{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}}),
        w3({{5, 6, 1}});

      SECTION("with singletons") {
        auto weakly_comps = dag::weakly_connected_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator>(eg, 0ul);

        std::set<std::set<EdgeType>> results;
        for (auto&& c: weakly_comps)
          results.emplace(
              c.edge_set().underlying_estimator().begin(),
              c.edge_set().underlying_estimator().end());

        REQUIRE(results ==
            std::set<std::set<EdgeType>>({w1, w2, w3}));
      }

      SECTION("without singletons") {
        auto weakly_comps = dag::weakly_connected_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator>(eg, 0ul, false);

        std::set<std::set<EdgeType>> results;
        for (auto&& c: weakly_comps)
          results.emplace(
              c.edge_set().underlying_estimator().begin(),
              c.edge_set().underlying_estimator().end());

        REQUIRE(results ==
            std::set<std::set<EdgeType>>({w1, w2}));
      }
    }
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
    using ProbType = dag::adjacency_prob::deterministic<EdgeType>;
    using CompType = dag::temporal_component<
      EdgeType, dag::unordered_set_cardinality_estimator>;

    dag::network<EdgeType> network(
        {{1, 2, 1, 4}, {2, 1, 2, 1}, {1, 2, 5, 0}, {2, 3, 6, 1}, {3, 4, 8, 1},
          {5, 6, 1, 2}});
    ProbType prob(3);
    dag::implicit_event_graph<EdgeType, ProbType> eg(network, prob, 0ul);

    SECTION("out component") {
      CompType c = dag::out_component<EdgeType, ProbType,
               dag::unordered_set_cardinality_estimator>(
          eg, {2, 3, 6, 1}, 0ul);
      REQUIRE(c.edge_set().underlying_estimator() ==
                std::unordered_set<EdgeType, dag::hash<EdgeType>>(
                  {{2, 3, 6, 1}, {3, 4, 8, 1}}));
      REQUIRE(c.node_set().underlying_estimator() ==
                std::unordered_set<int, dag::hash<int>>({3, 4}));
    }

    SECTION("in component") {
      CompType c = dag::in_component<EdgeType, ProbType,
               dag::unordered_set_cardinality_estimator>(
          eg, {2, 3, 6, 1}, 0ul);
      REQUIRE(c.edge_set().underlying_estimator() ==
                std::unordered_set<EdgeType, dag::hash<EdgeType>>(
                  {{2, 1, 2, 1}, {2, 3, 6, 1}, {1, 2, 5, 0}, {1, 2, 1, 4}}));
      REQUIRE(c.node_set().underlying_estimator() ==
                std::unordered_set<int, dag::hash<int>>({2, 1}));
    }

    SECTION("all out components") {
      std::unordered_map<EdgeType,
        std::unordered_set<EdgeType, dag::hash<EdgeType>>> true_oc({
        {{1, 2, 1, 4}, {{1, 2, 1, 4}, {2, 3, 6, 1}, {3, 4, 8, 1}}},
        {{2, 1, 2, 1},
          {{2, 1, 2, 1}, {1, 2, 5, 0}, {2, 3, 6, 1}, {3, 4, 8, 1}}},
        {{1, 2, 5, 0}, {{1, 2, 5, 0}, {2, 3, 6, 1}, {3, 4, 8, 1}}},
        {{2, 3, 6, 1}, {{2, 3, 6, 1}, {3, 4, 8, 1}}},
        {{3, 4, 8, 1}, {{3, 4, 8, 1}}},
        {{5, 6, 1, 2}, {{5, 6, 1, 2}}},
      });
      SECTION("root out components") {
        auto out_comps = dag::out_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul, true);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: out_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_oc[e]);
        }
        REQUIRE_THAT(keys,
            UnorderedEquals(
              std::vector<EdgeType>(
                {{1, 2, 1, 4}, {2, 1, 2, 1}, {5, 6, 1, 2}})));
      }

      SECTION("root and non-root out components") {
        auto out_comps = dag::out_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: out_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_oc[e]);
        }
        REQUIRE_THAT(keys, UnorderedEquals(network.edges_cause()));
      }
    }

    SECTION("all in components") {
      std::unordered_map<EdgeType,
        std::unordered_set<EdgeType, dag::hash<EdgeType>>> true_ic({
        {{1, 2, 1, 4}, {{1, 2, 1, 4}}},
        {{2, 1, 2, 1}, {{2, 1, 2, 1}}},
        {{1, 2, 5, 0}, {{1, 2, 5, 0}, {2, 1, 2, 1}}},
        {{2, 3, 6, 1},
          {{2, 3, 6, 1}, {1, 2, 5, 0}, {2, 1, 2, 1}, {1, 2, 1, 4}}},
        {{3, 4, 8, 1},
          {{2, 1, 2, 1}, {1, 2, 5, 0}, {2, 3, 6, 1},
            {1, 2, 1, 4}, {3, 4, 8, 1}}},
        {{5, 6, 1, 2}, {{5, 6, 1, 2}}},
      });
      SECTION("root in components") {
        auto in_comps = dag::in_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul, true);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: in_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_ic[e]);
        }
        REQUIRE_THAT(keys,
            UnorderedEquals(
              std::vector<EdgeType>({{3, 4, 8, 1}, {5, 6, 1, 2}})));
      }

      SECTION("root and non-root in components") {
        auto in_comps = dag::in_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator,
          dag::unordered_set_cardinality_estimator>(
              eg, 0ul);
        std::vector<EdgeType> keys;
        for (auto&& [e, c]: in_comps) {
          keys.push_back(e);
          REQUIRE(c.edge_set().underlying_estimator() == true_ic[e]);
        }
        REQUIRE_THAT(keys, UnorderedEquals(network.edges_cause()));
      }
    }

    SECTION("weakly connected components") {
      std::set<EdgeType>
        w1({{1, 2, 1, 4}, {2, 1, 2, 1}, {1, 2, 5, 0}, {2, 3, 6, 1},
            {3, 4, 8, 1}}),
        w2({{5, 6, 1, 2}});

      SECTION("with singletons") {
        auto weakly_comps = dag::weakly_connected_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator>(eg, 0ul);

        std::set<std::set<EdgeType>> results;
        for (auto&& c: weakly_comps)
          results.emplace(
              c.edge_set().underlying_estimator().begin(),
              c.edge_set().underlying_estimator().end());

        REQUIRE(results == std::set<std::set<EdgeType>>({w1, w2}));
      }

      SECTION("without singletons") {
        auto weakly_comps = dag::weakly_connected_components<
          EdgeType, ProbType,
          dag::unordered_set_cardinality_estimator>(eg, 0ul, false);

        std::set<std::set<EdgeType>> results;
        for (auto&& c: weakly_comps)
          results.emplace(
              c.edge_set().underlying_estimator().begin(),
              c.edge_set().underlying_estimator().end());

        REQUIRE(results == std::set<std::set<EdgeType>>({w1}));
      }
    }
  }
}
