#include <vector>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;

#include <dag/utils.hpp>
#include <dag/temporal_edges.hpp>
#include <dag/networks.hpp>
#include <dag/temporal_algorithms.hpp>
#include <dag/temporal_adjacency.hpp>

TEST_CASE("time_window",
    "[dag::time_window][dag::cause_time_window][dag::effect_time_window]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}});
  REQUIRE(dag::cause_time_window(network) == std::make_pair(1, 8));
  REQUIRE(dag::effect_time_window(network) == std::make_pair(3, 9));
  REQUIRE(dag::time_window(network) == std::make_pair(1, 9));
}

TEST_CASE("event graph", "[dag::event_graph]") {
  SECTION("undirected temporal network") {
    using EdgeType = dag::undirected_temporal_edge<int, int>;
    dag::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
    SECTION("small delta-t") {
      dag::temporal_adjacency::limited_waiting_time<EdgeType> adj(2);
      dag::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
    SECTION("large delta-t") {
      dag::temporal_adjacency::limited_waiting_time<EdgeType> adj(5);
      dag::directed_network<EdgeType> eg = event_graph(network, adj);
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
      dag::temporal_adjacency::limited_waiting_time<EdgeType> adj(2);
      dag::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
    SECTION("large delta-t") {
      dag::temporal_adjacency::limited_waiting_time<EdgeType> adj(5);
      dag::directed_network<EdgeType> eg = event_graph(network, adj);
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
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}});
    SECTION("small delta-t") {
      dag::temporal_adjacency::limited_waiting_time<EdgeType> adj(2);
      dag::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1, 5}, {2, 3, 6, 7}},
              {{2, 1, 2, 3}, {1, 2, 5, 5}},
              {{1, 2, 5, 5}, {2, 3, 6, 7}},
              {{2, 3, 6, 7}, {3, 4, 8, 9}}
            })));
    }
    SECTION("large delta-t") {
      dag::temporal_adjacency::limited_waiting_time<EdgeType> adj(5);
      dag::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedEquals(
            std::vector<dag::directed_edge<EdgeType>>({
              {{1, 2, 1, 5}, {2, 3, 6, 7}},
              {{2, 1, 2, 3}, {1, 2, 5, 5}},
              {{1, 2, 5, 5}, {2, 3, 6, 7}},
              {{2, 3, 6, 7}, {3, 4, 8, 9}}
            })));
    }
  }
}

TEST_CASE("percolation out-cluster", "[dag::out_cluster]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  using AdjT = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  SECTION("from an event") {
    std::vector<
      std::pair<
        EdgeType,
        dag::temporal_cluster<EdgeType, AdjT>>> true_oc({
          {{1, 2, 1, 5},
            {{{1, 2, 1, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
          {{2, 1, 2, 3},
            {{{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
          {{1, 2, 5, 5},
            {{{1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
          {{2, 3, 6, 7},
            {{{2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
          {{3, 4, 8, 9},
            {{{3, 4, 8, 9}}, adj}},
          {{5, 6, 1, 3},
            {{{5, 6, 1, 3}}, adj}}});
    REQUIRE(std::ranges::all_of(true_oc,
          [&network, &adj](const auto& p) {
            return dag::out_cluster(network, adj, p.first) == p.second;
          }));
  }

  SECTION("from a node and time") {
    REQUIRE(dag::out_cluster(network, adj, 1, 3) ==
        dag::temporal_cluster<EdgeType, AdjT>({
          {1, 1, 3, 3},
          {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj));
    REQUIRE(dag::out_cluster(network, adj, 3, 7) ==
        dag::temporal_cluster<EdgeType, AdjT>({
          {3, 3, 7, 7},
          {3, 4, 8, 9}}, adj));
  }
}

TEST_CASE("percolation in-cluster", "[dag::in_cluster]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  using AdjT = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  SECTION("to an event") {
    std::vector<
      std::pair<
        EdgeType,
        dag::temporal_cluster<EdgeType, AdjT>>> true_ic({
          {{1, 2, 1, 5},
            {{{1, 2, 1, 5}}, adj}},
          {{2, 1, 2, 3},
            {{{2, 1, 2, 3}}, adj}},
          {{1, 2, 5, 5},
            {{{1, 2, 5, 5}, {2, 1, 2, 3}}, adj}},
          {{2, 3, 6, 7},
            {{{2, 3, 6, 7}, {1, 2, 5, 5}, {2, 1, 2, 3}, {1, 2, 1, 5}}, adj}},
          {{3, 4, 8, 9},
            {{{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {1, 2, 1, 5},
            {3, 4, 8, 9}}, adj}},
          {{5, 6, 1, 3},
            {{{5, 6, 1, 3}}, adj}}});
    REQUIRE(std::ranges::all_of(true_ic,
          [&network, &adj](const auto& p) {
            return dag::in_cluster(network, adj, p.first) == p.second;
          }));
  }

  SECTION("to a node and time") {
    REQUIRE(dag::in_cluster(network, adj, 4, 10) ==
        dag::temporal_cluster<EdgeType, AdjT>({
          {4, 4, 10, 10},
          {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {1, 2, 1, 5}, {3, 4, 8, 9}},
          adj));

    REQUIRE(dag::in_cluster(network, adj, 3, 8) ==
        dag::temporal_cluster<EdgeType, AdjT>({
          {3, 3, 8, 8},
          {2, 3, 6, 7}, {1, 2, 5, 5}, {2, 1, 2, 3}, {1, 2, 1, 5}}, adj));
  }
}

TEST_CASE("percolation out-clusters", "[dag::out_clusters]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  using AdjT = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  std::vector<
    std::pair<
      EdgeType,
      dag::temporal_cluster<EdgeType, AdjT>>> true_oc({
        {{1, 2, 1, 5},
          {{{1, 2, 1, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
        {{2, 1, 2, 3},
          {{{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
        {{1, 2, 5, 5},
          {{{1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
        {{2, 3, 6, 7},
          {{{2, 3, 6, 7}, {3, 4, 8, 9}}, adj}},
        {{3, 4, 8, 9},
          {{{3, 4, 8, 9}}, adj}},
        {{5, 6, 1, 3},
          {{{5, 6, 1, 3}}, adj}}});
  REQUIRE_THAT(dag::out_clusters(network, adj), UnorderedEquals(true_oc));
}

TEST_CASE("percolation in-clusters", "[dag::in_clusters]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  using AdjT = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  std::vector<
    std::pair<
      EdgeType,
      dag::temporal_cluster<EdgeType, AdjT>>> true_ic({
        {{1, 2, 1, 5},
          {{{1, 2, 1, 5}}, adj}},
        {{2, 1, 2, 3},
          {{{2, 1, 2, 3}}, adj}},
        {{1, 2, 5, 5},
          {{{1, 2, 5, 5}, {2, 1, 2, 3}}, adj}},
        {{2, 3, 6, 7},
          {{{2, 3, 6, 7}, {1, 2, 5, 5}, {2, 1, 2, 3}, {1, 2, 1, 5}}, adj}},
        {{3, 4, 8, 9},
          {{{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {1, 2, 1, 5},
          {3, 4, 8, 9}}, adj}},
        {{5, 6, 1, 3},
          {{{5, 6, 1, 3}}, adj}}});


  REQUIRE_THAT(dag::in_clusters(network, adj), UnorderedEquals(true_ic));
}

TEST_CASE("is reachable (temporal networks)", "[dag::is_reachable]") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  using AdjT = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  REQUIRE(dag::is_reachable(network, adj, 2, 0, 3, 8));
  REQUIRE(dag::is_reachable(network, adj, 2, 0, 4, 10));
  REQUIRE_FALSE(dag::is_reachable(network, adj, 3, 8, 2, 0));
  REQUIRE_FALSE(dag::is_reachable(network, adj, 5, 0, 4, 10));
}

TEST_CASE("static projection") {
  using EdgeType = dag::directed_delayed_temporal_edge<int, int>;
  dag::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  auto sp = dag::static_projection(network);
  REQUIRE(sp.vertices() == network.vertices());
  REQUIRE_THAT(sp.edges(),
      UnorderedEquals(std::vector<dag::directed_edge<int>>{
        {1, 2}, {2, 1}, {2, 3}, {3, 4}, {5, 6}}));
}
