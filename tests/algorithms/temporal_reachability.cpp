#include <catch2/catch_test_macros.hpp>

#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <ranges>
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/algorithms/temporal_reachability.hpp>

TEST_CASE("in_cluster", "[reticula::in_cluster]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto c = reticula::in_cluster(net, {2, 3, 6}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{2, 3, 6}, {1, 2, 5}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::in_cluster(net, 2, 6, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert({{2, 2, 6}, {1, 2, 5}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }

  SECTION("directed temporal network") {
    using EdgeType = reticula::directed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto c = reticula::in_cluster(net, {2, 3, 6}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{2, 3, 6}, {1, 2, 5}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::in_cluster(net, 2, 6, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert({{2, 2, 6}, {1, 2, 5}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9},
       {5, 6, 1, 3}});
    AdjType adj(2.5);

    auto c = reticula::in_cluster(net, {2, 3, 6, 7}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert(
      {{2, 1, 2, 3}, {2, 3, 6, 7}, {1, 2, 5, 5}, {1, 2, 1, 5}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::in_cluster(net, 2, 7, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert(
      {{2, 1, 2, 3}, {2, 2, 7, 7}, {1, 2, 5, 5}, {1, 2, 1, 5}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }
}

TEST_CASE("out_cluster", "[reticula::out_cluster]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto c = reticula::out_cluster(net, {2, 3, 6}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{2, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::out_cluster(net, 3, 6, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert({{3, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }

  SECTION("directed temporal network") {
    using EdgeType = reticula::directed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto c = reticula::out_cluster(net, {2, 3, 6}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{2, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::out_cluster(net, 3, 6, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert({{3, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9},
       {5, 6, 1, 3}});
    AdjType adj(2.5);

    auto c = reticula::out_cluster(net, {2, 3, 6, 7}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{2, 3, 6, 7}, {3, 4, 8, 9}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::out_cluster(net, 3, 7, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert({{3, 3, 7, 7}, {3, 4, 8, 9}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }
}

TEST_CASE("weak_cluster", "[reticula::weak_cluster]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto c = reticula::weak_cluster(net, {2, 3, 6}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::weak_cluster(net, 2, 6, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert({{2, 2, 6}, {2, 3, 6}, {1, 2, 5}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }

  SECTION("directed temporal network") {
    using EdgeType = reticula::directed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto c = reticula::weak_cluster(net, {2, 3, 6}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert({{2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::weak_cluster(net, 2, 6, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert(
      {{2, 2, 6}, {2, 3, 6}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}}, adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9},
       {5, 6, 1, 3}});
    AdjType adj(2.5);

    auto c = reticula::weak_cluster(net, {2, 3, 6, 7}, adj);
    reticula::temporal_cluster<EdgeType> expected;
    expected.insert(
      {{1, 2, 1, 5}, {2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {3, 4, 8, 9}},
      adj);
    REQUIRE_THAT(c, UnorderedRangeEquals(expected));

    auto c2 = reticula::weak_cluster(net, 2, 7, adj);
    reticula::temporal_cluster<EdgeType> expected2;
    expected2.insert(
      {{2, 2, 7, 7},
       {1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9}},
      adj);
    REQUIRE_THAT(c2, UnorderedRangeEquals(expected2));
  }
}

TEST_CASE("in-clusters", "[reticula::in_clusters]") {
  SECTION("undirected temporal network") {
    using EdgeType = reticula::undirected_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto clusters = reticula::in_clusters(net, adj);
    std::unordered_map<EdgeType, std::vector<EdgeType>> expected = {
      {{2, 1, 2}, {{2, 1, 2}, {1, 2, 1}}},
      {{1, 2, 1}, {{1, 2, 1}}},
      {{1, 2, 5}, {{1, 2, 5}}},
      {{2, 3, 6}, {{1, 2, 5}, {2, 3, 6}}},
      {{3, 4, 8}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
      {{5, 6, 1}, {{5, 6, 1}}},
    };
    REQUIRE_THAT(
      clusters | std::views::keys,
      UnorderedRangeEquals(expected | std::views::keys));
    for (auto& [edge, cluster] : clusters)
      REQUIRE_THAT(cluster, UnorderedRangeEquals(expected.at(edge)));
  }

  SECTION("directed temporal network") {
    using EdgeType = reticula::directed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}, {5, 6, 1}});
    AdjType adj(2.5);

    auto clusters = reticula::in_clusters(net, adj);
    std::unordered_map<EdgeType, std::vector<EdgeType>> expected = {
      {{2, 1, 2}, {{2, 1, 2}, {1, 2, 1}}},
      {{1, 2, 1}, {{1, 2, 1}}},
      {{1, 2, 5}, {{1, 2, 5}}},
      {{2, 3, 6}, {{1, 2, 5}, {2, 3, 6}}},
      {{3, 4, 8}, {{1, 2, 5}, {2, 3, 6}, {3, 4, 8}}},
      {{5, 6, 1}, {{5, 6, 1}}},
    };
    REQUIRE_THAT(
      clusters | std::views::keys,
      UnorderedRangeEquals(expected | std::views::keys));
    for (auto& [edge, cluster] : clusters)
      REQUIRE_THAT(cluster, UnorderedRangeEquals(expected.at(edge)));
  }

  SECTION("directed delayed temporal network") {
    using EdgeType = reticula::directed_delayed_temporal_edge;
    using AdjType = reticula::adjacency::limited_waiting_time;
    reticula::network<EdgeType> net(
      {{1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9},
       {5, 6, 1, 3}});
    AdjType adj(2.5);

    auto clusters = reticula::in_clusters(net, adj);
    std::unordered_map<EdgeType, std::vector<EdgeType>> expected = {
      {{1, 2, 1, 5}, {{1, 2, 1, 5}}},
      {{2, 1, 2, 3}, {{2, 1, 2, 3}}},
      {{1, 2, 5, 5}, {{1, 2, 5, 5}, {2, 1, 2, 3}}},
      {{2, 3, 6, 7}, {{2, 3, 6, 7}, {1, 2, 5, 5}, {2, 1, 2, 3}, {1, 2, 1, 5}}},
      {{3, 4, 8, 9},
       {{2, 1, 2, 3}, {1, 2, 5, 5}, {2, 3, 6, 7}, {1, 2, 1, 5}, {3, 4, 8, 9}}},
      {{5, 6, 1, 3}, {{5, 6, 1, 3}}}};
    REQUIRE_THAT(
      clusters | std::views::keys,
      UnorderedRangeEquals(expected | std::views::keys));
    for (auto& [edge, cluster] : clusters)
      REQUIRE_THAT(cluster, UnorderedRangeEquals(expected.at(edge)));
  }
}
