#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/generation/uniform_hypergraph.hpp>

TEST_CASE(
  "Random uniform hypergraph", "[reticula::random_uniform_hypergraph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 10000;
  reticula::VertexType m = 3;

  SECTION("empty") {
    auto g1 = reticula::random_uniform_hypergraph(n, m, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::random_uniform_hypergraph(0, 0, 1, gen);
    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("edge sizes") {
    double p = 1e-7;
    reticula::undirected_hypernetwork r =
      reticula::random_uniform_hypergraph(n, m, p, gen);
    for (auto&& e : r.edges())
      REQUIRE(e.incident_verts().size() == m);
  }

  SECTION("randomness") {
    double p = 1e-7;
    reticula::undirected_hypernetwork r =
      reticula::random_uniform_hypergraph(n, m, p, gen);

    REQUIRE(r.vertices().size() == n);

    double mean = 16661.66; // comb(n, m)*p
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3 * sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3 * sigma);
  }

  SECTION("edge probability") {
    double p = 0.25;
    reticula::VertexType n = 20;
    reticula::VertexType m = 3;
    std::unordered_map<reticula::undirected_hyperedge, std::size_t> n_edges;
    std::size_t n_trials = 500;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::undirected_hypernetwork r =
        reticula::random_uniform_hypergraph(n, m, p, gen);
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
  "Random directed uniform hypergraph",
  "[reticula::random_directed_uniform_hypergraph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 500;
  reticula::VertexType m1 = 3;
  reticula::VertexType m2 = 4;

  SECTION("empty") {
    auto g1 = reticula::random_directed_uniform_hypergraph(n, m1, m2, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(n));

    auto g2 = reticula::random_directed_uniform_hypergraph(0, 0, 0, 1, gen);
    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("edge sizes") {
    double p = 2e-13;
    reticula::directed_hypernetwork r =
      reticula::random_directed_uniform_hypergraph(n, m1, m2, p, gen);
    for (auto&& e : r.edges()) {
      REQUIRE(e.mutator_verts().size() == static_cast<std::size_t>(m1));
      REQUIRE(e.mutated_verts().size() == static_cast<std::size_t>(m2));
    }
  }

  SECTION("randomness") {
    double p = 2e-13;
    reticula::directed_hypernetwork r =
      reticula::random_directed_uniform_hypergraph(n, m1, m2, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = 10656.72;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3 * sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3 * sigma);
  }

  SECTION("edge probability") {
    double p = 0.25;
    reticula::VertexType n = 5;
    reticula::VertexType m1 = 3, m2 = 2;
    std::unordered_map<reticula::directed_hyperedge, std::size_t> n_edges;
    std::size_t n_trials = 500;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::directed_hypernetwork r =
        reticula::random_directed_uniform_hypergraph(n, m1, m2, p, gen);
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
