#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::AllMatch;
using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/algorithms/vertex_degree.hpp>
#include <reticula/concepts.hpp>
#include <reticula/generation/deterministic.hpp>

TEST_CASE("path graph", "[reticula::path_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::path_graph(0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("single vertex") {
    auto graph = reticula::path_graph(1);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("single vertex periodic") {
    auto graph = reticula::path_graph(1, true);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("aperiodic path") {
    auto graph = reticula::path_graph(5);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1}, {1, 2}, {2, 3}, {3, 4}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }

  SECTION("periodic path") {
    auto graph = reticula::path_graph(5, true);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("cycle graph", "[reticula::cycle_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::cycle_graph(0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("single vertex") {
    auto graph = reticula::cycle_graph(1);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("cycle of size 5") {
    auto graph = reticula::cycle_graph(5);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("star graph", "[reticula::star_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::star_graph(0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("single vertex") {
    auto graph = reticula::star_graph(1);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("star of size 5") {
    auto graph = reticula::star_graph(5);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1}, {0, 2}, {0, 3}, {0, 4}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("wheel graph", "[reticula::wheel_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::wheel_graph(0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("single vertex") {
    auto graph = reticula::wheel_graph(1);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("wheel of size 5") {
    auto graph = reticula::wheel_graph(5);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>({
                         {0, 1},
                         {0, 2},
                         {0, 3},
                         {0, 4}, // center to outer vertices
                         {1, 2},
                         {2, 3},
                         {3, 4},
                         {4, 1} // outer cycle
                       })));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("complete bipartite graph", "[reticula::complete_bipartite_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::complete_bipartite_graph(0, 0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("bipartite graph with one part") {
    auto graph = reticula::complete_bipartite_graph(3, 0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{3})));
  }

  SECTION("bipartite graph with two parts") {
    auto graph = reticula::complete_bipartite_graph(2, 3);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 2}, {0, 3}, {0, 4}, {1, 2}, {1, 3}, {1, 4}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("complete graph", "[reticula::complete_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::complete_graph(0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("single vertex") {
    auto graph = reticula::complete_graph(1);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("complete graph of size 5") {
    auto graph = reticula::complete_graph(5);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1},
                          {0, 2},
                          {0, 3},
                          {0, 4},
                          {1, 2},
                          {1, 3},
                          {1, 4},
                          {2, 3},
                          {2, 4},
                          {3, 4}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE(
  "complete bipartite directed graph",
  "[reticula::complete_bipartite_directed_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::complete_bipartite_directed_graph(0, 0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("bipartite graph with one part") {
    auto graph = reticula::complete_bipartite_directed_graph(3, 0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{3})));
  }

  SECTION("bipartite directed graph with two parts") {
    auto graph = reticula::complete_bipartite_directed_graph(2, 3);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::directed_edge>(
                         {{0, 2}, {0, 3}, {0, 4}, {1, 2}, {1, 3}, {1, 4}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("complete directed graph", "[reticula::complete_directed_graph]") {
  SECTION("empty graph") {
    auto graph = reticula::complete_directed_graph(0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE(std::ranges::empty(graph.vertices()));
  }

  SECTION("single vertex") {
    auto graph = reticula::complete_directed_graph(1);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{1})));
  }

  SECTION("complete directed graph of size 5") {
    auto graph = reticula::complete_directed_graph(5);
    REQUIRE_THAT(
      graph.edges(),
      UnorderedRangeEquals(
        std::vector<reticula::directed_edge>(
          {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 0}, {1, 2}, {1, 3},
           {1, 4}, {2, 0}, {2, 1}, {2, 3}, {2, 4}, {3, 0}, {3, 1},
           {3, 2}, {3, 4}, {4, 0}, {4, 1}, {4, 2}, {4, 3}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }
}

TEST_CASE("regular ring lattice", "[reticula::regular_ring_lattice]") {
  SECTION("empty graph") {
    REQUIRE_THROWS_AS(
      reticula::regular_ring_lattice(0, 2), std::invalid_argument);
  }

  SECTION("single vertex") {
    auto graph = reticula::regular_ring_lattice(1, 0);
    REQUIRE(std::ranges::empty(graph.edges()));
    REQUIRE_THAT(
      graph.vertices(), RangeEquals(std::vector<reticula::VertexType>({0})));
  }

  SECTION("ring lattice of size 5 with degree 2") {
    auto graph = reticula::regular_ring_lattice(5, 2);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{5})));
  }

  SECTION("large regular ring lattice") {
    std::size_t n = 1000, k = 4;
    REQUIRE_THROWS_AS(
      reticula::regular_ring_lattice(n, k + 1), std::invalid_argument);

    reticula::undirected_network ring = reticula::regular_ring_lattice(n, k);

    REQUIRE_THAT(
      reticula::degree_sequence(ring),
      RangeEquals(std::vector<reticula::VertexType>(n, k)));
  }
}

TEST_CASE("square grid graph", "[reticula::square_grid_graph]") {
  SECTION("empty graph") {
    auto graph0 = reticula::square_grid_graph({0});
    auto graph1 = reticula::square_grid_graph({0});
    auto graph2 = reticula::square_grid_graph({0, 0});
    REQUIRE(std::ranges::empty(graph0.edges()));
    REQUIRE(std::ranges::empty(graph0.vertices()));
    REQUIRE(graph0 == graph1);
    REQUIRE(graph0 == graph2);
  }

  SECTION("2x2 grid") {
    auto graph = reticula::square_grid_graph({2, 2});
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1}, {0, 2}, {1, 3}, {2, 3}})));
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(reticula::VertexType{}, reticula::VertexType{4})));
  }

  SECTION("2x2 periodic grid") {
    auto graph = reticula::square_grid_graph({2, 3}, true);
    REQUIRE_THAT(
      graph.edges(), UnorderedRangeEquals(
                       std::vector<reticula::undirected_edge>(
                         {{0, 1},
                          {0, 2},
                          {0, 3},
                          {1, 2},
                          {1, 4},
                          {2, 5},
                          {3, 4},
                          {3, 5},
                          {4, 5}})));
  }

  SECTION("3x3x3x3 periodic grid") {
    reticula::VertexType k = 3;
    auto graph = reticula::square_grid_graph({k, k, k, k}, true);
    REQUIRE_THAT(
      graph.vertices(),
      RangeEquals(
        std::views::iota(
          reticula::VertexType{}, reticula::VertexType{k * k * k * k})));
    REQUIRE_THAT(
      reticula::degree_sequence(graph),
      RangeEquals(std::vector<reticula::VertexType>(k * k * k * k, 4 * 2)));
  }
}
