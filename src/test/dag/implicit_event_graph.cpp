#include <vector>

#include "catch.hpp"
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include "../../../include/dag/implicit_event_graph.hpp"
#include "../../../include/dag/adjacency_prob.hpp"

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
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
    REQUIRE_THAT(eg.events_cause(),
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

      REQUIRE_THAT(eg.events_cause(), Equals(event_list));
      REQUIRE_THAT(eg.events_effect(), Equals(event_list));
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
