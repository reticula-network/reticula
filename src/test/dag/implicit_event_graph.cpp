#include <vector>
#include <cmath>

#include "catch.hpp"
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include "../../../include/implicit_event_graph.hpp"

TEST_CASE("deterministic adjacency probability",
    "[dag::adjacency_prob::deterministic]") {
  using EdgeType = dag::directed_temporal_edge<int, int>;
  EdgeType a(1, 2, 3), b(2, 3, 4), c(2, 3, 5);
  dag::adjacency_prob::deterministic<EdgeType> prob1(2);
  dag::adjacency_prob::deterministic<EdgeType> prob2(3);

  REQUIRE(prob1.p(a, b) == 1.0);
  REQUIRE(prob2.p(a, b) == 1.0);

  REQUIRE(prob1.p(a, c) == 0.0);
  REQUIRE(prob2.p(a, c) == 1.0);

  REQUIRE(prob1.p(c, a) == 0.0);
}

TEST_CASE("exponential adjacency probability",
    "[dag::adjacency_prob::exponential]") {
  using EdgeType = dag::directed_temporal_edge<int, int>;
  EdgeType a(1, 2, 3), b(2, 3, 4), c(2, 3, 5);
  dag::adjacency_prob::exponential<EdgeType> prob1(1);
  dag::adjacency_prob::exponential<EdgeType> prob2(2);

  REQUIRE(prob1.p(a, b) == Approx(std::exp(-1.0/1.0)));
  REQUIRE(prob2.p(a, b) == Approx(1.0/2.0*std::exp(-1.0/2.0)));

  REQUIRE(prob1.p(a, c) == Approx(std::exp(-2.0/1.0)));
  REQUIRE(prob2.p(a, c) == Approx(1.0/2.0*std::exp(-2.0/2.0)));

  REQUIRE(prob1.p(c, a) == 0.0);
}

TEST_CASE("implicit event graphs", "[dag::implicit_event_graph]") {
  SECTION("handle duplicate and unordered event list") {
    using EdgeType = dag::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
        {2, 1, 2}, {2, 1, 2}, {1, 2, 5}};

    dag::adjacency_prob::deterministic<EdgeType> prob(3);
    dag::implicit_event_graph<EdgeType,
      dag::adjacency_prob::deterministic<EdgeType>>
        eg(event_list, prob, (size_t)0);
    REQUIRE(eg.event_count() == 5);
    REQUIRE(eg.node_count()  == 4);
    REQUIRE_THAT(eg.topo(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
  }

  SECTION("when given one") {
    using EdgeType = dag::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};

    dag::adjacency_prob::deterministic<EdgeType> prob(6);
    dag::implicit_event_graph<EdgeType,
      dag::adjacency_prob::deterministic<EdgeType>>
        eg(event_list, prob, (size_t)0);

    SECTION("has correct properties") {
      int t1, t2;
      std::tie(t1, t2) = eg.time_window();
      REQUIRE(t1 == 1);
      REQUIRE(t2 == 8);

      REQUIRE(eg.event_count() == 5);
      REQUIRE(eg.node_count()  == 4);

      REQUIRE_THAT(eg.topo(), Equals(event_list));
    }

    SECTION("successors are calculated correctly") {
      REQUIRE_THAT(eg.successors(EdgeType(2, 1, 2)),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 5)})));

      REQUIRE_THAT(eg.successors(EdgeType(2, 1, 2), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 5)})));

      REQUIRE_THAT(eg.successors(EdgeType(1, 2, 1)),
          UnorderedEquals(
            std::vector<EdgeType>({EdgeType(2, 1, 2), EdgeType(2, 3, 6)})));

      REQUIRE_THAT(eg.successors(EdgeType(1, 2, 1), true),
          Equals(std::vector<EdgeType>({EdgeType(2, 1, 2)})));
    }

    SECTION("predecessors are calculated correctly") {
      REQUIRE_THAT(eg.predecessors(EdgeType(2, 3, 6)),
          UnorderedEquals(
            std::vector<EdgeType>({EdgeType(1, 2, 5), EdgeType(1, 2, 1)})));

      REQUIRE_THAT(eg.predecessors(EdgeType(2, 3, 6), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 5)})));

      REQUIRE_THAT(eg.predecessors(EdgeType(1, 2, 5)),
          Equals(std::vector<EdgeType>({EdgeType(2, 1, 2)})));

      REQUIRE_THAT(eg.predecessors(EdgeType(1, 2, 5), true),
          Equals(std::vector<EdgeType>({EdgeType(2, 1, 2)})));

    }
  }
}
