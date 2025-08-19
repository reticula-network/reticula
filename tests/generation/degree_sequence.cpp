#include <catch2/catch_test_macros.hpp>

#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/generation/degree_sequence.hpp>
#include <reticula/networks.hpp>

TEST_CASE(
  "random degree sequence graph", "[reticula::random_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::degree_sequence_graph(
      std::vector<reticula::VertexType>(n, 0), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 =
      reticula::degree_sequence_graph(std::vector<reticula::VertexType>{}, gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("produces the degree sequence we wanted with no self-loop") {
    std::size_t n = 20;
    reticula::VertexType w = 3;

    std::vector<reticula::VertexType> degree_sequence(n, w);

    auto g = reticula::degree_sequence_graph(degree_sequence, gen);

    std::vector<reticula::VertexType> degrees;
    for (auto v : g.vertices())
      degrees.push_back(g.degree(v));

    REQUIRE_THAT(degrees, UnorderedRangeEquals(degree_sequence));

    REQUIRE(std::ranges::none_of(g.edges(), [](const auto& e) {
      return e.incident_verts().size() == 1;
    }));
  }

  SECTION("produces the degree sequence exactly") {
    std::vector<reticula::VertexType> degree_sequence = {2, 2, 3, 1, 1,
                                                         1, 1, 1, 2, 0};
    auto g = reticula::degree_sequence_graph(degree_sequence, gen);

    std::vector<reticula::VertexType> degrees;
    for (auto v : g.vertices())
      degrees.push_back(g.degree(v));

    REQUIRE_THAT(degrees, UnorderedRangeEquals(degree_sequence));

    REQUIRE(std::ranges::none_of(g.edges(), [](const auto& e) {
      return e.incident_verts().size() == 1;
    }));
  }

  // TODO: test uniformity
}

TEST_CASE(
  "random directed degree sequence graph",
  "[reticula::random_directed_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::directed_degree_sequence_graph(
      std::vector<std::pair<reticula::VertexType, reticula::VertexType>>(
        n, {0, 0}),
      gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::directed_degree_sequence_graph(
      std::vector<std::pair<reticula::VertexType, reticula::VertexType>>(),
      gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("produces the degree sequence we wanted with no self-loop") {
    std::size_t n = 20;
    reticula::VertexType w = 3;

    std::vector<std::pair<reticula::VertexType, reticula::VertexType>>
      degree_sequence(n, {w, w});

    auto g = reticula::directed_degree_sequence_graph(degree_sequence, gen);

    std::vector<std::pair<reticula::VertexType, reticula::VertexType>> degrees;
    for (auto v : g.vertices())
      degrees.emplace_back(g.in_degree(v), g.out_degree(v));

    REQUIRE_THAT(degrees, UnorderedRangeEquals(degree_sequence));

    REQUIRE(std::ranges::none_of(g.edges(), [](const auto& e) {
      return e.head() == e.tail();
    }));
  }

  SECTION("produces the degree sequence exactly") {
    std::vector<std::pair<reticula::VertexType, reticula::VertexType>>
      degree_sequence = {{2, 0}, {2, 2}, {3, 2}, {2, 3}, {2, 0},
                         {3, 3}, {1, 2}, {2, 2}, {0, 3}, {3, 3}};
    auto g = reticula::directed_degree_sequence_graph(degree_sequence, gen);

    std::vector<std::pair<reticula::VertexType, reticula::VertexType>> degrees;
    for (auto v : g.vertices())
      degrees.emplace_back(g.in_degree(v), g.out_degree(v));

    REQUIRE_THAT(degrees, UnorderedRangeEquals(degree_sequence));

    REQUIRE(std::ranges::none_of(g.edges(), [](const auto& e) {
      return e.head() == e.tail();
    }));
  }

  // TODO: test uniformity
}
