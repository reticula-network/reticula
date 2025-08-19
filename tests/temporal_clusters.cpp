#include <reticula/algorithms/properties.hpp>
#include <unordered_set>

#include <catch2/catch_test_macros.hpp>

#include <reticula/temporal_clusters.hpp>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

#include "stringify.hpp"

TEST_CASE("temporal cluster properties", "[reticula::temporal_cluster]") {
  using EdgeType = reticula::undirected_temporal_hyperedge;
  using AdjType = reticula::adjacency::limited_waiting_time;
  using CompType = reticula::temporal_cluster<EdgeType>;

  CompType comp;
  AdjType adj(3.0);

  comp.insert({{1, 2}, 1.0}, adj);
  comp.insert({{1, 3}, 3.0}, adj);
  comp.insert({{2, 5}, 3.0}, adj);
  comp.insert({{4, 5}, 5.0}, adj);

  SECTION("correct basic properties") {
    REQUIRE(
      std::unordered_set<EdgeType>(comp.begin(), comp.end()) ==
      std::unordered_set<EdgeType>(
        {{{1, 2}, 1.0}, {{1, 3}, 3.0}, {{2, 5}, 3.0}, {{4, 5}, 5.0}}));
    std::cerr << std::format("window: {}", comp.lifetime()) << std::endl;
    REQUIRE(comp.lifetime() == reticula::window{1.0, 8.0});
    REQUIRE(comp.mass() == 21.0);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE_FALSE(comp.covers(5, 15.0));

    REQUIRE(comp.size() == 4);
  }

  SECTION("insertion") {
    comp.insert({{5, 1}, 12.0}, adj);
    REQUIRE(comp.size() == 5);
    REQUIRE(comp.mass() == 27.0);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE(comp.covers(5, 15.0));
  }

  SECTION("merging") {
    CompType comp2;
    comp2.insert({{5, 1}, 12.0}, adj);

    comp.merge(comp2);

    REQUIRE(comp.size() == 5);
    REQUIRE(comp.mass() == 27.0);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE(comp.covers(5, 15.0));
  }
}

TEST_CASE(
  "temporal cluster properties (simple)", "[reticula::temporal_cluster]") {
  using EdgeType = reticula::undirected_temporal_hyperedge;
  using AdjType = reticula::adjacency::simple;
  using CompType = reticula::temporal_cluster<EdgeType>;

  CompType comp;
  AdjType adj;

  comp.insert({{1, 2}, 1.0}, adj);
  comp.insert({{1, 3}, 3.0}, adj);
  comp.insert({{2, 5}, 3.0}, adj);
  comp.insert({{4, 5}, 5.0}, adj);

  double inf = std::numeric_limits<double>::infinity();

  SECTION("correct basic properties") {
    REQUIRE(
      std::unordered_set<EdgeType>(comp.begin(), comp.end()) ==
      std::unordered_set<EdgeType>(
        {{{1, 2}, 1.0}, {{1, 3}, 3.0}, {{2, 5}, 3.0}, {{4, 5}, 5.0}}));
    REQUIRE(comp.lifetime() == reticula::window{1.0, inf});
    REQUIRE(comp.mass() == inf);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE_FALSE(comp.covers(6, 15.0));

    REQUIRE(comp.size() == 4);
  }

  SECTION("insertion") {
    comp.insert({{5, 1}, 12.0}, adj);
    REQUIRE(comp.size() == 5);
    REQUIRE(comp.mass() == inf);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE(comp.covers(5, 15.0));
  }

  SECTION("merging") {
    CompType comp2;
    AdjType adj2;
    comp2.insert({{5, 1}, 12.0}, adj2);

    comp.merge(comp2);

    REQUIRE(comp.size() == 5);
    REQUIRE(comp.mass() == inf);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE(comp.covers(5, 15.0));
  }
}

TEST_CASE("temporal cluster size", "[reticula::temporal_cluster_size]") {
  using EdgeType = reticula::undirected_temporal_hyperedge;
  using AdjType = reticula::adjacency::limited_waiting_time;
  using CompType = reticula::temporal_cluster<EdgeType>;

  CompType comp;
  AdjType adj(3.0);

  comp.insert({{1, 2}, 1.0}, adj);
  comp.insert({{1, 3}, 3.0}, adj);
  comp.insert({{2, 5}, 3.0}, adj);
  comp.insert({{4, 5}, 5.0}, adj);

  reticula::temporal_cluster_size comp_size(comp);

  REQUIRE(comp.size() == comp_size.size());
  REQUIRE(comp.volume() == comp_size.volume());
  REQUIRE(comp.mass() == comp_size.mass());
  REQUIRE(comp.lifetime() == comp_size.lifetime());
}

TEST_CASE(
  "temporal cluster size estimate",
  "[reticula::temporal_cluster_size_estimate]") {
  using EdgeType = reticula::undirected_temporal_hyperedge;
  using AdjType = reticula::adjacency::limited_waiting_time;
  using CompType = reticula::temporal_cluster_sketch<EdgeType>;

  CompType comp(0.01, 42);
  AdjType adj(3.0);

  comp.insert({{1, 2}, 1.0}, adj);
  comp.insert({{1, 3}, 3.0}, adj);
  comp.insert({{2, 5}, 3.0}, adj);
  comp.insert({{4, 5}, 5.0}, adj);

  reticula::temporal_cluster_size_estimate comp_size(comp);

  REQUIRE(comp.size_estimate() == comp_size.size_estimate());
  REQUIRE(comp.mass_estimate() == comp_size.mass_estimate());
  REQUIRE(comp.volume_estimate() == comp_size.volume_estimate());
  REQUIRE(comp.lifetime() == comp_size.lifetime());
}
