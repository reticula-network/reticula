#include <cmath>
#include <algorithm>

#include <catch2/catch.hpp>

#include <dag/networks.hpp>
#include <dag/random_networks.hpp>

TEST_CASE("G(n, p) random graph", "[dag::gnp_random_graph]") {
  std::mt19937_64 gen(42);
  std::size_t n = 1000;
  double p = 0.1;
  dag::undirected_network<std::size_t> r =
    dag::gnp_random_graph<std::size_t>(n, p, gen);

  REQUIRE(r.vertices().size() == n);

  double mean = static_cast<double>(n*(n-1)/2)*p;
  double sigma = std::sqrt(mean);
  REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
  REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);
}

TEST_CASE("Barabasi-Albert random graph", "[dag::ba_random_graph]") {
  std::mt19937_64 gen(42);
  std::size_t n = 100, m = 3;
  dag::undirected_network<std::size_t> r =
    dag::ba_random_graph<std::size_t>(n, m, gen);
  REQUIRE(r.vertices().size() == n);
  REQUIRE(r.edges().size() == (n-m)*m);
}

TEST_CASE("random k-regular graph", "[dag::random_regular_graph]") {
  std::mt19937_64 gen(42);
  std::size_t n = 10, k = 4;
  dag::undirected_network<std::size_t> r =
    dag::random_regular_graph<std::size_t>(n, k, gen);
  REQUIRE(r.vertices().size() == n);
  REQUIRE(r.edges().size() == k*n/2);

  auto verts = r.vertices();
  REQUIRE(std::all_of(verts.cbegin(), verts.cend(),
        [&r, k](std::size_t v) { return r.degree(v) == k; }));
}