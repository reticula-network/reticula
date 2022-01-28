#include <vector>
#include <set>
#include <unordered_set>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include <dag/components.hpp>

#include <dag/static_edges.hpp>
#include <dag/temporal_edges.hpp>
#include <dag/static_hyperedges.hpp>
#include <dag/temporal_hyperedges.hpp>

TEST_CASE("component complies with the concept", "[dag::component]") {
  STATIC_REQUIRE(dag::exact_component<dag::component<int>>);
  STATIC_REQUIRE(dag::network_component<dag::component<int>>);
}

TEST_CASE("component sketch complies with the concept",
    "[dag::component_sketch]") {
  STATIC_REQUIRE(dag::network_component<
      dag::component_sketch<int>>);
  STATIC_REQUIRE(dag::network_component<
      dag::component_sketch<std::pair<int, int>>>);
  STATIC_REQUIRE(dag::network_component<
      dag::component_sketch<
        dag::directed_temporal_hyperedge<std::pair<int, int>, double>>>);
}

TEST_CASE("component properties", "[dag::component]") {
  using CompType = dag::component<int>;

  CompType comp(0, 0);

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
    CompType comp2(0, 0);

    comp2.insert(3);
    comp.merge(comp2);

    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }
}
