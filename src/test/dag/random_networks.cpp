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

TEST_CASE("random expected degree sequence graph",
    "[dag::random_expected_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = dag::random_expected_degree_sequence_graph<std::size_t>(
        std::vector<double>(n, 0.0), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = dag::random_expected_degree_sequence_graph<std::size_t>(
        std::vector<double>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("doesn't have a loop if we don't want it") {
    std::size_t n = 20;
    double w = 3.0;

    auto g = dag::random_expected_degree_sequence_graph<std::size_t>(
        std::vector<double>(n, w), gen, false);
    REQUIRE(std::ranges::none_of(g.edges(),
          [](const auto& e) {
            return e.incident_verts().size() == 1;
          }));
  }

  SECTION("edge existance probability") {
    std::size_t ens = 20'000;
    std::size_t n = 20;

    std::unordered_map<
      std::pair<std::size_t, std::size_t>,
      std::size_t,
      dag::hash<std::pair<std::size_t, std::size_t>>> p;

    SECTION("with self-loops") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = dag::random_expected_degree_sequence_graph<std::size_t>(
            std::ranges::iota_view{std::size_t{}, n}, gen, true);
        for (std::size_t v: g.vertices())
          for (std::size_t u: g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<std::size_t, std::size_t>,
              std::size_t>& kv) {
              auto& [u, v] = kv.first;
              double wu = static_cast<double>(u);
              double wv = static_cast<double>(v);
              double s = static_cast<double>(n)*static_cast<double>(n-1)/2.0;
              double puv = std::min(wu*wv/s, 1.0);
              if (u == v) puv *= 2.0;
              double mean = static_cast<double>(ens)*puv;
              double sigma = std::sqrt(puv*(1-puv)*static_cast<double>(ens));

              return static_cast<double>(kv.second) >= mean - 3*sigma &&
                      static_cast<double>(kv.second) <= mean + 3*sigma;
            }));
    }

    SECTION("without self-loop") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = dag::random_expected_degree_sequence_graph<std::size_t>(
            std::ranges::iota_view{std::size_t{}, n}, gen, false);
        for (std::size_t v: g.vertices())
          for (std::size_t u: g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<std::size_t, std::size_t>,
              std::size_t>& kv) {
              auto& [u, v] = kv.first;
              double wu = static_cast<double>(u);
              double wv = static_cast<double>(v);
              double s = static_cast<double>(n)*static_cast<double>(n-1)/2.0;
              double puv = std::min(wu*wv/s, 1.0);
              if (u == v) puv = 0;
              double mean = static_cast<double>(ens)*puv;
              double sigma = std::sqrt(puv*(1-puv)*static_cast<double>(ens));

              return static_cast<double>(kv.second) >= mean - 3*sigma &&
                      static_cast<double>(kv.second) <= mean + 3*sigma;
            }));
    }
  }
}

TEST_CASE("random directed expected degree sequence graph",
    "[dag::random_directed_expected_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = dag::random_directed_expected_degree_sequence_graph<std::size_t>(
        std::vector<double>(n, 0.0), std::vector<double>(n, 0.0), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = dag::random_directed_expected_degree_sequence_graph<std::size_t>(
        std::vector<double>(), std::vector<double>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("doesn't have a loop if we don't want it") {
    std::size_t n = 20;
    double w = 3.0;

    auto g = dag::random_directed_expected_degree_sequence_graph<std::size_t>(
        std::vector<double>(n, w), std::vector<double>(n, w), gen, false);
    REQUIRE(std::ranges::none_of(g.edges(),
          [](const auto& e) {
            return e.head() == e.tail();
          }));
  }

  SECTION("edge existance probability") {
    std::size_t ens = 20'000;
    std::size_t n = 20;

    std::unordered_map<
      std::pair<std::size_t, std::size_t>,
      std::size_t,
      dag::hash<std::pair<std::size_t, std::size_t>>> p;

    SECTION("with self-loops") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = dag::random_directed_expected_degree_sequence_graph<
          std::size_t>(
              std::ranges::iota_view{std::size_t{}, n},
              std::ranges::iota_view{std::size_t{}, n} | std::views::reverse,
              gen, true);
        for (std::size_t v: g.vertices())
          for (std::size_t u: g.successors(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<std::size_t, std::size_t>,
              std::size_t>& kv) {
              auto& [u, v] = kv.first;
              double wu = static_cast<double>(u);
              double wv = 19 - static_cast<double>(v);
              double s = static_cast<double>(n)*static_cast<double>(n-1)/2.0;
              double puv = std::min(wu*wv/s, 1.0);
              if (u == v) puv *= 2.0;
              double mean = static_cast<double>(ens)*puv;
              double sigma = std::sqrt(puv*(1-puv)*static_cast<double>(ens));

              return static_cast<double>(kv.second) >= mean - 3*sigma &&
                      static_cast<double>(kv.second) <= mean + 3*sigma;
            }));
    }

    SECTION("without self-loop") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = dag::random_directed_expected_degree_sequence_graph<
          std::size_t>(
              std::ranges::iota_view{std::size_t{}, n},
              std::ranges::iota_view{std::size_t{}, n} | std::views::reverse,
              gen, false);
        for (std::size_t v: g.vertices())
          for (std::size_t u: g.successors(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<std::size_t, std::size_t>,
              std::size_t>& kv) {
              auto& [u, v] = kv.first;
              double wu = static_cast<double>(u);
              double wv = 19 - static_cast<double>(v);
              double s = static_cast<double>(n)*static_cast<double>(n-1)/2.0;
              double puv = std::min(wu*wv/s, 1.0);
              if (u == v) puv = 0;
              double mean = static_cast<double>(ens)*puv;
              double sigma = std::sqrt(puv*(1-puv)*static_cast<double>(ens));

              return static_cast<double>(kv.second) >= mean - 3*sigma &&
                      static_cast<double>(kv.second) <= mean + 3*sigma;
            }));
    }
  }
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
