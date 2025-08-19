#include <catch2/catch_test_macros.hpp>

#include <catch2/matchers/catch_matchers_range_equals.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/event_graphs.hpp>
#include <reticula/networks.hpp>

// These are the cases in SI Fig. 1 of https://arxiv.org/pdf/2107.01510
TEST_CASE(
  "event graph successors reduction", "[reticula::event_graph::successors]") {
  using EdgeType = reticula::undirected_temporal_edge;
  reticula::network<EdgeType> list1{
    {1, 2, 0.0}, {1, 2, 1.0}, {2, 3, 2.0}, {0, 1, 3.0}};

  reticula::network<EdgeType> list2{
    {1, 2, 0.0}, {2, 3, 1.0}, {1, 2, 2.0}, {0, 1, 3.0}};

  reticula::network<EdgeType> list3{
    {1, 2, 0.0}, {2, 3, 1.0}, {0, 1, 2.0}, {1, 2, 3.0}};

  auto source = list1.edges_cause().front();

  using namespace reticula::event_graph;

  SECTION("successors are calculated correctly for dt=0.5") {
    reticula::adjacency::limited_waiting_time adj(0.5);

    REQUIRE_THAT(
      reticula::event_graph::successors(list1, source, adj, true),
      RangeEquals(std::vector<EdgeType>()));

    REQUIRE_THAT(
      reticula::event_graph::successors(list2, source, adj, true),
      RangeEquals(std::vector<EdgeType>()));

    REQUIRE_THAT(
      reticula::event_graph::successors(list3, source, adj, true),
      RangeEquals(std::vector<EdgeType>()));
  }

  SECTION("successors are calculated correctly for dt=1.5") {
    reticula::adjacency::limited_waiting_time adj(1.5);

    REQUIRE_THAT(
      reticula::event_graph::successors(list1, source, adj, true),
      RangeEquals(std::vector<EdgeType>({{1, 2, 1.0}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(list2, source, adj, true),
      RangeEquals(std::vector<EdgeType>({{2, 3, 1.0}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(list3, source, adj, true),
      RangeEquals(std::vector<EdgeType>({{2, 3, 1.0}})));
  }

  SECTION("successors are calculated correctly for dt=2.5") {
    reticula::adjacency::limited_waiting_time adj(2.5);

    REQUIRE_THAT(
      reticula::event_graph::successors(list1, source, adj, true),
      RangeEquals(std::vector<EdgeType>({{1, 2, 1.0}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(list2, source, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 1.0}, {1, 2, 2.0}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(list3, source, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 1.0}, {0, 1, 2.0}})));
  }

  SECTION("successors are calculated correctly for dt=3.5") {
    reticula::adjacency::limited_waiting_time adj(3.5);

    REQUIRE_THAT(
      reticula::event_graph::successors(list1, source, adj, true),
      RangeEquals(std::vector<EdgeType>({{1, 2, 1.0}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(list2, source, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 1.0}, {1, 2, 2.0}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(list3, source, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 1.0}, {0, 1, 2.0}})));
  }
}

TEST_CASE("event graph successors", "[reticula::event_graph::successors]") {
  SECTION("directed_temporal_edge") {
    using EdgeType = reticula::directed_temporal_edge;
    reticula::network<EdgeType> event_list{
      {1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};
    reticula::adjacency::limited_waiting_time adj(6);

    REQUIRE_THAT(
      reticula::event_graph::successors(event_list, {2, 1, 2}, adj),
      RangeEquals(std::vector<EdgeType>({{1, 2, 5}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(event_list, {2, 1, 2}, adj, true),
      RangeEquals(std::vector<EdgeType>({{1, 2, 5}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(event_list, {1, 2, 1}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 1, 2}, {2, 3, 6}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(event_list, {1, 2, 1}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 1, 2}, {2, 3, 6}})));
  }

  SECTION("undirected_temporal_edge") {
    using EdgeType = reticula::undirected_temporal_edge;
    reticula::network<EdgeType> net{
      {1, 2, 1}, {1, 2, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};
    reticula::adjacency::limited_waiting_time adj(6);

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 1}, adj),
      UnorderedRangeEquals(
        std::vector<EdgeType>({{1, 2, 2}, {1, 2, 5}, {2, 3, 6}})));
    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 1}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 2}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 2}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5}, {2, 3, 6}})));
    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 2}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {2, 3, 6}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{3, 4, 8}})));
    REQUIRE(reticula::event_graph::successors(net, {3, 4, 8}, adj).empty());

    reticula::adjacency::limited_waiting_time tight(1.0);
    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 1}, tight),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 2}})));
    REQUIRE(reticula::event_graph::successors(net, {1, 2, 2}, tight).empty());
  }

  SECTION("directed_delayed_temporal_edge") {
    using EdgeType = reticula::directed_delayed_temporal_edge;
    reticula::network<EdgeType> net(
      {{1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9},
       {5, 6, 1, 3}});
    reticula::adjacency::limited_waiting_time adj(2.5);

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {2, 1, 2, 3}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5, 5}})));
    REQUIRE_THAT(
      reticula::event_graph::successors(net, {2, 1, 2, 3}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5, 5}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 1, 5}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 6, 7}})));
    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 1, 5}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 6, 7}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 5, 5}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 6, 7}})));

    REQUIRE_THAT(
      reticula::event_graph::successors(net, {2, 3, 6, 7}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{3, 4, 8, 9}})));

    REQUIRE(reticula::event_graph::successors(net, {3, 4, 8, 9}, adj).empty());
    REQUIRE(reticula::event_graph::successors(net, {5, 6, 1, 3}, adj).empty());

    reticula::adjacency::limited_waiting_time tight(1.0);

    REQUIRE(
      reticula::event_graph::successors(net, {2, 1, 2, 3}, tight).empty());
    REQUIRE_THAT(
      reticula::event_graph::successors(net, {1, 2, 1, 5}, tight),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 6, 7}})));
  }
}

TEST_CASE("event graph predecessors", "[reticula::event_graph::predecessors]") {
  SECTION("directed_temporal_edge") {
    using EdgeType = reticula::directed_temporal_edge;
    reticula::network<EdgeType> event_list{
      {1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};
    reticula::adjacency::limited_waiting_time adj(6);

    REQUIRE_THAT(
      reticula::event_graph::predecessors(event_list, {2, 3, 6}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5}, {1, 2, 1}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(event_list, {2, 3, 6}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5}, {1, 2, 1}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(event_list, {1, 2, 5}, adj),
      RangeEquals(std::vector<EdgeType>({{2, 1, 2}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(event_list, {1, 2, 5}, adj, true),
      RangeEquals(std::vector<EdgeType>({{2, 1, 2}})));
  }

  SECTION("undirected_temporal_edge") {
    using EdgeType = reticula::undirected_temporal_edge;
    reticula::network<EdgeType> net{
      {1, 2, 1}, {1, 2, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};
    reticula::adjacency::limited_waiting_time adj(6);

    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {2, 3, 6}, adj),
      UnorderedRangeEquals(
        std::vector<EdgeType>({{1, 2, 1}, {1, 2, 2}, {1, 2, 5}})));
    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {2, 3, 6}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {1, 2, 5}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 1}, {1, 2, 2}})));
    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {1, 2, 5}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 2}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {1, 2, 2}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 1}})));
    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {1, 2, 2}, adj, true),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 1}})));

    REQUIRE(reticula::event_graph::predecessors(net, {1, 2, 1}, adj).empty());

    reticula::adjacency::limited_waiting_time tight(1.0);
    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {2, 3, 6}, tight),
      UnorderedRangeEquals(std::vector<EdgeType>({{1, 2, 5}})));
  }

  SECTION("directed_delayed_temporal_edge") {
    using EdgeType = reticula::directed_delayed_temporal_edge;
    reticula::network<EdgeType> net(
      {{1, 2, 1, 5},
       {2, 1, 2, 3},
       {1, 2, 5, 5},
       {2, 3, 6, 7},
       {3, 4, 8, 9},
       {5, 6, 1, 3}});
    reticula::adjacency::limited_waiting_time adj(2.5);

    REQUIRE(
      reticula::event_graph::predecessors(net, {1, 2, 1, 5}, adj).empty());

    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {1, 2, 5, 5}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 1, 2, 3}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {2, 3, 6, 7}, adj),
      UnorderedRangeEquals(
        std::vector<EdgeType>({{1, 2, 1, 5}, {1, 2, 5, 5}})));

    REQUIRE_THAT(
      reticula::event_graph::predecessors(net, {3, 4, 8, 9}, adj),
      UnorderedRangeEquals(std::vector<EdgeType>({{2, 3, 6, 7}})));

    REQUIRE(
      reticula::event_graph::predecessors(net, {2, 1, 2, 3}, adj).empty());
    REQUIRE(
      reticula::event_graph::predecessors(net, {5, 6, 1, 3}, adj).empty());

    reticula::adjacency::limited_waiting_time tight(1.0);

    REQUIRE(
      reticula::event_graph::predecessors(net, {1, 2, 5, 5}, tight).empty());
  }
}
