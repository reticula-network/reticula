#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/network_concepts.hpp>
#include <reticula/static_edges.hpp>
#include <reticula/static_hyperedges.hpp>
#include <reticula/temporal_edges.hpp>
#include <reticula/temporal_hyperedges.hpp>

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
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));

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
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));

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
        RangeEquals(std::vector<int>({2})));
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(edge.mutator_verts(),
        RangeEquals(std::vector<int>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));

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
        UnorderedRangeEquals(std::vector<int>({2, 3, 4})));
    REQUIRE_THAT(edge.heads(),
        UnorderedRangeEquals(std::vector<int>({2, 3, 4})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.tails(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3, 4})));

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

  SECTION("can be constructed from static projection") {
    reticula::undirected_edge<int> static_edge(1, 2);
    reticula::undirected_temporal_edge<int, int> te(static_edge, 3);

    REQUIRE(te == reticula::undirected_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(te.cause_time() == 3);
    REQUIRE(te.effect_time() == 3);
    REQUIRE_THAT(te.mutated_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(te.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(te.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
  }

  SECTION("have correct properties") {
    reticula::undirected_temporal_edge<int, int> edge(1, 2, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
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

  SECTION("can be constructed from static projection") {
    reticula::undirected_hyperedge<int> static_edge({1, 2, 3, 4});
    reticula::undirected_temporal_hyperedge<int, int> te(static_edge, 3);

    REQUIRE(te == reticula::undirected_temporal_hyperedge<int, int>(
        {1, 2, 3, 4}, 3));
    REQUIRE(te.cause_time() == 3);
    REQUIRE(te.effect_time() == 3);
    REQUIRE_THAT(te.mutated_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3, 4})));
    REQUIRE_THAT(te.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3, 4})));
    REQUIRE_THAT(te.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3, 4})));
  }


  SECTION("have correct properties") {
    reticula::undirected_temporal_hyperedge<int, int> edge({1, 2, 3}, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
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

  SECTION("can be constructed from static projection") {
    reticula::directed_edge<int> static_edge(1, 2);
    reticula::directed_temporal_edge<int, int> te(static_edge, 3);
    REQUIRE(te == reticula::directed_temporal_edge<int, int>(1, 2, 3));
    REQUIRE(te.cause_time() == 3);
    REQUIRE(te.effect_time() == 3);
    REQUIRE_THAT(te.mutated_verts(),
        RangeEquals(std::vector<int>({2})));
    REQUIRE_THAT(te.mutator_verts(),
        RangeEquals(std::vector<int>({1})));
    REQUIRE_THAT(te.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
  }

  SECTION("have correct properties") {
    reticula::directed_temporal_edge<int, int> edge(1, 2, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        RangeEquals(std::vector<int>({2})));
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(edge.mutator_verts(),
        RangeEquals(std::vector<int>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));

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

  SECTION("can be constructed from static projection") {
    reticula::directed_hyperedge<int> static_edge({1, 2}, {2, 3});
    reticula::directed_temporal_hyperedge<int, int> te(static_edge, 3);

    REQUIRE(te == reticula::directed_temporal_hyperedge<int, int>(
        {1, 2}, {2, 3}, 3));
    REQUIRE(te.cause_time() == 3);
    REQUIRE(te.effect_time() == 3);
    REQUIRE_THAT(te.mutated_verts(),
        UnorderedRangeEquals(std::vector<int>({2, 3})));
    REQUIRE_THAT(te.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(te.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
  }

  SECTION("have correct properties") {
    reticula::directed_temporal_hyperedge<int, int> edge({1, 2}, {2, 3}, 1);

    REQUIRE_THAT(edge.mutated_verts(),
        UnorderedRangeEquals(std::vector<int>({2, 3})));
    REQUIRE_THAT(edge.heads(),
        UnorderedRangeEquals(std::vector<int>({2, 3})));
    REQUIRE_THAT(edge.mutator_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.tails(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2, 3})));

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

  SECTION("can be constructed from static projection") {
    reticula::directed_edge<int> static_edge(1, 2);
    reticula::directed_delayed_temporal_edge<int, int> te(static_edge, 3, 4);

    REQUIRE(te ==
        reticula::directed_delayed_temporal_edge<int, int>(1, 2, 3, 4));
    REQUIRE(te.cause_time() == 3);
    REQUIRE(te.effect_time() == 4);
    REQUIRE_THAT(te.mutated_verts(),
        RangeEquals(std::vector<int>({2})));
    REQUIRE_THAT(te.mutator_verts(),
        RangeEquals(std::vector<int>({1})));
    REQUIRE_THAT(te.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));
  }

  SECTION("have correct properties") {
    reticula::directed_delayed_temporal_edge<int, int> edge(1, 2, 1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        RangeEquals(std::vector<int>({2})));
    REQUIRE(edge.head() == 2);
    REQUIRE_THAT(edge.mutator_verts(),
        RangeEquals(std::vector<int>({1})));
    REQUIRE(edge.tail() == 1);
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 2})));

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

  SECTION("can be constructed from static projection") {
    reticula::directed_hyperedge<int> static_edge({1, 5, 2}, {2, 6});
    reticula::directed_delayed_temporal_hyperedge<int, int> te(
        static_edge, 1, 2);

    REQUIRE(te ==
        reticula::directed_delayed_temporal_hyperedge<int, int>(
            {1, 5, 2}, {2, 6}, 1, 2));
    REQUIRE(te.cause_time() == 1);
    REQUIRE(te.effect_time() == 2);
    REQUIRE_THAT(te.mutated_verts(),
        RangeEquals(std::vector<int>({2, 6})));
    REQUIRE_THAT(te.mutator_verts(),
        RangeEquals(std::vector<int>({1, 2, 5})));
    REQUIRE_THAT(te.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 5, 2, 6})));
  }

  SECTION("have correct properties") {
    reticula::directed_delayed_temporal_hyperedge<int, int>
      edge({1, 5}, {2, 6}, 1, 2);

    REQUIRE_THAT(edge.mutated_verts(),
        RangeEquals(std::vector<int>({2, 6})));
    REQUIRE_THAT(edge.heads(),
        RangeEquals(std::vector<int>({2, 6})));
    REQUIRE_THAT(edge.mutator_verts(),
        RangeEquals(std::vector<int>({1, 5})));
    REQUIRE_THAT(edge.tails(),
        RangeEquals(std::vector<int>({1, 5})));
    REQUIRE_THAT(edge.incident_verts(),
        UnorderedRangeEquals(std::vector<int>({1, 5, 2, 6})));

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

TEST_CASE(
  "uniform constructor - directed edges",
  "[reticula::uniform_const][reticula::directed_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2};
    std::vector<int> v2 = {1};
    REQUIRE(
      reticula::directed_edge<int>{reticula::uniform_const, v1, v2} ==
      reticula::directed_edge<int>{2, 1});

    REQUIRE(
      reticula::directed_edge<int>{reticula::uniform_const, v2, v1} ==
      reticula::directed_edge<int>{1, 2});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {5};
    REQUIRE(
      reticula::directed_edge<int>{reticula::uniform_const, v, v} ==
      reticula::directed_edge<int>{5, 5});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;

    REQUIRE_THROWS_AS(
      reticula::directed_edge<int>(reticula::uniform_const, empty_range, empty_range),
      std::invalid_argument);
  }

  SECTION("too long input ranges throw exception") {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {3};

    REQUIRE_THROWS_AS(
      reticula::directed_edge<int>(reticula::uniform_const, v1, v2),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      reticula::directed_edge<int>(reticula::uniform_const, v2, v1),
      std::invalid_argument);
  }
}

TEST_CASE(
  "uniform constructor - undirected edges",
  "[reticula::uniform_const][reticula::undirected_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 1};
    REQUIRE(
      reticula::undirected_edge<int>{reticula::uniform_const, v1, v1} ==
      reticula::undirected_edge<int>{2, 1});

    std::vector<int> v2 = {1, 2};
    REQUIRE(
      reticula::undirected_edge<int>{reticula::uniform_const, v2, v2} ==
      reticula::undirected_edge<int>{2, 1});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {5};
    REQUIRE(
      reticula::undirected_edge<int>{reticula::uniform_const, v, v} ==
      reticula::undirected_edge<int>{5, 5});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;

    REQUIRE_THROWS_AS(
      reticula::undirected_edge<int>(reticula::uniform_const, empty_range, empty_range),
      std::invalid_argument);
  }

  SECTION("too long input ranges throw exception") {
    std::vector<int> v1 = {1, 2, 3};

    REQUIRE_THROWS_AS(
      reticula::undirected_edge<int>(reticula::uniform_const, v1, v1),
      std::invalid_argument);
  }

  SECTION("unequal ranges throw exception") {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {2, 3};

    REQUIRE_THROWS_AS(
      reticula::undirected_edge<int>(reticula::uniform_const, v1, v2),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      reticula::undirected_edge<int>(reticula::uniform_const, v2, v1),
      std::invalid_argument);
  }
}

TEST_CASE(
  "uniform constructor - directed hyperedges",
  "[reticula::uniform_const][reticula::directed_hyperedge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 3, 4};
    std::vector<int> v2 = {1, 2, 3, 4};
    REQUIRE(
      reticula::directed_hyperedge<int>{reticula::uniform_const, v1, v2} ==
      reticula::directed_hyperedge<int>{v1, v2});

    REQUIRE(
      reticula::directed_hyperedge<int>{reticula::uniform_const, v2, v1} ==
      reticula::directed_hyperedge<int>{v2, v1});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {1, 2, 3};
    REQUIRE(
      reticula::directed_hyperedge<int>{reticula::uniform_const, v, v} ==
      reticula::directed_hyperedge<int>{v, v});
  }

  SECTION("empty ranges") {
    std::vector<int> v;
    REQUIRE(
      reticula::directed_hyperedge<int>{reticula::uniform_const, v, v} ==
      reticula::directed_hyperedge<int>{v, v});
  }
}

TEST_CASE(
  "uniform constructor - undirected hyperedges",
  "[reticula::uniform_const][reticula::undirected_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 1, 3, 4};
    REQUIRE(
      reticula::undirected_hyperedge<int>{reticula::uniform_const, v1, v1} ==
      reticula::undirected_hyperedge<int>{v1});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;

    REQUIRE_THROWS_AS(
      reticula::undirected_hyperedge<int>(
        reticula::uniform_const, empty_range, empty_range),
      std::invalid_argument);
  }

  SECTION("unequal ranges throw exception") {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {2, 3, 4};

    REQUIRE_THROWS_AS(
      reticula::undirected_hyperedge<int>(reticula::uniform_const, v1, v2),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      reticula::undirected_hyperedge<int>(reticula::uniform_const, v2, v1),
      std::invalid_argument);
  }
}


TEST_CASE(
  "uniform constructor - directed temporal edges",
  "[reticula::uniform_const][reticula::directed_tempotal_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2};
    std::vector<int> v2 = {1};
    int t = 1;
    REQUIRE(
      reticula::directed_temporal_edge<int, int>{
        reticula::uniform_const, v1, v2, t, t} ==
      reticula::directed_temporal_edge<int, int>{2, 1, t});

    REQUIRE(
      reticula::directed_temporal_edge<int, int>{
        reticula::uniform_const, v2, v1, t, t} ==
      reticula::directed_temporal_edge<int, int>{1, 2, t});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {5};
    int t = 1;
    REQUIRE(
      reticula::directed_temporal_edge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::directed_temporal_edge<int, int>{5, 5, t});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;
    int t = 1;
    REQUIRE_THROWS_AS(
      (reticula::directed_temporal_edge<int, int>(
        reticula::uniform_const, empty_range, empty_range, t, t)),
      std::invalid_argument);
  }

  SECTION("too long input ranges throw exception") {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {3};
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::directed_temporal_edge<int, int>(
        reticula::uniform_const, v1, v2, t, t)),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      (reticula::directed_temporal_edge<int, int>(
        reticula::uniform_const, v2, v1, t, t)),
      std::invalid_argument);
  }

  SECTION("unequal times throw exception") {
    std::vector<int> v = {1, 2};
    int t1 = 1;
    int t2 = 2;

    REQUIRE_THROWS_AS(
      (reticula::directed_temporal_edge<int, int>(
        reticula::uniform_const, v, v, t1, t2)),
      std::invalid_argument);
  }
}

TEST_CASE(
  "uniform constructor - directed delayed temporal edges",
  "[reticula::uniform_const][reticula::directed_delayed_tempotal_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2};
    std::vector<int> v2 = {1};
    int t = 1;
    REQUIRE(
      reticula::directed_delayed_temporal_edge<int, int>{
        reticula::uniform_const, v1, v2, t, t} ==
      reticula::directed_delayed_temporal_edge<int, int>{2, 1, t, t});

    REQUIRE(
      reticula::directed_delayed_temporal_edge<int, int>{
        reticula::uniform_const, v2, v1, t, t} ==
      reticula::directed_delayed_temporal_edge<int, int>{1, 2, t, t});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {5};
    int t = 1;
    REQUIRE(
      reticula::directed_delayed_temporal_edge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::directed_delayed_temporal_edge<int, int>{5, 5, t, t});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;
    int t = 1;
    REQUIRE_THROWS_AS(
      (reticula::directed_delayed_temporal_edge<int, int>(
        reticula::uniform_const, empty_range, empty_range, t, t)),
      std::invalid_argument);
  }

  SECTION("too long input ranges throw exception") {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {3};
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::directed_delayed_temporal_edge<int, int>(
        reticula::uniform_const, v1, v2, t, t)),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      (reticula::directed_delayed_temporal_edge<int, int>(
        reticula::uniform_const, v2, v1, t, t)),
      std::invalid_argument);
  }

  SECTION("unequal times") {
    std::vector<int> v = {1, 2};
    int t1 = 1;
    int t2 = 2;

    REQUIRE(
      (reticula::directed_delayed_temporal_edge<int, int>(
        reticula::uniform_const, v, v, t1, t2)) ==
      reticula::directed_delayed_temporal_edge<int, int>{1, 2, t1, t2});
  }
}

TEST_CASE(
  "uniform constructor - undirected temporal edges",
  "[reticula::uniform_const][reticula::undirected_temporal_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 1};
    int t = 1;

    REQUIRE(
      (reticula::undirected_temporal_edge<int, int>{
        reticula::uniform_const, v1, v1, t, t}) ==
      reticula::undirected_temporal_edge<int, int>{2, 1, t});

    std::vector<int> v2 = {1, 2};
    REQUIRE(
      reticula::undirected_temporal_edge<int, int>{
        reticula::uniform_const, v2, v2, t, t} ==
      reticula::undirected_temporal_edge<int, int>{2, 1, t});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {5};
    int t = 1;

    REQUIRE(
      reticula::undirected_temporal_edge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::undirected_temporal_edge<int, int>{5, 5, t});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_edge<int, int>(
        reticula::uniform_const, empty_range, empty_range, t, t)),
      std::invalid_argument);
  }

  SECTION("too long input ranges throw exception") {
    std::vector<int> v1 = {1, 2, 3};
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_edge<int, int>(
        reticula::uniform_const, v1, v1, t, t)),
      std::invalid_argument);
  }

  SECTION("unequal ranges throw exception") {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {2, 3};
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_edge<int, int>(
        reticula::uniform_const, v1, v2, t, t)),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_edge<int, int>(
        reticula::uniform_const, v2, v1, t, t)),
      std::invalid_argument);
  }

  SECTION("unequal times throw exception") {
    std::vector<int> v = {1, 2};
    int t1 = 1;
    int t2 = 2;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_edge<int, int>(
        reticula::uniform_const, v, v, t1, t2)),
      std::invalid_argument);
  }
}

TEST_CASE(
  "uniform constructor - directed temporal hyperedges",
  "[reticula::uniform_const][reticula::directed_temporal_hyperedge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 3, 4};
    std::vector<int> v2 = {1, 2, 3, 4};
    int t = 1;

    REQUIRE(
      reticula::directed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v1, v2, t, t} ==
      reticula::directed_temporal_hyperedge<int, int>{v1, v2, t});

    REQUIRE(
      reticula::directed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v2, v1, t, t} ==
      reticula::directed_temporal_hyperedge<int, int>{v2, v1, t});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {1, 2, 3};
    int t = 1;

    REQUIRE(
      reticula::directed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::directed_temporal_hyperedge<int, int>{v, v, t});
  }

  SECTION("empty ranges") {
    std::vector<int> v;
    int t = 1;

    REQUIRE(
      reticula::directed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::directed_temporal_hyperedge<int, int>{v, v, t});
  }

  SECTION("unequal times throw exception") {
    std::vector<int> v = {1, 2};
    int t1 = 1;
    int t2 = 2;

    REQUIRE_THROWS_AS(
      (reticula::directed_temporal_hyperedge<int, int>(
        reticula::uniform_const, v, v, t1, t2)),
      std::invalid_argument);
  }
}

TEST_CASE(
  "uniform constructor - directed delayed temporal hyperedges",
  "[reticula::uniform_const][reticula::directed_delayed_temporal_hyperedge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 3, 4};
    std::vector<int> v2 = {1, 2, 3, 4};
    int t = 1;

    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v1, v2, t, t} ==
      reticula::directed_delayed_temporal_hyperedge<int, int>{v1, v2, t, t});

    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v2, v1, t, t} ==
      reticula::directed_delayed_temporal_hyperedge<int, int>{v2, v1, t, t});
  }

  SECTION("same vertex in both ranges") {
    std::vector<int> v = {1, 2, 3};
    int t = 1;

    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::directed_delayed_temporal_hyperedge<int, int>{v, v, t, t});
  }

  SECTION("empty ranges") {
    std::vector<int> v;
    int t = 1;

    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v, v, t, t} ==
      reticula::directed_delayed_temporal_hyperedge<int, int>{v, v, t, t});
  }

  SECTION("unequal times") {
    std::vector<int> v = {1, 2};
    int t1 = 1;
    int t2 = 2;

    REQUIRE(
      reticula::directed_delayed_temporal_hyperedge<int, int>{
        reticula::uniform_const, v, v, t1, t2} ==
      reticula::directed_delayed_temporal_hyperedge<int, int>{v, v, t1, t2});
  }
}

TEST_CASE(
  "uniform constructor - undirected temporal hyperedges",
  "[reticula::uniform_const][reticula::undirected_edge]") {
  SECTION("basic construction") {
    std::vector<int> v1 = {2, 1, 3, 4};
    int t = 1;

    REQUIRE(
      reticula::undirected_temporal_hyperedge<int, int>{
        reticula::uniform_const, v1, v1, t, t} ==
      reticula::undirected_temporal_hyperedge<int, int>{v1, t});
  }

  SECTION("empty ranges throw exception") {
    std::vector<int> empty_range;
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_hyperedge<int, int>(
        reticula::uniform_const, empty_range, empty_range, t, t)),
      std::invalid_argument);
  }

  SECTION("unequal ranges throw exception") {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {2, 3, 4};
    int t = 1;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_hyperedge<int, int>(
        reticula::uniform_const, v1, v2, t, t)),
      std::invalid_argument);

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_hyperedge<int, int>(
        reticula::uniform_const, v2, v1, t, t)),
      std::invalid_argument);
  }

  SECTION("unequal times throw exception") {
    std::vector<int> v = {1, 2};
    int t1 = 1;
    int t2 = 2;

    REQUIRE_THROWS_AS(
      (reticula::undirected_temporal_hyperedge<int, int>(
        reticula::uniform_const, v, v, t1, t2)),
      std::invalid_argument);
  }
}
