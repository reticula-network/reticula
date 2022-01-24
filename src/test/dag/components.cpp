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

TEMPLATE_TEST_CASE("component complies with the concept", "[dag::component]",
    dag::directed_edge<int>, dag::directed_hyperedge<int>,
    dag::undirected_edge<int>, dag::undirected_hyperedge<int>,
    (dag::directed_temporal_edge<int, int>),
    (dag::directed_temporal_hyperedge<int, int>),
    (dag::directed_delayed_temporal_edge<int, int>),
    (dag::directed_delayed_temporal_hyperedge<int, int>)) {
  STATIC_REQUIRE(dag::exact_component<dag::component<TestType>>);
}

TEMPLATE_TEST_CASE("component sketch complies with the concept",
    "[dag::component_sketch]",
    dag::directed_edge<int>, dag::directed_hyperedge<int>,
    dag::undirected_edge<int>, dag::undirected_hyperedge<int>,
    (dag::directed_temporal_edge<int, int>),
    (dag::directed_temporal_hyperedge<int, int>),
    (dag::directed_delayed_temporal_edge<int, int>),
    (dag::directed_delayed_temporal_hyperedge<int, int>)) {
  STATIC_REQUIRE(dag::network_component<dag::component_sketch<TestType>>);
}

TEST_CASE("component properties", "[dag::component]") {
  using EdgeType = dag::directed_edge<int>;
  using CompType = dag::component<EdgeType>;

  STATIC_REQUIRE(dag::network_component<CompType>);

  CompType comp(0, 0);

  EdgeType a(1, 2), b(2, 0), c(2, 3);

  comp.insert(a);
  comp.insert(b);

  SECTION("correct basic properties") {
    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2}));
    REQUIRE(comp.contains(0));
    REQUIRE(comp.contains(1));
    REQUIRE(comp.contains(2));
    REQUIRE(comp.size() == 3);
  }

  SECTION("insertion") {
    comp.insert(c);

    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }

  SECTION("merging") {
    CompType comp2(0, 0);

    comp2.insert(c);
    comp.merge(comp2);

    REQUIRE(std::unordered_set<int>(comp.begin(), comp.end()) ==
        std::unordered_set<int>({0, 1, 2, 3}));
    REQUIRE(comp.size() == 4);
  }
}
