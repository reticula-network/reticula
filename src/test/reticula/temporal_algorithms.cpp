#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/ranges.hpp>
#include <reticula/utils.hpp>
#include <reticula/temporal_edges.hpp>
#include <reticula/networks.hpp>
#include <reticula/temporal_algorithms.hpp>
#include <reticula/temporal_adjacency.hpp>
#include <reticula/random_networks.hpp>

TEST_CASE("time_window",
    "[reticula::time_window]"
    "[reticula::cause_time_window]"
    "[reticula::effect_time_window]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}});
  REQUIRE(reticula::cause_time_window(network) == std::make_pair(1, 8));
  REQUIRE(reticula::effect_time_window(network) == std::make_pair(3, 9));
  REQUIRE(reticula::time_window(network) == std::make_pair(1, 9));

  reticula::network<EdgeType> empty_network;
  REQUIRE_THROWS_AS(reticula::cause_time_window(empty_network),
      std::invalid_argument);
  REQUIRE_THROWS_AS(reticula::effect_time_window(empty_network),
      std::invalid_argument);
  REQUIRE_THROWS_AS(reticula::time_window(empty_network),
      std::invalid_argument);
}

TEST_CASE("event graph", "[reticula::event_graph]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge<int, int>;
    reticula::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
    SECTION("small delta-t") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(2);
      reticula::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
    SECTION("large delta-t") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(5);
      reticula::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<EdgeType>>({
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
    using EdgeType = reticula::directed_temporal_edge<int, int>;
    reticula::network<EdgeType> network(
        {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
    SECTION("small delta-t") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(2);
      reticula::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
    SECTION("large delta-t") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(5);
      reticula::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<EdgeType>>({
              {{1, 2, 1}, {2, 1, 2}},
              {{1, 2, 1}, {2, 3, 6}},
              {{2, 1, 2}, {1, 2, 5}},
              {{1, 2, 5}, {2, 3, 6}},
              {{2, 3, 6}, {3, 4, 8}}
            })));
    }
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
    reticula::network<EdgeType> network(
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}});
    SECTION("small delta-t") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(2);
      reticula::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<EdgeType>>({
              {{1, 2, 1, 5}, {2, 3, 6, 7}},
              {{2, 1, 2, 3}, {1, 2, 5, 5}},
              {{1, 2, 5, 5}, {2, 3, 6, 7}},
              {{2, 3, 6, 7}, {3, 4, 8, 9}}
            })));
    }
    SECTION("large delta-t") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(5);
      reticula::directed_network<EdgeType> eg = event_graph(network, adj);
      REQUIRE_THAT(eg.edges(),
          UnorderedRangeEquals(
            std::vector<reticula::directed_edge<EdgeType>>({
              {{1, 2, 1, 5}, {2, 3, 6, 7}},
              {{2, 1, 2, 3}, {1, 2, 5, 5}},
              {{1, 2, 5, 5}, {2, 3, 6, 7}},
              {{2, 3, 6, 7}, {3, 4, 8, 9}}
            })));
    }
  }
}

TEST_CASE("percolation simple out-cluster", "[reticula::out_cluster]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, double>;
  using AdjT = reticula::temporal_adjacency::simple<EdgeType>;
  AdjT adj;
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  SECTION("from an event") {
    std::vector<
      std::pair<
        EdgeType,
        reticula::temporal_cluster<EdgeType, AdjT>>> true_oc({
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
    REQUIRE(reticula::ranges::all_of(true_oc,
          [&network, &adj](const auto& p) {
            return reticula::out_cluster(network, adj, p.first) == p.second;
          }));
  }

  SECTION("from a node and time") {
    REQUIRE(reticula::out_cluster(network, adj, 1, 3) ==
        reticula::temporal_cluster<EdgeType, AdjT>({
          {1, 1, 3, 3},
          {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj));
    REQUIRE(reticula::out_cluster(network, adj, 3, 7) ==
        reticula::temporal_cluster<EdgeType, AdjT>({
          {3, 3, 7, 7},
          {3, 4, 8, 9}}, adj));
  }
}

TEST_CASE("percolation LWT out-cluster", "[reticula::out_cluster]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  using AdjT = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  SECTION("from an event") {
    std::vector<
      std::pair<
        EdgeType,
        reticula::temporal_cluster<EdgeType, AdjT>>> true_oc({
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
    REQUIRE(reticula::ranges::all_of(true_oc,
          [&network, &adj](const auto& p) {
            return reticula::out_cluster(network, adj, p.first) == p.second;
          }));
  }

  SECTION("from a node and time") {
    REQUIRE(reticula::out_cluster(network, adj, 1, 3) ==
        reticula::temporal_cluster<EdgeType, AdjT>({
          {1, 1, 3, 3},
          {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}}, adj));
    REQUIRE(reticula::out_cluster(network, adj, 3, 7) ==
        reticula::temporal_cluster<EdgeType, AdjT>({
          {3, 3, 7, 7},
          {3, 4, 8, 9}}, adj));
  }
}

TEST_CASE("percolation in-cluster", "[reticula::in_cluster]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  using AdjT = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  SECTION("to an event") {
    std::vector<
      std::pair<
        EdgeType,
        reticula::temporal_cluster<EdgeType, AdjT>>> true_ic({
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
    REQUIRE(reticula::ranges::all_of(true_ic,
          [&network, &adj](const auto& p) {
            return reticula::in_cluster(network, adj, p.first) == p.second;
          }));
  }

  SECTION("to a node and time") {
    REQUIRE(reticula::in_cluster(network, adj, 4, 10) ==
        reticula::temporal_cluster<EdgeType, AdjT>({
          {4, 4, 10, 10},
          {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {1, 2, 1, 5}, {3, 4, 8, 9}},
          adj));

    REQUIRE(reticula::in_cluster(network, adj, 3, 8) ==
        reticula::temporal_cluster<EdgeType, AdjT>({
          {3, 3, 8, 8},
          {2, 3, 6, 7}, {1, 2, 5, 5}, {2, 1, 2, 3}, {1, 2, 1, 5}}, adj));
  }
}


TEST_CASE("percolation out-clusters", "[reticula::out_clusters]") {
  SECTION("small example") {
    using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
    using AdjT = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
    AdjT adj(2);
    reticula::network<EdgeType> network(
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
          {5, 6, 1, 3}});

    std::vector<
      std::pair<
        EdgeType,
        reticula::temporal_cluster<EdgeType, AdjT>>> true_oc({
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
    REQUIRE_THAT(reticula::out_clusters(network, adj),
        UnorderedEquals(true_oc));
  }

  SECTION("random network") {
    std::mt19937_64 state(42);
    auto network =
      reticula::random_directed_fully_mixed_temporal_network<int>(
          12, 0.03, 50, state);
    reticula::temporal_adjacency::simple<
      reticula::directed_temporal_edge<int, double>> adj;
    REQUIRE_NOTHROW(reticula::out_clusters(network, adj));
  }
}

TEST_CASE("percolation in-clusters", "[reticula::in_clusters]") {
  SECTION("small example") {
    using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
    using AdjT = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
    AdjT adj(2);
    reticula::network<EdgeType> network(
        {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
          {5, 6, 1, 3}});

    std::vector<
      std::pair<
        EdgeType,
        reticula::temporal_cluster<EdgeType, AdjT>>> true_ic({
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


    REQUIRE_THAT(reticula::in_clusters(network, adj), UnorderedRangeEquals(true_ic));
  }

  SECTION("random network") {
    std::mt19937_64 state(42);
    auto network =
      reticula::random_directed_fully_mixed_temporal_network<int>(
          12, 0.03, 50, state);
    reticula::temporal_adjacency::simple<
      reticula::directed_temporal_edge<int, double>> adj;
    REQUIRE_NOTHROW(reticula::in_clusters(network, adj));
  }
}

TEST_CASE("is reachable (temporal networks)", "[reticula::is_reachable]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  using AdjT = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  AdjT adj(2);
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  REQUIRE(reticula::is_reachable(network, adj, 2, 0, 3, 8));
  REQUIRE(reticula::is_reachable(network, adj, 2, 0, 4, 10));
  REQUIRE_FALSE(reticula::is_reachable(network, adj, 3, 8, 2, 0));
  REQUIRE_FALSE(reticula::is_reachable(network, adj, 5, 0, 4, 10));
}

TEST_CASE("static projection", "[reticula::static_projection]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  auto sp = reticula::static_projection(network);
  REQUIRE_THAT(sp.vertices(), RangeEquals(network.vertices()));
  REQUIRE_THAT(sp.edges(),
      UnorderedRangeEquals(std::vector<reticula::directed_edge<int>>{
        {1, 2}, {2, 1}, {2, 3}, {3, 4}, {5, 6}}));
}

TEST_CASE("link timeline", "[reticula::link_timeline]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  auto tl = reticula::link_timeline(network, {1, 2});
  REQUIRE(tl == std::vector<EdgeType>{
        {1, 2, 1, 5}, {1, 2, 5, 5}});
}

TEST_CASE("link timelines", "[reticula::link_timelines]") {
  using EdgeType = reticula::directed_delayed_temporal_edge<int, int>;
  reticula::network<EdgeType> network(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9},
        {5, 6, 1, 3}});

  auto tls = reticula::link_timelines(network);

  using S = EdgeType::StaticProjectionType;
  for (auto& [s, v]: tls) {
    if (s == S{1, 2})
      REQUIRE(v == std::vector<EdgeType>{{1, 2, 1, 5}, {1, 2, 5, 5}});
    else if (s == S{2, 1})
      REQUIRE(v == std::vector<EdgeType>{{2, 1, 2, 3}});
    else if (s == S{2, 3})
      REQUIRE(v == std::vector<EdgeType>{{2, 3, 6, 7}});
    else if (s == S{3, 4})
      REQUIRE(v == std::vector<EdgeType>{{3, 4, 8, 9}});
    else  // if (s == S{5, 6})
      REQUIRE(v == std::vector<EdgeType>{{5, 6, 1, 3}});
  }
}
