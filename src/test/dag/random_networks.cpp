#include <cmath>
#include <algorithm>

#include <catch2/catch.hpp>

#include <dag/networks.hpp>
#include <dag/random_networks.hpp>

TEMPLATE_TEST_CASE("G(n, p) random graph", "[dag::gnp_random_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 1000;
  double p = 0.1;
  dag::undirected_network<TestType> r =
    dag::gnp_random_graph<TestType>(n, p, gen);

  REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

  double mean = static_cast<double>(n*(n-1)/2)*p;
  double sigma = std::sqrt(mean);
  REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
  REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);
}

TEMPLATE_TEST_CASE("Barabasi-Albert random graph", "[dag::ba_random_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 1000, m = 3;
  dag::undirected_network<TestType> r =
    dag::ba_random_graph<TestType>(n, m, gen);
  REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));
  REQUIRE(r.edges().size() == static_cast<std::size_t>((n-m)*m));
}

TEMPLATE_TEST_CASE("random k-regular graph", "[dag::random_regular_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 1000, k = 4;
  dag::undirected_network<TestType> r =
    dag::random_regular_graph<TestType>(n, k, gen);
  REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));
  REQUIRE(r.edges().size() == static_cast<std::size_t>(k*n/2));

  auto verts = r.vertices();
  REQUIRE(std::all_of(verts.cbegin(), verts.cend(),
        [&r, k](TestType v) {
          return r.degree(v) == static_cast<std::size_t>(k);
        }));
}

TEST_CASE("power-law with secified mean",
    "[dag::power_law_with_specified_mean]") {
  REQUIRE(dag::random_number_distribution<
      dag::power_law_with_specified_mean<>>);
}

TEST_CASE("residual power-law with specified mean",
    "[dag::residual_power_law_with_specified_mean]") {
  REQUIRE(dag::random_number_distribution<
      dag::residual_power_law_with_specified_mean<>>);
}

TEST_CASE("hawkes univariate exponential",
    "[dag::hawkes_univariate_exponential]") {
  REQUIRE(dag::random_number_distribution<
      dag::hawkes_univariate_exponential<>>);
}
