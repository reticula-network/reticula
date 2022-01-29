#include <sstream>
#include <string>

#include <catch2/catch.hpp>
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include <dag/network_concepts.hpp>
#include <dag/static_edges.hpp>
#include <dag/static_hyperedges.hpp>

TEST_CASE("undirected edges", "[dag::undirected_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2\n3 4\n5 6\n");
    dag::undirected_edge<int> a{}, b{}, c{};
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
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));

    REQUIRE(edge.is_incident(1));

    REQUIRE(edge.is_incident(2));

    REQUIRE_FALSE(edge.is_incident(3));

    REQUIRE(dag::adjacent(edge, dag::undirected_edge<int>(2, 3)));
    REQUIRE(dag::adjacent(edge, dag::undirected_edge<int>(3, 2)));
    REQUIRE_FALSE(dag::adjacent(edge, dag::undirected_edge<int>(3, 4)));

    REQUIRE_NOTHROW(dag::hash<dag::undirected_edge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<dag::undirected_edge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<dag::undirected_edge<int>>{}(edge, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::undirected_edge<int>(1, 2) == dag::undirected_edge<int>(1, 2));
    REQUIRE(dag::undirected_edge<int>(1, 2) == dag::undirected_edge<int>(2, 1));
    REQUIRE(dag::undirected_edge<int>(1, 2) != dag::undirected_edge<int>(2, 3));
  }
}

TEST_CASE("undirected hyperedges", "[dag::undirected_hyperedge]") {
  SECTION("comply with static_edge concept") {
    STATIC_REQUIRE(dag::static_edge<dag::undirected_hyperedge<int>>);
    STATIC_REQUIRE(dag::static_edge<dag::undirected_hyperedge<std::size_t>>);
  }

  SECTION("have correct properties") {
    dag::undirected_hyperedge<int> edge({1, 2, 3});

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));

    REQUIRE(edge.is_incident(1));
    REQUIRE(edge.is_incident(2));
    REQUIRE(edge.is_incident(3));

    REQUIRE_FALSE(edge.is_incident(4));

    REQUIRE(dag::adjacent(edge, edge));
    REQUIRE(dag::adjacent(edge, dag::undirected_hyperedge<int>({2, 3, 4})));
    REQUIRE(dag::adjacent(edge, dag::undirected_hyperedge<int>({3, 4, 5})));
    REQUIRE_FALSE(dag::adjacent(edge, dag::undirected_hyperedge<int>({4, 5})));

    REQUIRE_NOTHROW(dag::hash<
        dag::undirected_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::undirected_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::undirected_hyperedge<int>>{}(edge, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::undirected_hyperedge<int>({1, 2, 3}) ==
        dag::undirected_hyperedge<int>({1, 2, 3}));
    REQUIRE(dag::undirected_hyperedge<int>({1, 2, 3}) ==
        dag::undirected_hyperedge<int>({3, 1, 2, 3}));
    REQUIRE(dag::undirected_hyperedge<int>({1, 2, 3}) !=
        dag::undirected_hyperedge<int>({2, 3}));
  }
}


TEST_CASE("directed edges", "[dag::directed_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2\n3 4\n5 6\n");
    dag::directed_edge<int> a{}, b{}, c{};
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
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(edge.mutator_verts(),
        Equals(std::vector<int>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(2));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(dag::adjacent(edge, dag::directed_edge<int>(2, 3)));
    REQUIRE_FALSE(dag::adjacent(edge, dag::directed_edge<int>(3, 2)));
    REQUIRE_FALSE(dag::adjacent(edge, dag::directed_edge<int>(3, 4)));

    REQUIRE_NOTHROW(dag::hash<
        dag::directed_edge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::directed_edge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::directed_edge<int>>{}(edge, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_edge<int>(1, 2) == dag::directed_edge<int>(1, 2));
    REQUIRE(dag::directed_edge<int>(1, 2) != dag::directed_edge<int>(2, 1));
    REQUIRE(dag::directed_edge<int>(1, 2) != dag::directed_edge<int>(2, 3));
  }
}

TEST_CASE("directed hyperedges", "[dag::directed_hyperedge]") {
  SECTION("comply with static_edge concept") {
    STATIC_REQUIRE(dag::static_edge<dag::directed_hyperedge<int>>);
    STATIC_REQUIRE(dag::static_edge<dag::directed_hyperedge<std::size_t>>);
  }

  SECTION("have correct properties") {
    dag::directed_hyperedge<int> edge({1, 2}, {2, 3, 4});

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({2, 3, 4})));
    REQUIRE_THAT(edge.heads(),
        UnorderedEquals(std::vector<int>({2, 3, 4})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.tails(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3, 4})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE(edge.is_out_incident(2));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE(edge.is_in_incident(3));
    REQUIRE(edge.is_in_incident(4));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(5));
    REQUIRE_FALSE(edge.is_out_incident(5));

    REQUIRE(dag::adjacent(edge,
          dag::directed_hyperedge<int>({2, 3, 4}, {5, 6})));
    REQUIRE(dag::adjacent(edge,
          dag::directed_hyperedge<int>({4, 5}, {5, 6})));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_hyperedge<int>({5}, {5, 6})));

    REQUIRE_NOTHROW(dag::hash<
        dag::directed_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::directed_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::directed_hyperedge<int>>{}(edge, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        dag::directed_hyperedge<int>({1, 2}, {2, 3}));
    REQUIRE(dag::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        dag::directed_hyperedge<int>({1, 2}, {3, 2}));
    REQUIRE(dag::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        dag::directed_hyperedge<int>({2, 1}, {2, 3}));
    REQUIRE(dag::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        dag::directed_hyperedge<int>({2, 1}, {3, 2}));

    REQUIRE(dag::directed_hyperedge<int>({1, 2}, {2, 3}) !=
        dag::directed_hyperedge<int>({2, 3}, {1, 2}));
    REQUIRE(dag::directed_hyperedge<int>({1, 2}, {2, 3}) !=
        dag::directed_hyperedge<int>({2, 3}, {4, 5}));
  }
}

#include <dag/temporal_edges.hpp>
#include <dag/temporal_hyperedges.hpp>

TEST_CASE("undirected temporal edges",
    "[dag::undirected_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3\n4 5 6\n7 8 9\n");
    dag::undirected_temporal_edge<int, int> a{}, b{}, c{};
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

  SECTION("have correct properties") {
    dag::undirected_temporal_edge<int, int> edge(1, 2, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE(edge.cause_time() == 1);
    REQUIRE(edge.effect_time() == 1);

    REQUIRE(edge.is_incident(1));

    REQUIRE(edge.is_incident(2));

    REQUIRE_FALSE(edge.is_incident(3));

    REQUIRE(dag::adjacent(edge,
          dag::undirected_temporal_edge<int, int>(2, 3, 2)));
    REQUIRE(dag::adjacent(edge,
          dag::undirected_temporal_edge<int, int>(3, 2, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::undirected_temporal_edge<int, int>(3, 4, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::undirected_temporal_edge<int, int>(1, 2, 1)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::undirected_temporal_edge<int, int>(1, 2, 0)));

    REQUIRE_NOTHROW(dag::hash<
        dag::undirected_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::undirected_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::undirected_temporal_edge<int, int>>{}(edge, uint32_t{}));
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

TEST_CASE("undirected temporal hyperedges",
    "[dag::undirected_temporal_hyperedge]") {
  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<
      dag::undirected_temporal_hyperedge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::undirected_temporal_hyperedge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::undirected_temporal_hyperedge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    dag::undirected_temporal_hyperedge<int, int> edge({1, 2, 3}, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));
    REQUIRE(edge.cause_time() == 1);
    REQUIRE(edge.effect_time() == 1);

    REQUIRE(edge.is_incident(1));
    REQUIRE(edge.is_incident(2));
    REQUIRE(edge.is_incident(3));
    REQUIRE_FALSE(edge.is_incident(4));

    REQUIRE(dag::adjacent(edge,
          dag::undirected_temporal_hyperedge<int, int>({2, 3, 4}, 2)));
    REQUIRE(dag::adjacent(edge,
          dag::undirected_temporal_hyperedge<int, int>({3, 4}, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::undirected_temporal_hyperedge<int, int>({4, 5}, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 1)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 0)));

    REQUIRE_NOTHROW(dag::hash<
        dag::undirected_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::undirected_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::undirected_temporal_hyperedge<int, int>>{}(edge, uint32_t{}));

    dag::undirected_temporal_hyperedge<std::string, int>
      edge2({"hello", "world", "!!"}, 2);
    REQUIRE_NOTHROW(dag::hash<
        dag::undirected_temporal_hyperedge<
          std::string, int>>{}(edge2));
    REQUIRE_NOTHROW(std::hash<
        dag::undirected_temporal_hyperedge<
          std::string, int>>{}(edge2));
    REQUIRE_NOTHROW(hll::hash<
        dag::undirected_temporal_hyperedge<
          std::string, int>>{}(edge2, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) ==
        dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3));
    REQUIRE(dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) ==
        dag::undirected_temporal_hyperedge<int, int>({2, 1, 3}, 3));
    REQUIRE(dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) !=
        dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 2));
    REQUIRE(dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) !=
        dag::undirected_temporal_hyperedge<int, int>({1, 2, 4}, 3));
    REQUIRE(dag::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) !=
        dag::undirected_temporal_hyperedge<int, int>({1, 2}, 3));
    REQUIRE(dag::undirected_temporal_hyperedge<int, int>({1, 2}, 2) <
        dag::undirected_temporal_hyperedge<int, int>({1, 2}, 3));
  }
}

TEST_CASE("directed temporal edges", "[dag::directed_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3\n4 5 6\n7 8 9\n");
    dag::directed_temporal_edge<int, int> a{}, b{}, c{};
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

  SECTION("have correct properties") {
    dag::directed_temporal_edge<int, int> edge(1, 2, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        Equals(std::vector<int>({2})));
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(edge.mutator_verts(),
        Equals(std::vector<int>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(2));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(dag::adjacent(edge,
          dag::directed_temporal_edge<int, int>(2, 3, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_edge<int, int>(2, 3, 1)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_edge<int, int>(2, 3, 0)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_edge<int, int>(3, 2, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_edge<int, int>(3, 4, 2)));

    REQUIRE_NOTHROW(dag::hash<
        dag::directed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::directed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::directed_temporal_edge<int, int>>{}(edge, uint32_t{}));
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

TEST_CASE("directed temporal hyperedges",
    "[dag::directed_temporal_hyperedge]") {
  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_temporal_hyperedge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_temporal_hyperedge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_temporal_hyperedge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    dag::directed_temporal_hyperedge<int, int> edge({1, 2}, {2, 3}, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({2, 3})));
    REQUIRE_THAT(edge.heads(),
        UnorderedEquals(std::vector<int>({2, 3})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.tails(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2, 3})));

    REQUIRE(edge.is_out_incident(1));
    REQUIRE(edge.is_out_incident(2));
    REQUIRE_FALSE(edge.is_out_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(4));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_in_incident(1));
    REQUIRE_FALSE(edge.is_in_incident(4));

    REQUIRE(dag::adjacent(edge,
          dag::directed_temporal_hyperedge<int, int>({2, 4}, {5, 6}, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_hyperedge<int, int>({2, 4}, {5, 6}, 1)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_hyperedge<int, int>({2, 4}, {5, 6}, 0)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_hyperedge<int, int>({4, 5}, {6, 7}, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_temporal_hyperedge<int, int>({4, 5}, {1, 2}, 2)));

    REQUIRE_NOTHROW(dag::hash<
        dag::directed_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::directed_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::directed_temporal_hyperedge<int, int>>{}(edge, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3) ==
        dag::directed_temporal_hyperedge<int, int>({1, 2}, {3, 2}, 3));
    REQUIRE(dag::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3) !=
        dag::directed_temporal_hyperedge<int, int>({2, 3}, {1, 2}, 3));
    REQUIRE(dag::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3) !=
        dag::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 2));
    REQUIRE(dag::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 2) <
        dag::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3));
  }
}

TEST_CASE("directed delayed temporal edges",
    "[dag::directed_delayed_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3 4\n5 6 7 8\n9 10 11 12\n");
    dag::directed_delayed_temporal_edge<int, int> a{}, b{}, c{};
    s >> a >> b >> c;
    REQUIRE(a == dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(b == dag::directed_delayed_temporal_edge<int, int>(5, 6, 7, 8));
    REQUIRE(c == dag::directed_delayed_temporal_edge<int, int>(9, 10, 11, 12));
  }

  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_delayed_temporal_edge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_delayed_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_delayed_temporal_edge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    dag::directed_delayed_temporal_edge<int, int> edge(1, 2, 1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        Equals(std::vector<int>({2})));
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(edge.mutator_verts(),
        Equals(std::vector<int>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));

    REQUIRE(edge.cause_time() == 1);
    REQUIRE(edge.effect_time() == 2);

    REQUIRE(edge.is_out_incident(1));
    REQUIRE_FALSE(edge.is_out_incident(2));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE_FALSE(edge.is_in_incident(1));

    REQUIRE_FALSE(edge.is_in_incident(3));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(2, 3, 3, 4)));
    REQUIRE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(2, 3, 3, 15)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(2, 3, 1, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(2, 3, 2, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(2, 3, 2, 7)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(2, 3, 0, 1)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(3, 2, 3, 4)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_edge<int, int>(3, 4, 3, 4)));

    REQUIRE_NOTHROW(dag::hash<
        dag::directed_delayed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        dag::directed_delayed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::directed_delayed_temporal_edge<int, int>>{}(edge, uint32_t{}));
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
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 2, 3) <
        dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 3) <
        dag::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
  }
}

TEST_CASE("directed delayed temporal hyperedges",
    "[dag::directed_delayed_temporal_hyperedge]") {
  SECTION("comply with temporal_edge concept") {
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_delayed_temporal_hyperedge<int, int>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_delayed_temporal_hyperedge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(dag::temporal_edge<
      dag::directed_delayed_temporal_hyperedge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    dag::directed_delayed_temporal_hyperedge<int, int>
      edge({1, 5}, {2, 6}, 1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        Equals(std::vector<int>({2, 6})));
    REQUIRE_THAT(edge.heads(),
        Equals(std::vector<int>({2, 6})));
    REQUIRE_THAT(edge.mutator_verts(),
        Equals(std::vector<int>({1, 5})));
    REQUIRE_THAT(edge.tails(),
        Equals(std::vector<int>({1, 5})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 5, 2, 6})));

    REQUIRE(edge.cause_time() == 1);
    REQUIRE(edge.effect_time() == 2);

    REQUIRE(edge.is_out_incident(1));
    REQUIRE(edge.is_out_incident(5));
    REQUIRE_FALSE(edge.is_out_incident(2));
    REQUIRE_FALSE(edge.is_out_incident(3));

    REQUIRE(edge.is_in_incident(2));
    REQUIRE(edge.is_in_incident(6));
    REQUIRE_FALSE(edge.is_in_incident(1));
    REQUIRE_FALSE(edge.is_in_incident(4));

    REQUIRE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 3, 4)));
    REQUIRE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 3, 15)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 1, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 2, 2)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 0, 1)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {3, 7}, {2, 7}, 3, 4)));
    REQUIRE_FALSE(dag::adjacent(edge,
          dag::directed_delayed_temporal_hyperedge<int, int>(
            {3, 7}, {4, 8}, 3, 4)));

    REQUIRE_NOTHROW(dag::hash<
        dag::directed_delayed_temporal_hyperedge<int, int>>{}(
          edge));
    REQUIRE_NOTHROW(std::hash<
        dag::directed_delayed_temporal_hyperedge<int, int>>{}(
          edge));
    REQUIRE_NOTHROW(hll::hash<
        dag::directed_delayed_temporal_hyperedge<int, int>>{}(
          edge, uint32_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) ==
        dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4));
    REQUIRE(dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) !=
        dag::directed_delayed_temporal_hyperedge<int, int>(
          {2, 7}, {1, 4}, 3, 4));
    REQUIRE(dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) !=
        dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {1, 4}, 3, 3));
    REQUIRE(dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) !=
        dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 2, 4));
    REQUIRE(dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 2, 6) <
        dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 7));
    REQUIRE(dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 5) <
        dag::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 7));
  }
}

TEST_CASE("higher-order edges", "[]") {
  using HyperEventGraphEdge =
    dag::directed_edge<
      dag::directed_temporal_hyperedge<int, float>>;
  REQUIRE_NOTHROW(HyperEventGraphEdge({{}, {}}));
  REQUIRE_NOTHROW(dag::hash<HyperEventGraphEdge>{}(
        HyperEventGraphEdge({{}, {}})));
  REQUIRE_NOTHROW(std::hash<HyperEventGraphEdge>{}(
        HyperEventGraphEdge({{}, {}})));
  REQUIRE_NOTHROW(hll::hash<HyperEventGraphEdge>{}(
        HyperEventGraphEdge({{}, {}}), uint32_t{}));
}
