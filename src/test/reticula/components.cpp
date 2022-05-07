#include <vector>
#include <set>
#include <unordered_set>

#include <catch2/catch.hpp>

#include <reticula/components.hpp>

#include <reticula/static_edges.hpp>
#include <reticula/temporal_edges.hpp>
#include <reticula/static_hyperedges.hpp>
#include <reticula/temporal_hyperedges.hpp>

TEST_CASE("component complies with the concept", "[reticula::component]") {
  STATIC_REQUIRE(reticula::exact_component<reticula::component<int>>);
  STATIC_REQUIRE(reticula::network_component<reticula::component<int>>);

  STATIC_REQUIRE(reticula::network_component<
      reticula::component<reticula::directed_edge<int>>>);
  REQUIRE_NOTHROW(reticula::component<int>());
  REQUIRE_NOTHROW(reticula::component<
        reticula::directed_temporal_hyperedge<std::pair<int, int>, double>>());
}

TEST_CASE("component sketch complies with the concept",
    "[reticula::component_sketch]") {
  STATIC_REQUIRE(reticula::network_component<
      reticula::component_sketch<int>>);
  STATIC_REQUIRE(reticula::network_component<
      reticula::component_sketch<std::pair<int, int>>>);
  STATIC_REQUIRE(reticula::network_component<
      reticula::component_sketch<
        reticula::directed_temporal_hyperedge<std::pair<int, int>, double>>>);
  REQUIRE_NOTHROW(reticula::component_sketch<int>());
  REQUIRE_NOTHROW(reticula::component_sketch<
        reticula::directed_temporal_hyperedge<std::pair<int, int>, double>>());
}

TEST_CASE("component properties", "[reticula::component]") {
  using CompType = reticula::component<int>;

  CompType comp(0);

  comp.insert(0);
  comp.insert(1);
  comp.insert(2);

  SECTION("equality operator") {
    REQUIRE(comp == CompType({0, 1, 2}));
    REQUIRE(comp == CompType({1, 0, 2}));
  }

  SECTION("correct basic properties") {
    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2}));
    REQUIRE(comp.contains(0));
    REQUIRE(comp.contains(1));
    REQUIRE(comp.contains(2));
    REQUIRE(comp.size() == 3);
  }

  SECTION("insertion") {
    comp.insert(3);

    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }

  SECTION("merging") {
    CompType comp2(0);

    comp2.insert(3);
    comp.merge(comp2);

    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }
}

TEST_CASE("component size", "[reticula::component_size]") {
  using CompType = reticula::component<int>;

  CompType comp(0);

  comp.insert(0);
  comp.insert(1);
  comp.insert(2);

  reticula::component_size<int> comp_size(comp);
  REQUIRE(comp_size.size() == comp.size());
}

TEST_CASE("component size estimate", "[reticula::component_size_estimate]") {
  using CompType = reticula::component_sketch<int>;

  CompType comp(0);

  comp.insert(0);
  comp.insert(1);
  comp.insert(2);

  reticula::component_size_estimate<int> comp_size_est(comp);
  REQUIRE(comp_size_est.size_estimate() == comp.size_estimate());
}
