#include <cmath>
#include <algorithm>

#include <catch2/catch.hpp>

#include <reticula/networks.hpp>
#include <reticula/random_networks.hpp>

using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

TEMPLATE_TEST_CASE("Random G(n, p) graph",
    "[reticula::random_gnp_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 100000;

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
    reticula::undirected_network<TestType> r =
      reticula::random_gnp_graph<TestType>(n, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = static_cast<double>(n)*static_cast<double>((n-1)/2)*p;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);

    // TODO add uniformity test
  }
}

TEMPLATE_TEST_CASE("Barabasi-Albert random graph",
    "[reticula::random_barabasi_albert_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 1000, m = 3;
  reticula::undirected_network<TestType> r =
    reticula::random_barabasi_albert_graph<TestType>(n, m, gen);
  REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));
  REQUIRE(r.edges().size() == static_cast<std::size_t>((n-m)*m));
}

TEMPLATE_TEST_CASE("random k-regular graph", "[reticula::random_regular_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 1000, k = 4;
  reticula::undirected_network<TestType> r =
    reticula::random_regular_graph<TestType>(n, k, gen);
  REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));
  REQUIRE(r.edges().size() == static_cast<std::size_t>(k*n/2));

  auto verts = r.vertices();
  REQUIRE(std::all_of(verts.cbegin(), verts.cend(),
        [&r, k](TestType v) {
          return r.degree(v) == static_cast<std::size_t>(k);
        }));

  // TODO add uniformity test
}

TEST_CASE("random directed degree sequence graph",
    "[reticula::random_directed_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::random_directed_degree_sequence_graph<int>(
        std::vector<std::pair<int, int>>(n, {0, 0}), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::random_directed_degree_sequence_graph<int>(
        std::vector<std::pair<int, int>>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("produces the degree sequence we wanted with no self-loop") {
    std::size_t n = 20;
    int w = 3;

    std::vector<std::pair<int, int>>
      degree_sequence(n, {w, w});

    auto g = reticula::random_directed_degree_sequence_graph<int>(
        degree_sequence, gen);

    std::vector<std::pair<int, int>> degrees;
    for (auto v: g.vertices())
      degrees.emplace_back(
          static_cast<int>(g.in_degree(v)),
          static_cast<int>(g.out_degree(v)));

    REQUIRE_THAT(degrees, UnorderedEquals(degree_sequence));

    REQUIRE(std::ranges::none_of(g.edges(),
          [](const auto& e) {
            return e.head() == e.tail();
          }));
  }

  // TODO: test uniformity
}

TEST_CASE("random degree sequence graph",
    "[reticula::random_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::random_degree_sequence_graph<int>(
        std::vector<int>(n, 0), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::random_degree_sequence_graph<int>(
        std::vector<int>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("produces the degree sequence we wanted with no self-loop") {
    std::size_t n = 20;
    int w = 3;

    std::vector<int> degree_sequence(n, w);

    auto g = reticula::random_degree_sequence_graph<int>(
        degree_sequence, gen);

    std::vector<int> degrees;
    for (auto v: g.vertices())
      degrees.push_back(static_cast<int>(g.degree(v)));

    REQUIRE_THAT(degrees, UnorderedEquals(degree_sequence));

    REQUIRE(std::ranges::none_of(g.edges(),
          [](const auto& e) {
            return e.incident_verts().size() == 1;
          }));
  }

  // TODO: test uniformity
}

TEST_CASE("random expected degree sequence graph",
    "[reticula::random_expected_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::random_expected_degree_sequence_graph<int>(
        std::vector<double>(n, 0.0), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::random_expected_degree_sequence_graph<int>(
        std::vector<double>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("doesn't have a loop if we don't want it") {
    std::size_t n = 20;
    double w = 3.0;

    auto g = reticula::random_expected_degree_sequence_graph<int>(
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
      std::pair<int, int>,
      std::size_t,
      reticula::hash<std::pair<int, int>>> p;

    SECTION("with self-loops") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = reticula::random_expected_degree_sequence_graph<int>(
            std::ranges::iota_view{int{}, static_cast<int>(n)}, gen, true);
        for (auto v: g.vertices())
          for (auto u: g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<int, int>,
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
        auto g = reticula::random_expected_degree_sequence_graph<int>(
            std::ranges::iota_view{int{}, static_cast<int>(n)}, gen, false);
        for (auto v: g.vertices())
          for (auto u: g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<int, int>,
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
    "[reticula::random_directed_expected_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::random_directed_expected_degree_sequence_graph<int>(
        std::vector<std::pair<double, double>>(n, {0.0, 0.0}), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::random_directed_expected_degree_sequence_graph<int>(
        std::vector<std::pair<double, double>>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("doesn't have a loop if we don't want it") {
    std::size_t n = 20;
    double w = 3.0;

    auto g = reticula::random_directed_expected_degree_sequence_graph<int>(
        std::vector<std::pair<double, double>>(n, {w, w}), gen, false);
    REQUIRE(std::ranges::none_of(g.edges(),
          [](const auto& e) {
            return e.head() == e.tail();
          }));
  }

  SECTION("edge existance probability") {
    std::size_t ens = 20'000;
    std::size_t n = 20;

    std::unordered_map<
      std::pair<int, int>,
      std::size_t,
      reticula::hash<std::pair<int, int>>> p;

    std::vector<std::pair<double, double>> weights;
    weights.reserve(n);
    std::ranges::transform(
        std::ranges::iota_view{int{}, static_cast<int>(n)},
        std::ranges::iota_view{
          int{}, static_cast<int>(n)} | std::views::reverse,
        std::back_inserter(weights),
        [](int i, int j) { return std::make_pair(i, j);});

    SECTION("with self-loops") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = reticula::random_directed_expected_degree_sequence_graph<int>(
            weights, gen, true);
        for (auto v: g.vertices())
          for (auto u: g.successors(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<int, int>,
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
        auto g = reticula::random_directed_expected_degree_sequence_graph<int>(
            weights, gen, false);
        for (auto v: g.vertices())
          for (auto u: g.successors(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(std::ranges::all_of(p,
            [n, ens](const std::pair<
              std::pair<int, int>,
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

TEST_CASE("random fully-mixed temporal network",
    "[reticula::random_fully_mixed_temporal_network]") {
  std::mt19937_64 gen(42);

  std::size_t n = 20;
  double max_t = 500;
  double rate = 2.0;
  auto g = reticula::random_fully_mixed_temporal_network(n, rate, max_t, gen);

  double per_event_mean = max_t*rate;
  double edges = static_cast<double>(n*(n-1)/2);

  REQUIRE(static_cast<double>(g.edges().size()) >
      edges*per_event_mean - 3*std::sqrt(edges*per_event_mean));
  REQUIRE(static_cast<double>(g.edges().size()) <
      edges*per_event_mean + 3*std::sqrt(edges*per_event_mean));

  std::unordered_map<
    reticula::undirected_edge<std::size_t>, std::size_t,
    reticula::hash<reticula::undirected_edge<std::size_t>>> counts;
  for (auto&& e: g.edges())
    counts[e.static_projection()]++;

  REQUIRE(std::ranges::all_of(counts,
        [per_event_mean](
            std::pair<reticula::undirected_edge<std::size_t>, std::size_t> kv) {
          auto& [k, count] = kv;
          double sigma = std::sqrt(per_event_mean);
          return static_cast<double>(count) > per_event_mean - 4*sigma &&
                static_cast<double>(count) < per_event_mean + 4*sigma;
        }));
}


TEST_CASE("random directed fully-mixed temporal network",
    "[reticula::random_directed_fully_mixed_temporal_network]") {
  std::mt19937_64 gen(42);

  std::size_t n = 20;
  double max_t = 500;
  double rate = 2.0;
  auto g = reticula::random_directed_fully_mixed_temporal_network(
      n, rate, max_t, gen);

  double per_event_mean = max_t*rate;
  double edges = static_cast<double>(n*(n-1));

  REQUIRE(static_cast<double>(g.edges().size()) >
      edges*per_event_mean - 3*std::sqrt(edges*per_event_mean));
  REQUIRE(static_cast<double>(g.edges().size()) <
      edges*per_event_mean + 3*std::sqrt(edges*per_event_mean));

  std::unordered_map<
    reticula::directed_edge<std::size_t>, std::size_t,
    reticula::hash<reticula::directed_edge<std::size_t>>> counts;
  for (auto&& e: g.edges())
    counts[e.static_projection()]++;

  REQUIRE(std::ranges::all_of(counts,
        [per_event_mean](
            std::pair<reticula::directed_edge<std::size_t>, std::size_t> kv) {
          auto& [k, count] = kv;
          double sigma = std::sqrt(per_event_mean);
          return static_cast<double>(count) > per_event_mean - 4*sigma &&
                static_cast<double>(count) < per_event_mean + 4*sigma;
        }));
}

TEST_CASE("power-law with secified mean",
    "[reticula::power_law_with_specified_mean]") {
  REQUIRE(reticula::random_number_distribution<
      reticula::power_law_with_specified_mean<>>);
}

TEST_CASE("residual power-law with specified mean",
    "[reticula::residual_power_law_with_specified_mean]") {
  REQUIRE(reticula::random_number_distribution<
      reticula::residual_power_law_with_specified_mean<>>);
}

TEST_CASE("hawkes univariate exponential",
    "[reticula::hawkes_univariate_exponential]") {
  REQUIRE(reticula::random_number_distribution<
      reticula::hawkes_univariate_exponential<>>);
}