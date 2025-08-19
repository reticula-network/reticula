#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/generation/gnp.hpp>

TEST_CASE("Random G(n, p) graph", "[reticula::random_gnp_graph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 100000;

  SECTION("empty") {
    auto g1 = reticula::random_gnp_graph(n, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(n));

    auto g2 = reticula::random_gnp_graph(0, 1, gen);
    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("randomness") {
    double p = 0.00005;
    reticula::undirected_network r = reticula::random_gnp_graph(n, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = static_cast<double>(n) * static_cast<double>(n - 1) / 2 * p;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3 * sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3 * sigma);
  }

  SECTION("edge probability") {
    double p = 0.4;
    reticula::VertexType n = 15;
    std::unordered_map<reticula::undirected_edge, std::size_t> n_edges;
    n_edges.reserve(n * (n - 1) / 2);
    std::size_t n_trials = 300;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::undirected_network r = reticula::random_gnp_graph(n, p, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n) / static_cast<double>(n_trials);
      REQUIRE(p_est > p * 0.75);
      REQUIRE(p_est < p * 1.25);
    }
  }
}

TEST_CASE(
  "Random directed G(n, p) graph", "[reticula::random_directed_gnp_graph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 100000;

  SECTION("empty") {
    auto g1 = reticula::random_directed_gnp_graph(n, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(n));

    auto g2 = reticula::random_directed_gnp_graph(0, 1, gen);
    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("randomness") {
    double p = 0.00005;
    reticula::directed_network r =
      reticula::random_directed_gnp_graph(n, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = static_cast<double>(n) * static_cast<double>(n - 1) * p;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3 * sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3 * sigma);
  }

  SECTION("edge probability") {
    double p = 0.4;
    reticula::VertexType n = 15;
    std::unordered_map<reticula::directed_edge, std::size_t> n_edges;
    n_edges.reserve(n * n);
    std::size_t n_trials = 300;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::directed_network r =
        reticula::random_directed_gnp_graph(n, p, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n) / static_cast<double>(n_trials);
      REQUIRE(p_est > p * 0.75);
      REQUIRE(p_est < p * 1.25);
    }
  }
}
