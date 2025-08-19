#include <unordered_set>

#include <catch2/catch_test_macros.hpp>

#include <reticula/components.hpp>
#include <reticula/networks.hpp>

TEST_CASE("component complies with the concept", "[reticula::component]") {
  STATIC_REQUIRE(reticula::component_like<reticula::component>);
  STATIC_REQUIRE(reticula::exact_component_like<reticula::component>);
  REQUIRE_NOTHROW(reticula::component());
}

TEST_CASE("component properties", "[reticula::component]") {
  reticula::component comp(0);

  comp.insert(0);
  comp.insert(1);
  comp.insert(2);

  SECTION("equality operator") {
    REQUIRE(comp == reticula::component({0, 1, 2}));
    REQUIRE(comp == reticula::component({1, 0, 2}));
  }

  SECTION("correct basic properties") {
    REQUIRE(
      std::unordered_set<reticula::VertexType>(comp.begin(), comp.end()) ==
      std::unordered_set<reticula::VertexType>({0, 1, 2}));
    REQUIRE(comp.contains(0));
    REQUIRE(comp.contains(1));
    REQUIRE(comp.contains(2));
    REQUIRE(comp.size() == 3);
  }

  SECTION("insertion") {
    comp.insert(3);

    REQUIRE(
      std::unordered_set<reticula::VertexType>(comp.begin(), comp.end()) ==
      std::unordered_set<reticula::VertexType>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }

  SECTION("merging") {
    reticula::component comp2(0);

    comp2.insert(3);
    comp.merge(comp2);

    REQUIRE(
      std::unordered_set<reticula::VertexType>(comp.begin(), comp.end()) ==
      std::unordered_set<reticula::VertexType>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }
}

TEST_CASE(
  "component sketch complies with the concept",
  "[reticula::component_sketch]") {
  STATIC_REQUIRE(reticula::component_like<reticula::component_sketch>);
  REQUIRE_NOTHROW(reticula::component_sketch());
}

TEST_CASE("component sketch properties", "[reticula::component_sketch]") {
  reticula::component_sketch comp(0, 0);

  comp.insert(std::vector<reticula::VertexType>{0, 1, 2});

  SECTION("merging") {
    reticula::component comp2(0);

    comp2.insert(3);
    comp.merge(comp2);

    reticula::component_sketch comp3({0, 1, 2, 3}, 0, 0);

    REQUIRE(comp.size_estimate() == comp3.size_estimate());
  }
}
