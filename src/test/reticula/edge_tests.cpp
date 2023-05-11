#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include <reticula/network_concepts.hpp>
#include <reticula/static_edges.hpp>
#include <reticula/static_hyperedges.hpp>

TEST_CASE("undirected edges", "[reticula::undirected_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2\n3 4\n5 6\n");
    reticula::undirected_edge<int> a{}, b{}, c{};
    s >> a >> b >> c;
    REQUIRE(a == reticula::undirected_edge<int>(1, 2));
    REQUIRE(b == reticula::undirected_edge<int>(3, 4));
    REQUIRE(c == reticula::undirected_edge<int>(5, 6));
  }

  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::undirected_edge<int>>);
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::undirected_edge<std::size_t>>);
  }

  SECTION("have correct properties") {
    reticula::undirected_edge<int> edge(1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedEquals(std::vector<int>({1, 2})));

    REQUIRE(edge.is_incident(1));

    REQUIRE(edge.is_incident(2));

    REQUIRE_FALSE(edge.is_incident(3));

    REQUIRE(reticula::adjacent(edge, reticula::undirected_edge<int>(2, 3)));
    REQUIRE(reticula::adjacent(edge, reticula::undirected_edge<int>(3, 2)));
    REQUIRE_FALSE(reticula::adjacent(
          edge, reticula::undirected_edge<int>(3, 4)));

    REQUIRE_NOTHROW(reticula::hash<reticula::undirected_edge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<reticula::undirected_edge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::undirected_edge<int>>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::undirected_edge<int>(1, 2) ==
        reticula::undirected_edge<int>(1, 2));
    REQUIRE(reticula::undirected_edge<int>(1, 2) ==
        reticula::undirected_edge<int>(2, 1));
    REQUIRE(reticula::undirected_edge<int>(1, 2) !=
        reticula::undirected_edge<int>(2, 3));
  }
}

TEST_CASE("undirected hyperedges", "[reticula::undirected_hyperedge]") {
  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::undirected_hyperedge<int>>);
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::undirected_hyperedge<std::size_t>>);
  }

  SECTION("have correct properties") {
    reticula::undirected_hyperedge<int> edge({1, 2, 3});

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

    REQUIRE(reticula::adjacent(edge, edge));
    REQUIRE(reticula::adjacent(edge,
          reticula::undirected_hyperedge<int>({2, 3, 4})));
    REQUIRE(reticula::adjacent(edge,
          reticula::undirected_hyperedge<int>({3, 4, 5})));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_hyperedge<int>({4, 5})));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::undirected_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::undirected_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::undirected_hyperedge<int>>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::undirected_hyperedge<int>({1, 2, 3}) ==
        reticula::undirected_hyperedge<int>({1, 2, 3}));
    REQUIRE(reticula::undirected_hyperedge<int>({1, 2, 3}) ==
        reticula::undirected_hyperedge<int>({3, 1, 2, 3}));
    REQUIRE(reticula::undirected_hyperedge<int>({1, 2, 3}) !=
        reticula::undirected_hyperedge<int>({2, 3}));
  }
}


TEST_CASE("directed edges", "[reticula::directed_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2\n3 4\n5 6\n");
    reticula::directed_edge<int> a{}, b{}, c{};
    s >> a >> b >> c;
    REQUIRE(a == reticula::directed_edge<int>(1, 2));
    REQUIRE(b == reticula::directed_edge<int>(3, 4));
    REQUIRE(c == reticula::directed_edge<int>(5, 6));
  }

  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::directed_edge<int>>);
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::directed_edge<std::size_t>>);
  }

  SECTION("have correct properties") {
    reticula::directed_edge<int> edge(1, 2);

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

    REQUIRE(reticula::adjacent(edge, reticula::directed_edge<int>(2, 3)));
    REQUIRE_FALSE(reticula::adjacent(edge, reticula::directed_edge<int>(3, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge, reticula::directed_edge<int>(3, 4)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::directed_edge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::directed_edge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::directed_edge<int>>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::directed_edge<int>(1, 2) ==
        reticula::directed_edge<int>(1, 2));
    REQUIRE(reticula::directed_edge<int>(1, 2) !=
        reticula::directed_edge<int>(2, 1));
    REQUIRE(reticula::directed_edge<int>(1, 2) !=
        reticula::directed_edge<int>(2, 3));
  }
}

TEST_CASE("directed hyperedges", "[reticula::directed_hyperedge]") {
  SECTION("comply with static_network_edge concept") {
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::directed_hyperedge<int>>);
    STATIC_REQUIRE(reticula::static_network_edge<
        reticula::directed_hyperedge<std::size_t>>);
  }

  SECTION("have correct properties") {
    reticula::directed_hyperedge<int> edge({1, 2}, {2, 3, 4});

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

    REQUIRE(reticula::adjacent(edge,
          reticula::directed_hyperedge<int>({2, 3, 4}, {5, 6})));
    REQUIRE(reticula::adjacent(edge,
          reticula::directed_hyperedge<int>({4, 5}, {5, 6})));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_hyperedge<int>({5}, {5, 6})));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::directed_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::directed_hyperedge<int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::directed_hyperedge<int>>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        reticula::directed_hyperedge<int>({1, 2}, {2, 3}));
    REQUIRE(reticula::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        reticula::directed_hyperedge<int>({1, 2}, {3, 2}));
    REQUIRE(reticula::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        reticula::directed_hyperedge<int>({2, 1}, {2, 3}));
    REQUIRE(reticula::directed_hyperedge<int>({1, 2}, {2, 3}) ==
        reticula::directed_hyperedge<int>({2, 1}, {3, 2}));

    REQUIRE(reticula::directed_hyperedge<int>({1, 2}, {2, 3}) !=
        reticula::directed_hyperedge<int>({2, 3}, {1, 2}));
    REQUIRE(reticula::directed_hyperedge<int>({1, 2}, {2, 3}) !=
        reticula::directed_hyperedge<int>({2, 3}, {4, 5}));
  }
}

#include <reticula/temporal_edges.hpp>
#include <reticula/temporal_hyperedges.hpp>

TEST_CASE("undirected temporal edges",
    "[reticula::undirected_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3\n4 5 6\n7 8 9\n");
    reticula::undirected_temporal_edge<int, int> a{}, b{}, c{};
    s >> a >> b >> c;
    REQUIRE(a == reticula::undirected_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(b == reticula::undirected_temporal_edge<int, int>(4, 5, 6));
    REQUIRE(c == reticula::undirected_temporal_edge<int, int>(7, 8, 9));
  }

  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::undirected_temporal_edge<int, int>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::undirected_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::undirected_temporal_edge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    reticula::undirected_temporal_edge<int, int> edge(1, 2, 1);

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

    REQUIRE(reticula::adjacent(edge,
          reticula::undirected_temporal_edge<int, int>(2, 3, 2)));
    REQUIRE(reticula::adjacent(edge,
          reticula::undirected_temporal_edge<int, int>(3, 2, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_temporal_edge<int, int>(3, 4, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_temporal_edge<int, int>(1, 2, 1)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_temporal_edge<int, int>(1, 2, 0)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::undirected_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::undirected_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::undirected_temporal_edge<int, int>>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::undirected_temporal_edge<int, int>(1, 2, 3) ==
        reticula::undirected_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(reticula::undirected_temporal_edge<int, int>(1, 2, 3) ==
        reticula::undirected_temporal_edge<int, int>(2, 1, 3));
    REQUIRE(reticula::undirected_temporal_edge<int, int>(1, 2, 3) !=
        reticula::undirected_temporal_edge<int, int>(1, 2, 2));
    REQUIRE(reticula::undirected_temporal_edge<int, int>(1, 2, 2) <
        reticula::undirected_temporal_edge<int, int>(1, 2, 3));
  }
}

TEST_CASE("undirected temporal hyperedges",
    "[reticula::undirected_temporal_hyperedge]") {
  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::undirected_temporal_hyperedge<int, int>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::undirected_temporal_hyperedge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::undirected_temporal_hyperedge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    reticula::undirected_temporal_hyperedge<int, int> edge({1, 2, 3}, 1);

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

    REQUIRE(reticula::adjacent(edge,
          reticula::undirected_temporal_hyperedge<int, int>({2, 3, 4}, 2)));
    REQUIRE(reticula::adjacent(edge,
          reticula::undirected_temporal_hyperedge<int, int>({3, 4}, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_temporal_hyperedge<int, int>({4, 5}, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 1)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 0)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::undirected_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::undirected_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::undirected_temporal_hyperedge<int, int>>{}(
            edge, std::uint64_t{}));

    reticula::undirected_temporal_hyperedge<std::string, int>
      edge2({"hello", "world", "!!"}, 2);
    REQUIRE_NOTHROW(reticula::hash<
        reticula::undirected_temporal_hyperedge<
          std::string, int>>{}(edge2));
    REQUIRE_NOTHROW(std::hash<
        reticula::undirected_temporal_hyperedge<
          std::string, int>>{}(edge2));
    REQUIRE_NOTHROW(hll::hash<
        reticula::undirected_temporal_hyperedge<
          std::string, int>>{}(edge2, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) ==
        reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3));
    REQUIRE(reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) ==
        reticula::undirected_temporal_hyperedge<int, int>({2, 1, 3}, 3));
    REQUIRE(reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) !=
        reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 2));
    REQUIRE(reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) !=
        reticula::undirected_temporal_hyperedge<int, int>({1, 2, 4}, 3));
    REQUIRE(reticula::undirected_temporal_hyperedge<int, int>({1, 2, 3}, 3) !=
        reticula::undirected_temporal_hyperedge<int, int>({1, 2}, 3));
    REQUIRE(reticula::undirected_temporal_hyperedge<int, int>({1, 2}, 2) <
        reticula::undirected_temporal_hyperedge<int, int>({1, 2}, 3));
  }
}

TEST_CASE("directed temporal edges", "[reticula::directed_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3\n4 5 6\n7 8 9\n");
    reticula::directed_temporal_edge<int, int> a{}, b{}, c{};
    s >> a >> b >> c;
    REQUIRE(a == reticula::directed_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(b == reticula::directed_temporal_edge<int, int>(4, 5, 6));
    REQUIRE(c == reticula::directed_temporal_edge<int, int>(7, 8, 9));
  }

  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_temporal_edge<int, int>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_temporal_edge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    reticula::directed_temporal_edge<int, int> edge(1, 2, 1);

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

    REQUIRE(reticula::adjacent(edge,
          reticula::directed_temporal_edge<int, int>(2, 3, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_edge<int, int>(2, 3, 1)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_edge<int, int>(2, 3, 0)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_edge<int, int>(3, 2, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_edge<int, int>(3, 4, 2)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::directed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::directed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::directed_temporal_edge<int, int>>{}(edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::directed_temporal_edge<int, int>(1, 2, 3) ==
        reticula::directed_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(reticula::directed_temporal_edge<int, int>(1, 2, 3) !=
        reticula::directed_temporal_edge<int, int>(2, 1, 3));
    REQUIRE(reticula::directed_temporal_edge<int, int>(1, 2, 3) !=
        reticula::directed_temporal_edge<int, int>(1, 2, 2));
    REQUIRE(reticula::directed_temporal_edge<int, int>(1, 2, 2) <
        reticula::directed_temporal_edge<int, int>(1, 2, 3));
  }
}

TEST_CASE("directed temporal hyperedges",
    "[reticula::directed_temporal_hyperedge]") {
  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_temporal_hyperedge<int, int>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_temporal_hyperedge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_temporal_hyperedge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    reticula::directed_temporal_hyperedge<int, int> edge({1, 2}, {2, 3}, 1);

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

    REQUIRE(reticula::adjacent(edge,
          reticula::directed_temporal_hyperedge<int, int>({2, 4}, {5, 6}, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_hyperedge<int, int>({2, 4}, {5, 6}, 1)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_hyperedge<int, int>({2, 4}, {5, 6}, 0)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_hyperedge<int, int>({4, 5}, {6, 7}, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_temporal_hyperedge<int, int>({4, 5}, {1, 2}, 2)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::directed_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::directed_temporal_hyperedge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::directed_temporal_hyperedge<int, int>>{}(
            edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3) ==
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {3, 2}, 3));
    REQUIRE(
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3) !=
        reticula::directed_temporal_hyperedge<int, int>({2, 3}, {1, 2}, 3));
    REQUIRE(
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3) !=
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 2));
    REQUIRE(
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 2) <
        reticula::directed_temporal_hyperedge<int, int>({1, 2}, {2, 3}, 3));
  }
}

TEST_CASE("directed delayed temporal edges",
    "[reticula::directed_delayed_temporal_edge]") {
  SECTION("are read correctly") {
    std::istringstream s("1 2 3 4\n5 6 7 8\n9 10 11 12\n");
    reticula::directed_delayed_temporal_edge<int, int> a{}, b{}, c{};
    s >> a >> b >> c;
    REQUIRE(a ==
        reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(b ==
        reticula::directed_delayed_temporal_edge<int, int>(5, 6, 7, 8));
    REQUIRE(c ==
        reticula::directed_delayed_temporal_edge<int, int>(9, 10, 11, 12));
  }

  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_delayed_temporal_edge<int, int>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_delayed_temporal_edge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_delayed_temporal_edge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    reticula::directed_delayed_temporal_edge<int, int> edge(1, 2, 1, 2);

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

    REQUIRE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(2, 3, 3, 4)));
    REQUIRE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(2, 3, 3, 15)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(2, 3, 1, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(2, 3, 2, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(2, 3, 2, 7)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(2, 3, 0, 1)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(3, 2, 3, 4)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_edge<int, int>(3, 4, 3, 4)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::directed_delayed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::directed_delayed_temporal_edge<int, int>>{}(edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::directed_delayed_temporal_edge<int, int>>{}(
          edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) ==
        reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) !=
        reticula::directed_delayed_temporal_edge<int, int>(2, 1, 3, 4));
    REQUIRE(reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) !=
        reticula::directed_delayed_temporal_edge<int, int>(1, 1, 3, 3));
    REQUIRE(reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4) !=
        reticula::directed_delayed_temporal_edge<int, int>(1, 2, 2, 4));
    REQUIRE(reticula::directed_delayed_temporal_edge<int, int>(1, 2, 2, 3) <
        reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 3) <
        reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
  }
}

TEST_CASE("directed delayed temporal hyperedges",
    "[reticula::directed_delayed_temporal_hyperedge]") {
  SECTION("comply with temporal_network_edge concept") {
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_delayed_temporal_hyperedge<int, int>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_delayed_temporal_hyperedge<std::size_t, std::size_t>>);
    STATIC_REQUIRE(reticula::temporal_network_edge<
      reticula::directed_delayed_temporal_hyperedge<std::size_t, double>>);
  }

  SECTION("have correct properties") {
    reticula::directed_delayed_temporal_hyperedge<int, int>
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

    REQUIRE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 3, 4)));
    REQUIRE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 3, 15)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 1, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 2, 2)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {2, 7}, {3, 7}, 0, 1)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {3, 7}, {2, 7}, 3, 4)));
    REQUIRE_FALSE(reticula::adjacent(edge,
          reticula::directed_delayed_temporal_hyperedge<int, int>(
            {3, 7}, {4, 8}, 3, 4)));

    REQUIRE_NOTHROW(reticula::hash<
        reticula::directed_delayed_temporal_hyperedge<int, int>>{}(
          edge));
    REQUIRE_NOTHROW(std::hash<
        reticula::directed_delayed_temporal_hyperedge<int, int>>{}(
          edge));
    REQUIRE_NOTHROW(hll::hash<
        reticula::directed_delayed_temporal_hyperedge<int, int>>{}(
          edge, std::uint64_t{}));
  }

  SECTION("compare correctly") {
    REQUIRE(reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) ==
        reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4));
    REQUIRE(reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) !=
        reticula::directed_delayed_temporal_hyperedge<int, int>(
          {2, 7}, {1, 4}, 3, 4));
    REQUIRE(reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) !=
        reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {1, 4}, 3, 3));
    REQUIRE(reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 4) !=
        reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 2, 4));
    REQUIRE(reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 2, 6) <
        reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 7));
    REQUIRE(reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 5) <
        reticula::directed_delayed_temporal_hyperedge<int, int>(
          {1, 4}, {2, 7}, 3, 7));
  }
}

TEST_CASE("higher-order edges", "[reticula::directed_edge]") {
  using HyperEventGraphEdge =
    reticula::directed_edge<
      reticula::directed_temporal_hyperedge<int, float>>;
  STATIC_REQUIRE(reticula::static_network_edge<HyperEventGraphEdge>);
  REQUIRE_NOTHROW(HyperEventGraphEdge({{}, {}}));
  REQUIRE_NOTHROW(reticula::hash<HyperEventGraphEdge>{}(
        HyperEventGraphEdge({{}, {}})));
  REQUIRE_NOTHROW(std::hash<HyperEventGraphEdge>{}(
        HyperEventGraphEdge({{}, {}})));
  REQUIRE_NOTHROW(hll::hash<HyperEventGraphEdge>{}(
        HyperEventGraphEdge({{}, {}}), std::uint64_t{}));

  using DirectedEventGraphEdge =
    reticula::directed_edge<
      reticula::undirected_temporal_edge<std::string, float>>;
  STATIC_REQUIRE(reticula::static_network_edge<DirectedEventGraphEdge>);
}
