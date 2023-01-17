#include <vector>
#include <random>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_approx.hpp>

using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Contains;
using Catch::Matchers::Equals;

#include <reticula/networks.hpp>
#include <reticula/components.hpp>
#include <reticula/operations.hpp>
#include <reticula/generators.hpp>

TEST_CASE("cartesian product", "[reticula::cartesian_product]") {
    reticula::undirected_network<int> graph1({{1, 2}, {1, 3}});
    reticula::undirected_network<int> graph2({{1, 2}, {2, 3}});

    reticula::undirected_network<std::pair<int, int>> prod =
      reticula::cartesian_product(graph1, graph2);

    REQUIRE(prod.vertices().size() ==
        graph1.vertices().size()*graph2.vertices().size());
    REQUIRE(prod.edges().size() ==
        graph1.vertices().size()*graph2.edges().size() +
        graph2.vertices().size()*graph1.edges().size());

    REQUIRE_THAT(prod.edges(),
        UnorderedEquals(
          std::vector<reticula::undirected_edge<std::pair<int, int>>>{
            {{1, 1}, {2, 1}}, {{1, 1}, {3, 1}}, {{1, 1}, {1, 2}},
            {{1, 2}, {2, 2}}, {{1, 2}, {3, 2}}, {{1, 2}, {1, 3}},
            {{1, 3}, {2, 3}}, {{1, 3}, {3, 3}},
            {{2, 1}, {2, 2}},
            {{2, 2}, {2, 3}},
            {{3, 1}, {3, 2}},
            {{3, 2}, {3, 3}}}));
}


TEST_CASE("vertex induced subgraph", "[reticula::vertex_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with components") {
    reticula::component<typename EdgeType::VertexType> comp({1, 2, 3, 4, 9});
    auto res = reticula::vertex_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::vertex_induced_subgraph(network, {1, 2, 3, 4, 9});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedEquals(res.edges()));
  }
}

TEST_CASE("edge induced subgraph", "[reticula::edge_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with component") {
    reticula::component<EdgeType> comp({
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});
    auto res = reticula::edge_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::edge_induced_subgraph(network, {
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedEquals(res.edges()));
  }
}

TEST_CASE("graph union", "[reticula::graph_union]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  reticula::network<EdgeType> n2(
      {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = reticula::graph_union(n1, n2);
  auto res2 = reticula::graph_union(n1, n2);
  REQUIRE(res1.edges() == res2.edges());
  REQUIRE_THAT(res1.edges(), Contains(n1.edges()));
  REQUIRE_THAT(res1.edges(), Contains(n2.edges()));

  REQUIRE(res1.vertices() == res2.vertices());
  REQUIRE_THAT(res1.vertices(), Contains(n1.vertices()));
  REQUIRE_THAT(res1.vertices(), Contains(n2.vertices()));
}

TEST_CASE("with edges", "[reticula::with_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_edges(n1, {
      {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2},
          {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}));
}

TEST_CASE("with vertices", "[reticula::with_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_vertices(n1, {3, 4, 5, 6});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 5, 6, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}


TEST_CASE("without edges", "[reticula::without_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::without_edges(n1,
      {{{3}, {3}, 1}, {{3}, {3}, 1}, {{2}, {7, 1}, 2}});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 4, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}));
}

TEST_CASE("without vertices", "[reticula::without_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::without_vertices(n1, {3, 4, 8});
  REQUIRE_THAT(res.vertices(), UnorderedEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 7}));
  REQUIRE_THAT(res.edges(), UnorderedEquals(
        std::vector<EdgeType>{{{7, 1}, {2}, 5}, {{2}, {1, 7}, 2}}));
}

TEST_CASE("occupy edges", "[reticula::occupy_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  std::mt19937 gen(42);
  std::unordered_map<EdgeType, double, reticula::hash<EdgeType>> probs = {
    {{{1}, {2, 4}, 1}, 0.0}, {{{2}, {1, 7}, 2}, 1.0}};
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto g1 = reticula::occupy_edges(n1, [&probs](const EdgeType& e) {
      return probs.contains(e) ? probs.at(e) : 0.0;
    }, gen);
  auto g2 = reticula::occupy_edges(n1, probs, gen, 0.0);
  REQUIRE_THAT(n1.vertices(), UnorderedEquals(g1.vertices()));
  REQUIRE_THAT(n1.vertices(), UnorderedEquals(g2.vertices()));

  REQUIRE_THAT(g1.edges(),
      UnorderedEquals(std::vector<EdgeType>{{{2}, {1, 7}, 2}}));
  REQUIRE_THAT(g2.edges(),
      UnorderedEquals(std::vector<EdgeType>{{{2}, {1, 7}, 2}}));
}

TEST_CASE("uniformly occupy edges", "[reticula::uniformly_occupy_edges]") {
  std::size_t n = 512;
  double p = 0.5;
  auto g = reticula::complete_graph(n);
  std::mt19937_64 gen(42);
  auto g2 = reticula::uniformly_occupy_edges(g, p, gen);
  double mean = static_cast<double>(n)*static_cast<double>((n-1)/2)*p;
  double sigma = std::sqrt(mean);
  REQUIRE(g2.vertices().size() == n);
  REQUIRE(static_cast<double>(g2.edges().size()) > mean - 3*sigma);
  REQUIRE(static_cast<double>(g2.edges().size()) < mean + 3*sigma);
}

TEST_CASE("occupy vertices", "[reticula::occupy_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  std::mt19937 gen(42);
  std::unordered_map<int, double> probs = {
    {1, 1.0}, {2, 1.0}, {7, 1.0}};
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto g1 = reticula::occupy_vertices(n1, [&probs](int v) {
      return probs.contains(v) ? probs.at(v) : 0.0;
    }, gen);
  auto g2 = reticula::occupy_vertices(n1, probs, gen, 0.0);
  REQUIRE_THAT(g1.vertices(), UnorderedEquals(std::vector<int>{1, 2, 7}));
  REQUIRE_THAT(g2.vertices(), UnorderedEquals(std::vector<int>{1, 2, 7}));

  REQUIRE_THAT(g1.edges(),
      UnorderedEquals(std::vector<EdgeType>{
        {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
  REQUIRE_THAT(g2.edges(),
      UnorderedEquals(std::vector<EdgeType>{
        {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}

TEST_CASE("uniformly occupy vertices",
    "[reticula::uniformly_occupy_vertices]") {
  std::size_t n = 512;
  double p = 0.5;
  auto g = reticula::complete_graph(n);
  std::mt19937_64 gen(42);
  auto g2 = reticula::uniformly_occupy_vertices(g, p, gen);
  double mean = static_cast<double>(n)*p;
  double sigma = std::sqrt(mean);
  REQUIRE(static_cast<double>(g2.vertices().size()) > mean - 3*sigma);
  REQUIRE(static_cast<double>(g2.vertices().size()) < mean + 3*sigma);
  REQUIRE(g2.edges().size() ==
      (g2.vertices().size()*(g2.vertices().size()-1))/2);
}

TEST_CASE("mapping", "[reticula::mapping]") {
  STATIC_REQUIRE(reticula::mapping<std::unordered_map<int, double>,
      int, double>);
  STATIC_REQUIRE(reticula::mapping<std::map<int, double>,
      int, double>);
}


TEST_CASE("relabel nodes", "[reticula::relabel_nodes]") {
  // let's make a graph with more to it than integral vertices
  reticula::undirected_network<int> graph1({{1, 2}, {1, 3}});
  reticula::undirected_network<int> graph2({{1, 2}, {1, 3}});
  reticula::undirected_network<std::pair<int, int>> orig =
    reticula::cartesian_product(graph1, graph2);

  reticula::undirected_network<std::size_t> relabeled =
    reticula::relabel_nodes<std::size_t>(orig);

  REQUIRE(orig.vertices().size() == relabeled.vertices().size());
  REQUIRE(orig.edges().size() == relabeled.edges().size());
}
