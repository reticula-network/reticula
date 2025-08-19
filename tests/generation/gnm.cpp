#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/generation/gnm.hpp>

TEST_CASE("Random G(n, m) graph", "[reticula::random_gnm_graph]") {

  std::mt19937_64 gen(42);

  SECTION("empty") {
    auto g1 = reticula::random_gnm_graph(10, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(10));

    REQUIRE_THROWS_AS(
      reticula::random_gnm_graph(0, 1, gen), std::invalid_argument);

    REQUIRE_THROWS_AS(
      reticula::random_gnm_graph(5, 100, gen), std::invalid_argument);
  }

  SECTION("edge count") {
    reticula::VertexType n = 1000;
    reticula::VertexType m = 10000;

    auto g1 = reticula::random_gnm_graph(n, m, gen);
    REQUIRE(g1.edges().size() == static_cast<std::size_t>(m));
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(n));
  }

  SECTION("edge probability") {
    reticula::VertexType n = 12;
    reticula::VertexType m = 30;
    std::unordered_map<reticula::undirected_edge, std::size_t> n_edges;
    std::size_t n_trials = 400;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::undirected_network r = reticula::random_gnm_graph(n, m, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    double p = static_cast<double>(m) / static_cast<double>(n * (n - 1) / 2);

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n) / static_cast<double>(n_trials);
      REQUIRE(p_est > p * 0.75);
      REQUIRE(p_est < p * 1.25);
    }
  }
}

TEST_CASE(
  "Random directed G(n, m) graph", "[retica::random_directed_gnm_graph]") {

  std::mt19937_64 gen(42);

  SECTION("empty") {
    auto g1 = reticula::random_directed_gnm_graph(10, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(10));

    REQUIRE_THROWS_AS(
      reticula::random_directed_gnm_graph(0, 1, gen), std::invalid_argument);

    REQUIRE_THROWS_AS(
      reticula::random_directed_gnm_graph(5, 100, gen), std::invalid_argument);
  }

  SECTION("edge count") {
    reticula::VertexType n = 1000;
    reticula::VertexType m = 10000;

    auto g1 = reticula::random_directed_gnm_graph(n, m, gen);
    REQUIRE(g1.edges().size() == static_cast<std::size_t>(m));
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(n));
  }

  SECTION("edge probability") {
    reticula::VertexType n = 12;
    reticula::VertexType m = 50;
    std::unordered_map<reticula::directed_edge, std::size_t> n_edges;
    std::size_t n_trials = 400;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::directed_network r =
        reticula::random_directed_gnm_graph(n, m, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    double p = static_cast<double>(m) / static_cast<double>(n * (n - 1));

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n) / static_cast<double>(n_trials);
      REQUIRE(p_est > p * 0.75);
      REQUIRE(p_est < p * 1.25);
    }
  }
}
