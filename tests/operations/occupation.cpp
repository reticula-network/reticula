#include "reticula/concepts.hpp"
#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <ranges>
#include <reticula/networks.hpp>
#include <reticula/operations/occupation.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("occupy edges", "[reticula::occupy_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  std::mt19937 gen(42);
  std::unordered_map<EdgeType, double, reticula::hash<EdgeType>> probs = {
    {{{1}, {2, 4}, 1}, 0.0}, {{{2}, {1, 7}, 2}, 1.0}};
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto g1 = reticula::occupy_edges(
    n1,
    [&probs](const EdgeType& e) {
      return probs.contains(e) ? probs.at(e) : 0.0;
    },
    gen);
  auto g2 = reticula::occupy_edges(n1, probs, gen, 0.0);
  REQUIRE_THAT(n1.vertices(), UnorderedRangeEquals(g1.vertices()));
  REQUIRE_THAT(n1.vertices(), UnorderedRangeEquals(g2.vertices()));

  REQUIRE_THAT(
    g1.edges(), UnorderedRangeEquals(std::vector<EdgeType>{{{2}, {1, 7}, 2}}));
  REQUIRE_THAT(
    g2.edges(), UnorderedRangeEquals(std::vector<EdgeType>{{{2}, {1, 7}, 2}}));
}

TEST_CASE("uniformly occupy edges", "[reticula::uniformly_occupy_edges]") {
  reticula::VertexType n = 512;
  std::vector<reticula::undirected_edge> edges;
  edges.reserve(n * (n - 1) / 2);
  for (reticula::VertexType i = 0; i < n; ++i)
    for (reticula::VertexType j = i + 1; j < n; ++j)
      edges.emplace_back(i, j);
  auto g = reticula::undirected_network(
    edges, std::views::iota(reticula::VertexType{}, n));

  std::mt19937_64 gen(42);
  double p = 0.5;
  auto g2 = reticula::uniformly_occupy_edges(g, p, gen);
  double mean = static_cast<double>(n) * static_cast<double>((n - 1) / 2) * p;
  double sigma = std::sqrt(mean);
  REQUIRE(g2.vertices().size() == n);
  REQUIRE(static_cast<double>(g2.edges().size()) > mean - 3 * sigma);
  REQUIRE(static_cast<double>(g2.edges().size()) < mean + 3 * sigma);
}

TEST_CASE("occupy vertices", "[reticula::occupy_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge;
  std::mt19937 gen(42);
  std::unordered_map<reticula::VertexType, double> probs = {
    {1, 1.0}, {2, 1.0}, {7, 1.0}};
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto g1 = reticula::occupy_vertices(
    n1,
    [&probs](reticula::VertexType v) {
      return probs.contains(v) ? probs.at(v) : 0.0;
    },
    gen);
  auto g2 = reticula::occupy_vertices(n1, probs, gen, 0.0);
  REQUIRE_THAT(
    g1.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 7}));
  REQUIRE_THAT(
    g2.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>{1, 2, 7}));

  REQUIRE_THAT(
    g1.edges(), UnorderedRangeEquals(
                  std::vector<EdgeType>{{{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
  REQUIRE_THAT(
    g2.edges(), UnorderedRangeEquals(
                  std::vector<EdgeType>{{{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}

TEST_CASE(
  "uniformly occupy vertices", "[reticula::uniformly_occupy_vertices]") {
  reticula::VertexType n = 512;
  std::vector<reticula::undirected_edge> edges;
  edges.reserve(n * (n - 1) / 2);
  for (reticula::VertexType i = 0; i < n; ++i)
    for (reticula::VertexType j = i + 1; j < n; ++j)
      edges.emplace_back(i, j);
  auto g = reticula::undirected_network(
    edges, std::views::iota(reticula::VertexType{}, n));

  std::mt19937_64 gen(42);
  double p = 0.5;
  auto g2 = reticula::uniformly_occupy_vertices(g, p, gen);
  double mean = static_cast<double>(n) * p;
  double sigma = std::sqrt(mean);
  REQUIRE(static_cast<double>(g2.vertices().size()) > mean - 3 * sigma);
  REQUIRE(static_cast<double>(g2.vertices().size()) < mean + 3 * sigma);
  REQUIRE(
    g2.edges().size() ==
    (g2.vertices().size() * (g2.vertices().size() - 1)) / 2);
}
