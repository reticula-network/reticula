#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include <reticula/implicit_event_graph_components.hpp>

TEST_CASE("in-/out-/weakly connected component",
    "[reticula::implicit_event_graph][reticula::in_component]"
    "[reticula::out_component][reticula::weakly_connected_component]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge<int, int>;
    using ProbType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
    reticula::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    ProbType prob(2);
    reticula::implicit_event_graph eg(network, prob);

    SECTION("in-component") {
      auto c = reticula::in_component(eg, {2, 3, 6});
      REQUIRE(c == reticula::component<EdgeType>({{2, 3, 6}, {1, 2, 5}}));
    }

    SECTION("out-component") {
      auto c = reticula::out_component(eg, {2, 3, 6});
      REQUIRE(c == reticula::component<EdgeType>({{2, 3, 6}, {3, 4, 8}}));
    }

    SECTION("weakly connected component") {
      auto c = reticula::weakly_connected_component(eg, {2, 3, 6});
      reticula::component<EdgeType> w({{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
      REQUIRE(c == w);
    }
  }

  SECTION("directed temporal network") {
    using EdgeType = reticula::directed_temporal_edge<int, int>;
    using ProbType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;

    reticula::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    ProbType prob(2);
    reticula::implicit_event_graph eg(network, prob);

    SECTION("in-component") {
      auto c = reticula::in_component(eg, {2, 3, 6});
      REQUIRE(c == reticula::component<EdgeType>({{2, 3, 6}, {1, 2, 5}}));
    }

    SECTION("out-component") {
      auto c = reticula::out_component(eg, {2, 3, 6});
      REQUIRE(c == reticula::component<EdgeType>({{2, 3, 6}, {3, 4, 8}}));
    }

    SECTION("weakly connected component") {
      auto c = reticula::weakly_connected_component(eg, {2, 3, 6});
      reticula::component<EdgeType> w({{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
      REQUIRE(c == w);
    }
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
    using ProbType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;

    reticula::network<EdgeType> network(
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
          {5, 6, 1, 3}});
    ProbType prob(2);
    reticula::implicit_event_graph eg(network, prob);

    SECTION("in-component") {
      auto c = reticula::in_component(eg, {2, 3, 6, 7});
      REQUIRE(c == reticula::component<EdgeType>(
                  {{2, 1, 2, 3}, {2, 3, 6, 7}, {1, 2, 5, 5}, {1, 2, 1, 5}}));
    }

    SECTION("out-component") {
      auto c = reticula::out_component(eg, {2, 3, 6, 7});
      REQUIRE(c == reticula::component<EdgeType>({{2, 3, 6, 7}, {3, 4, 8, 9}}));
    }

    SECTION("weakly connected component") {
      auto c = reticula::weakly_connected_component(eg, {2, 3, 6, 7});
      reticula::component<EdgeType> w({{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5},
          {2, 3, 6, 7}, {3, 4, 8, 9}});
      REQUIRE(c == w);
    }
  }
}

TEST_CASE("in-/out-/weakly connected components",
    "[reticula::implicit_event_graph][reticula::in_components]"
    "[reticula::out_components][reticula::weakly_connected_components]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge<int, int>;
    using ProbType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
    reticula::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    ProbType prob(2);
    reticula::implicit_event_graph eg(network, prob);

    SECTION("in-components") {
      std::vector<std::pair<EdgeType, reticula::component<EdgeType>>> true_ic({
        {{2, 1, 2}, {{2, 1, 2}, {1, 2, 1}}},
        {{1, 2, 1}, {{1, 2, 1}}},
        {{1, 2, 5}, {{1, 2, 5}}},
        {{2, 3, 6}, {{1, 2, 5}, {2, 3, 6}}},
        {{3, 4, 8}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      REQUIRE_THAT(reticula::in_components(eg), UnorderedEquals(true_ic));
    }

    SECTION("out-components") {
      std::vector<std::pair<EdgeType, reticula::component<EdgeType>>> true_oc({
        {{1, 2, 1}, {{1, 2, 1}, {2, 1, 2}}},
        {{2, 1, 2}, {{2, 1, 2}}},
        {{1, 2, 5}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{2, 3, 6}, {{2, 3, 6}, {3, 4, 8}}},
        {{3, 4, 8}, {{3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      REQUIRE_THAT(reticula::out_components(eg), UnorderedEquals(true_oc));
    }

    SECTION("weakly connected components") {
      std::vector<reticula::component<EdgeType>> ws({
        {{1, 2, 1}, {2, 1, 2}},
        {{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}},
        {{5, 6, 1}}});
      REQUIRE_THAT(reticula::weakly_connected_components(eg), UnorderedEquals(ws));
    }
  }

  SECTION("directed temporal network") {
    using EdgeType = reticula::directed_temporal_edge<int, int>;
    using ProbType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;

    reticula::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    ProbType prob(2);
    reticula::implicit_event_graph eg(network, prob);

    SECTION("in-components") {
      std::vector<std::pair<EdgeType, reticula::component<EdgeType>>> true_ic({
        {{2, 1, 2}, {{2, 1, 2}, {1, 2, 1}}},
        {{1, 2, 1}, {{1, 2, 1}}},
        {{1, 2, 5}, {{1, 2, 5}}},
        {{2, 3, 6}, {{1, 2, 5}, {2, 3, 6}}},
        {{3, 4, 8}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      REQUIRE_THAT(reticula::in_components(eg), UnorderedEquals(true_ic));
    }

    SECTION("out-components") {
      std::vector<std::pair<EdgeType, reticula::component<EdgeType>>> true_oc({
        {{1, 2, 1}, {{1, 2, 1}, {2, 1, 2}}},
        {{2, 1, 2}, {{2, 1, 2}}},
        {{1, 2, 5}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
        {{2, 3, 6}, {{2, 3, 6}, {3, 4, 8}}},
        {{3, 4, 8}, {{3, 4, 8}}},
        {{5, 6, 1}, {{5, 6, 1}}},
      });
      REQUIRE_THAT(reticula::out_components(eg), UnorderedEquals(true_oc));
    }

    SECTION("weakly connected components") {
      std::vector<reticula::component<EdgeType>> ws({
        {{1, 2, 1}, {2, 1, 2}},
        {{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}},
        {{5, 6, 1}}});
      REQUIRE_THAT(reticula::weakly_connected_components(eg), UnorderedEquals(ws));
    }
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
    using ProbType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;

    reticula::network<EdgeType> network(
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
          {5, 6, 1, 3}});
    ProbType prob(2);
    reticula::implicit_event_graph eg(network, prob);

    SECTION("in-components") {
      std::vector<std::pair<EdgeType, reticula::component<EdgeType>>> true_ic({
        {{1, 2, 1, 5}, {{1, 2, 1, 5}}},
        {{2, 1, 2, 3}, {{2, 1, 2, 3}}},
        {{1, 2, 5, 5}, {{1, 2, 5, 5}, {2, 1, 2, 3}}},
        {{2, 3, 6, 7},
          {{2, 3, 6, 7}, {1, 2, 5, 5}, {2, 1, 2, 3}, {1, 2, 1, 5}}},
        {{3, 4, 8, 9},
          {{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7},
            {1, 2, 1, 5}, {3, 4, 8, 9}}},
        {{5, 6, 1, 3}, {{5, 6, 1, 3}}},
      });
      REQUIRE_THAT(reticula::in_components(eg), UnorderedEquals(true_ic));
    }

    SECTION("out-components") {
      std::vector<std::pair<EdgeType, reticula::component<EdgeType>>> true_oc({
        {{1, 2, 1, 5}, {{1, 2, 1, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}},
        {{2, 1, 2, 3},
          {{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}},
        {{1, 2, 5, 5}, {{1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}},
        {{2, 3, 6, 7}, {{2, 3, 6, 7}, {3, 4, 8, 9}}},
        {{3, 4, 8, 9}, {{3, 4, 8, 9}}},
        {{5, 6, 1, 3}, {{5, 6, 1, 3}}},
      });
      REQUIRE_THAT(reticula::out_components(eg), UnorderedEquals(true_oc));
    }

    SECTION("weakly connected components") {
      std::vector<reticula::component<EdgeType>> ws({
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7},
         {3, 4, 8, 9}},
        {{5, 6, 1, 3}}});
      REQUIRE_THAT(reticula::weakly_connected_components(eg), UnorderedEquals(ws));
    }
  }
}
