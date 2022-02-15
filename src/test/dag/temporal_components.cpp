#include <vector>
#include <set>
#include <unordered_set>

#include <catch2/catch.hpp>

#include <dag/temporal_components.hpp>

#include <dag/temporal_edges.hpp>
#include <dag/temporal_hyperedges.hpp>

TEST_CASE("temporal component complies with the concept",
    "[dag::temporal_component]") {
  using EdgeType = dag::undirected_temporal_hyperedge<int, float>;
  using AdjType = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  STATIC_REQUIRE(dag::exact_temporal_component<
      dag::temporal_component<EdgeType, AdjType>>);
  STATIC_REQUIRE(dag::network_temporal_component<
      dag::temporal_component<EdgeType, AdjType>>);
}

TEST_CASE("temporal component properties", "[dag::temporal_component]") {
  using EdgeType = dag::undirected_temporal_hyperedge<int, float>;
  using AdjType = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  using CompType = dag::temporal_component<EdgeType, AdjType>;

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

TEST_CASE("temporal component size", "[dag::temporal_component_size]") {
  using EdgeType = dag::undirected_temporal_hyperedge<int, float>;
  using AdjType = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  using CompType = dag::temporal_component<EdgeType, AdjType>;

  CompType comp(AdjType(3.0));

  comp.insert({{1, 2}, 1.0});
  comp.insert({{1, 3}, 3.0});
  comp.insert({{2, 5}, 3.0});
  comp.insert({{4, 5}, 5.0});

  dag::temporal_component_size<EdgeType, AdjType> comp_size(comp);

  REQUIRE(comp.size() == comp_size.size());
  REQUIRE(comp.volume() == comp_size.volume());
  REQUIRE(comp.mass() == comp_size.mass());
  REQUIRE(comp.lifetime() == comp_size.lifetime());
}


TEST_CASE("temporal component size estimate",
    "[dag::temporal_component_size_estimate]") {
  using EdgeType = dag::undirected_temporal_hyperedge<int, float>;
  using AdjType = dag::temporal_adjacency::limited_waiting_time<EdgeType>;
  using CompType = dag::temporal_component_sketch<EdgeType, AdjType>;

  CompType comp(AdjType(3.0));

  comp.insert({{1, 2}, 1.0});
  comp.insert({{1, 3}, 3.0});
  comp.insert({{2, 5}, 3.0});
  comp.insert({{4, 5}, 5.0});

  dag::temporal_component_size_estimate<EdgeType, AdjType> comp_size(comp);

  REQUIRE(comp.size_estimate() == comp_size.size_estimate());
  REQUIRE(comp.mass_estimate() == comp_size.mass_estimate());
  REQUIRE(comp.volume_estimate() == comp_size.volume_estimate());
  REQUIRE(comp.lifetime() == comp_size.lifetime());
}
