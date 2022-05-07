#include <vector>
#include <set>
#include <unordered_set>

#include <catch2/catch.hpp>

#include <reticula/temporal_clusters.hpp>

#include <reticula/temporal_edges.hpp>
#include <reticula/temporal_hyperedges.hpp>

TEST_CASE("temporal cluster complies with the concept",
    "[reticula::temporal_cluster]") {
  using EdgeType = reticula::undirected_temporal_hyperedge<int, float>;
  using AdjType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  STATIC_REQUIRE(reticula::exact_temporal_cluster<
      reticula::temporal_cluster<EdgeType, AdjType>>);
  STATIC_REQUIRE(reticula::network_temporal_cluster<
      reticula::temporal_cluster<EdgeType, AdjType>>);

  STATIC_REQUIRE(reticula::network_temporal_cluster<
      reticula::temporal_cluster_sketch<EdgeType, AdjType>>);
}

TEST_CASE("temporal cluster properties", "[reticula::temporal_cluster]") {
  using EdgeType = reticula::undirected_temporal_hyperedge<int, float>;
  using AdjType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  using CompType = reticula::temporal_cluster<EdgeType, AdjType>;

  CompType comp(AdjType(3.0));

  comp.insert({{1, 2}, 1.0});
  comp.insert({{1, 3}, 3.0});
  comp.insert({{2, 5}, 3.0});
  comp.insert({{4, 5}, 5.0});

  SECTION("equality operator") {
    REQUIRE(comp == CompType(
          {{{1, 2}, 1.0}, {{1, 3}, 3.0}, {{2, 5}, 3.0}, {{4, 5}, 5.0}},
          AdjType(3.0)));
  }

  SECTION("correct basic properties") {
    REQUIRE(std::unordered_set<EdgeType>(comp.begin(), comp.end()) ==
      std::unordered_set<EdgeType>(
        {{{1, 2}, 1.0}, {{1, 3}, 3.0}, {{2, 5}, 3.0}, {{4, 5}, 5.0}}));
    REQUIRE(comp.lifetime() == std::pair<float, float>(1.0, 8.0));
    REQUIRE(comp.mass() == 21.0);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE_FALSE(comp.covers(5, 15.0));

    REQUIRE(comp.size() == 4);
  }

  SECTION("insertion") {
    comp.insert({{5, 1}, 12.0});
    REQUIRE(comp.size() == 5);
    REQUIRE(comp.mass() == 27.0);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE(comp.covers(5, 15.0));
  }

  SECTION("merging") {
    CompType comp2(AdjType(3.0));
    comp2.insert({{5, 1}, 12.0});

    comp.merge(comp2);

    REQUIRE(comp.size() == 5);
    REQUIRE(comp.mass() == 27.0);
    REQUIRE(comp.volume() == 5);

    REQUIRE(comp.covers(3, 5.0));
    REQUIRE(comp.covers(5, 15.0));
  }
}

TEST_CASE("temporal cluster size", "[reticula::temporal_cluster_size]") {
  using EdgeType = reticula::undirected_temporal_hyperedge<int, float>;
  using AdjType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  using CompType = reticula::temporal_cluster<EdgeType, AdjType>;

  CompType comp(AdjType(3.0));

  comp.insert({{1, 2}, 1.0});
  comp.insert({{1, 3}, 3.0});
  comp.insert({{2, 5}, 3.0});
  comp.insert({{4, 5}, 5.0});

  reticula::temporal_cluster_size<EdgeType, AdjType> comp_size(comp);

  REQUIRE(comp.size() == comp_size.size());
  REQUIRE(comp.volume() == comp_size.volume());
  REQUIRE(comp.mass() == comp_size.mass());
  REQUIRE(comp.lifetime() == comp_size.lifetime());
}


TEST_CASE("temporal cluster size estimate",
    "[reticula::temporal_cluster_size_estimate]") {
  using EdgeType = reticula::undirected_temporal_hyperedge<int, float>;
  using AdjType = reticula::temporal_adjacency::limited_waiting_time<EdgeType>;
  using CompType = reticula::temporal_cluster_sketch<EdgeType, AdjType>;

  CompType comp(AdjType(3.0));

  comp.insert({{1, 2}, 1.0});
  comp.insert({{1, 3}, 3.0});
  comp.insert({{2, 5}, 3.0});
  comp.insert({{4, 5}, 5.0});

  reticula::temporal_cluster_size_estimate<EdgeType, AdjType> comp_size(comp);

  REQUIRE(comp.size_estimate() == comp_size.size_estimate());
  REQUIRE(comp.mass_estimate() == comp_size.mass_estimate());
  REQUIRE(comp.volume_estimate() == comp_size.volume_estimate());
  REQUIRE(comp.lifetime() == comp_size.lifetime());
}
