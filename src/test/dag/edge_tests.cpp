#include <sstream>
#include <string>

#include "catch.hpp"
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include "../../../include/dag/network_concepts.hpp"
#include "../../../include/dag/static_edges.hpp"

TEST_CASE("undirected edges", "[dag::undirected_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2\n3 4\n5 6\n");
    dag::undirected_edge<int> a, b, c;
    s >> a >> b >> c;
    REQUIRE(a == dag::undirected_edge<int>(1, 2));
    REQUIRE(b == dag::undirected_edge<int>(3, 4));
    REQUIRE(c == dag::undirected_edge<int>(5, 6));
  }

  SECTION("comply with static_edge concept") {
    STATIC_REQUIRE(dag::static_edge<dag::undirected_edge<int>>);
    STATIC_REQUIRE(dag::static_edge<dag::undirected_edge<std::size_t>>);
  }

  SECTION("have correct properties") {
    dag::undirected_edge<int> edge(1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));

    REQUIRE(edge.is_incident(1));

    REQUIRE(edge.is_incident(2));

    REQUIRE_FALSE(edge.is_incident(3));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::undirected_edge<int>(1, 2) == dag::undirected_edge<int>(1, 2));
    REQUIRE(dag::undirected_edge<int>(1, 2) == dag::undirected_edge<int>(2, 1));
    REQUIRE(dag::undirected_edge<int>(1, 2) != dag::undirected_edge<int>(2, 3));
  }
}


TEST_CASE("directed edges", "[directed_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2\n3 4\n5 6\n");
    dag::directed_edge<int> a, b, c;
    s >> a >> b >> c;
    REQUIRE(a == dag::directed_edge<int>(1, 2));
    REQUIRE(b == dag::directed_edge<int>(3, 4));
    REQUIRE(c == dag::directed_edge<int>(5, 6));
  }

  SECTION("comply with static_edge concept") {
    STATIC_REQUIRE(dag::static_edge<dag::directed_edge<int>>);
    STATIC_REQUIRE(dag::static_edge<dag::directed_edge<std::size_t>>);
  }

  SECTION("have correct properties") {
    dag::directed_edge<int> edge(1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        Equals(std::vector<int>({2})));
    REQUIRE_THAT(edge.mutator_verts(),
        Equals(std::vector<int>({1})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(2));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(3));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_edge<int>(1, 2) == dag::directed_edge<int>(1, 2));
    REQUIRE(dag::directed_edge<int>(1, 2) != dag::directed_edge<int>(2, 1));
    REQUIRE(dag::directed_edge<int>(1, 2) != dag::directed_edge<int>(2, 3));
  }
}

#include "../../../include/dag/temporal_edges.hpp"

TEST_CASE("undirected temporal edges", "[undirected_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3\n4 5 6\n7 8 9\n");
    dag::undirected_temporal_edge<int, int> a, b, c;
    s >> a >> b >> c;
    REQUIRE(a == dag::undirected_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(b == dag::undirected_temporal_edge<int, int>(4, 5, 6));
    REQUIRE(c == dag::undirected_temporal_edge<int, int>(7, 8, 9));
  }

  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<
      dag::undirected_temporal_edge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::undirected_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::undirected_temporal_edge<std::size_t, double>>);
  }

  SECTION("compare correctly") {
    REQUIRE(dag::undirected_temporal_edge<int, int>(1, 2, 3) ==
        dag::undirected_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(dag::undirected_temporal_edge<int, int>(1, 2, 3) ==
        dag::undirected_temporal_edge<int, int>(2, 1, 3));
    REQUIRE(dag::undirected_temporal_edge<int, int>(1, 2, 3) !=
        dag::undirected_temporal_edge<int, int>(1, 2, 2));
    REQUIRE(dag::undirected_temporal_edge<int, int>(1, 2, 2) <
        dag::undirected_temporal_edge<int, int>(1, 2, 3));
  }
}

TEST_CASE("directed temporal edges", "[directed_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3\n4 5 6\n7 8 9\n");
    dag::directed_temporal_edge<int, int> a, b, c;
    s >> a >> b >> c;
    REQUIRE(a == dag::directed_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(b == dag::directed_temporal_edge<int, int>(4, 5, 6));
    REQUIRE(c == dag::directed_temporal_edge<int, int>(7, 8, 9));
  }

  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_temporal_edge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_temporal_edge<std::size_t, double>>);
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_temporal_edge<int, int>(1, 2, 3) ==
        dag::directed_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(dag::directed_temporal_edge<int, int>(1, 2, 3) !=
        dag::directed_temporal_edge<int, int>(2, 1, 3));
    REQUIRE(dag::directed_temporal_edge<int, int>(1, 2, 3) !=
        dag::directed_temporal_edge<int, int>(1, 2, 2));
    REQUIRE(dag::directed_temporal_edge<int, int>(1, 2, 2) <
        dag::directed_temporal_edge<int, int>(1, 2, 3));
  }
}

TEST_CASE("directed delayed temporal edges",
    "[directed_delayed_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3 4\n5 6 7 8\n9 10 11 12\n");
    dag::directed_delayed_temporal_edge<int, int> a, b, c;
    s >> a >> b >> c;
    REQUIRE(a == dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(b == dag::directed_delayed_temporal_edge<int, int>(5, 6, 7, 8));
    REQUIRE(c == dag::directed_delayed_temporal_edge<int, int>(9, 10, 11, 12));
  }

  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<dag::directed_delayed_temporal_edge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<dag::directed_delayed_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<dag::directed_delayed_temporal_edge<std::size_t, double>>);
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) ==
        dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) !=
        dag::directed_delayed_temporal_edge<int, int>(2, 1, 3, 4));
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) !=
        dag::directed_delayed_temporal_edge<int, int>(1, 1, 3, 3));
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) !=
        dag::directed_delayed_temporal_edge<int, int>(1, 2, 2, 4));
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 2, 4) <
        dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 2) <
        dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
  }
}
