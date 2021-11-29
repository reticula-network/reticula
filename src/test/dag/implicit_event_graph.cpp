#include <vector>

#include <catch2/catch.hpp>
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include <dag/implicit_event_graph.hpp>
#include <dag/adjacency_prob.hpp>

TEST_CASE("implicit event graphs", "[dag::implicit_event_graph]") {
  SECTION("handle duplicate and unordered event list") {
    using EdgeType = dag::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
        {2, 1, 2}, {2, 1, 2}, {1, 2, 5}};

    dag::adjacency_prob::deterministic<EdgeType> prob(3);
    dag::implicit_event_graph<EdgeType,
      dag::adjacency_prob::deterministic<EdgeType>>
        eg(event_list, prob, 0ul);
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
  }

  SECTION("when given direct successors test cases") {
    using EdgeType = dag::undirected_temporal_edge<int, double>;
    std::vector<EdgeType> list1{
      {1, 2, 0.0}, {1, 2, 1.0}, {2, 3, 2.0}, {0, 1, 3.0}};

    std::vector<EdgeType> list2{
      {1, 2, 0.0}, {2, 3, 1.0}, {1, 2, 2.0}, {0, 1, 3.0}};

    std::vector<EdgeType> list3{
      {1, 2, 0.0}, {2, 3, 1.0}, {0, 1, 2.0}, {1, 2, 3.0}};

    SECTION("successors are calculated correctly for dt=0.5") {
      dag::adjacency_prob::deterministic<EdgeType> prob(0.5);

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg1(list1, prob, 0ul);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>()));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg2(list2, prob, 0ul);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>()));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg3(list3, prob, 0ul);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>()));
    }

    SECTION("successors are calculated correctly for dt=1.5") {
      dag::adjacency_prob::deterministic<EdgeType> prob(1.5);

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg1(list1, prob, 0ul);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 1.0)})));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg2(list2, prob, 0ul);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(2, 3, 1.0)})));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg3(list3, prob, 0ul);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0)),
          Equals(std::vector<EdgeType>({EdgeType(2, 3, 1.0)})));
    }

    SECTION("successors are calculated correctly for dt=2.5") {
      dag::adjacency_prob::deterministic<EdgeType> prob(2.5);

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg1(list1, prob, 0ul);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 1.0)})));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg2(list2, prob, 0ul);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(1, 2, 2.0)})));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg3(list3, prob, 0ul);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(0, 1, 2.0)})));
    }

    SECTION("successors are calculated correctly for dt=3.5") {
      dag::adjacency_prob::deterministic<EdgeType> prob(3.5);

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg1(list1, prob, 0ul);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 1.0)})));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg2(list2, prob, 0ul);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(1, 2, 2.0)})));

      dag::implicit_event_graph<EdgeType,
        dag::adjacency_prob::deterministic<EdgeType>>
          eg3(list3, prob, 0ul);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(0, 1, 2.0)})));
    }
  }

  SECTION("when given one") {
    using EdgeType = dag::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};
    dag::adjacency_prob::deterministic<EdgeType> prob(6);
    dag::implicit_event_graph<EdgeType,
      dag::adjacency_prob::deterministic<EdgeType>>
        eg(event_list, prob, 0ul);


    SECTION("has correct properties") {
      int t1, t2;
      std::tie(t1, t2) = eg.time_window();
      REQUIRE(t1 == 1);
      REQUIRE(t2 == 8);

      REQUIRE(eg.seed() == 0ul);

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

      REQUIRE_THAT(eg.successors(EdgeType(2, 1, 2)),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 5)})));
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
