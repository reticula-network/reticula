#include <vector>

#include <catch2/catch.hpp>
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include <reticula/implicit_event_graphs.hpp>
#include <reticula/temporal_adjacency.hpp>

TEST_CASE("implicit event graphs", "[reticula::implicit_event_graph]") {
  SECTION("handle duplicate and unordered event list") {
    using EdgeType = reticula::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
        {2, 1, 2}, {2, 1, 2}, {1, 2, 5}};

    reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(3);
    reticula::implicit_event_graph<EdgeType,
      reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
        eg(event_list, adj);
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
  }

  SECTION("handle supplemental vert list") {
    using EdgeType = reticula::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
        {2, 1, 2}, {2, 1, 2}, {1, 2, 5}};

    reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(3);
    reticula::implicit_event_graph<EdgeType,
      reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
        eg({{2, 3, 6}, {3, 4, 8}, {1, 2, 1}, {2, 1, 2}, {1, 2, 5}},
            {0}, adj);
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
    REQUIRE_THAT(eg.events_cause(),
        Equals(std::vector<EdgeType>(
            {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));
    REQUIRE_THAT(eg.temporal_net_vertices(),
        Equals(std::vector<typename EdgeType::VertexType>({0, 1, 2, 3, 4})));
  }

  SECTION("when given direct successors test cases") {
    using EdgeType = reticula::undirected_temporal_edge<int, double>;
    std::vector<EdgeType> list1{
      {1, 2, 0.0}, {1, 2, 1.0}, {2, 3, 2.0}, {0, 1, 3.0}};

    std::vector<EdgeType> list2{
      {1, 2, 0.0}, {2, 3, 1.0}, {1, 2, 2.0}, {0, 1, 3.0}};

    std::vector<EdgeType> list3{
      {1, 2, 0.0}, {2, 3, 1.0}, {0, 1, 2.0}, {1, 2, 3.0}};

    SECTION("successors are calculated correctly for dt=0.5") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(0.5);

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg1(list1, adj);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>()));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg2(list2, adj);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>()));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg3(list3, adj);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>()));
    }

    SECTION("successors are calculated correctly for dt=1.5") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(1.5);

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg1(list1, adj);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 1.0)})));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg2(list2, adj);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(2, 3, 1.0)})));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg3(list3, adj);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0)),
          Equals(std::vector<EdgeType>({EdgeType(2, 3, 1.0)})));
    }

    SECTION("successors are calculated correctly for dt=2.5") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(2.5);

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg1(list1, adj);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 1.0)})));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg2(list2, adj);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(1, 2, 2.0)})));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg3(list3, adj);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(0, 1, 2.0)})));
    }

    SECTION("successors are calculated correctly for dt=3.5") {
      reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(3.5);

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg1(list1, adj);
      REQUIRE_THAT(eg1.successors(EdgeType(1, 2, 0.0), true),
          Equals(std::vector<EdgeType>({EdgeType(1, 2, 1.0)})));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg2(list2, adj);
      REQUIRE_THAT(eg2.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(1, 2, 2.0)})));

      reticula::implicit_event_graph<EdgeType,
        reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
          eg3(list3, adj);
      REQUIRE_THAT(eg3.successors(EdgeType(1, 2, 0.0), true),
          UnorderedEquals(
            std::vector<EdgeType>({
              EdgeType(2, 3, 1.0),
              EdgeType(0, 1, 2.0)})));
    }
  }

  SECTION("when given one") {
    using EdgeType = reticula::directed_temporal_edge<int, int>;
    std::vector<EdgeType> event_list{
      {1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}};
    reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj(6);
    reticula::implicit_event_graph<EdgeType,
      reticula::temporal_adjacency::limited_waiting_time<EdgeType>>
        eg(event_list, adj);


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
