#include <cmath>
#include <algorithm>

#include <reticula/ranges.hpp>
#include <reticula/networks.hpp>
#include <reticula/random_networks.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
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

    double mean = static_cast<double>(n)*static_cast<double>(n-1)/2*p;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);
  }

  SECTION("edge probability") {
    double p = 0.1;
    TestType n = 20;
    std::unordered_map<reticula::undirected_edge<TestType>, std::size_t> n_edges;
    std::size_t n_trials = 10000;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::undirected_network<TestType> r =
        reticula::random_gnp_graph<TestType>(n, p, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n)/static_cast<double>(n_trials);
      REQUIRE(p_est > p*0.75);
      REQUIRE(p_est < p*1.25);
    }
  }
}

TEMPLATE_TEST_CASE("Random directed G(n, p) graph",
    "[reticula::random_directed_gnp_graph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 100000;

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
    reticula::directed_network<TestType> r =
      reticula::random_directed_gnp_graph<TestType>(n, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = static_cast<double>(n)*static_cast<double>(n-1)*p;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);
  }

  SECTION("edge probability") {
    double p = 0.1;
    TestType n = 20;
    std::unordered_map<reticula::directed_edge<TestType>, std::size_t> n_edges;
    std::size_t n_trials = 10000;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::directed_network<TestType> r =
        reticula::random_directed_gnp_graph<TestType>(n, p, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n)/static_cast<double>(n_trials);
      REQUIRE(p_est > p*0.75);
      REQUIRE(p_est < p*1.25);
    }
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

    REQUIRE(reticula::ranges::none_of(g.edges(),
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

    REQUIRE(reticula::ranges::none_of(g.edges(),
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
    REQUIRE(reticula::ranges::none_of(g.edges(),
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
            reticula::views::iota(int{}, static_cast<int>(n)),
            gen, true);
        for (auto v: g.vertices())
          for (auto u: g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(reticula::ranges::all_of(p,
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
            reticula::views::iota(int{}, static_cast<int>(n)),
            gen, false);
        for (auto v: g.vertices())
          for (auto u: g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      // Three-sigma test with normal approximation of binomial distribution
      REQUIRE(reticula::ranges::all_of(p,
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
    REQUIRE(reticula::ranges::none_of(g.edges(),
          [](const auto& e) {
            return e.head() == e.tail();
          }));
  }

  SECTION("works with almost equal weight sequences") {
    std::size_t n = 20;
    REQUIRE_NOTHROW(
        reticula::random_directed_expected_degree_sequence_graph<int>(
          std::vector<std::pair<double, double>>(n, {2.99, 3.01}), gen, false));
  }

  SECTION("throws with unequal weight sequences") {
    std::size_t n = 20;
    REQUIRE_THROWS(
        reticula::random_directed_expected_degree_sequence_graph<int>(
          std::vector<std::pair<double, double>>(n, {2.0, 3.0}), gen, false));
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
    reticula::ranges::transform(
        reticula::views::iota(int{}, static_cast<int>(n)),
        reticula::views::iota(int{}, static_cast<int>(n))
          | reticula::views::reverse,
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
      REQUIRE(reticula::ranges::all_of(p,
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
      REQUIRE(reticula::ranges::all_of(p,
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

TEST_CASE("random expected degree sequence hypergraph",
    "[reticula::random_expected_degree_sequence_hypergraph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::random_expected_degree_sequence_hypergraph<int>(
        std::vector<double>(n, 0.0), std::vector<double>(), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::random_expected_degree_sequence_hypergraph<int>(
        std::vector<double>(), std::vector<double>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("works with almost equal weight sequences") {
    std::size_t n = 20;
    REQUIRE_NOTHROW(
        reticula::random_expected_degree_sequence_hypergraph<int>(
          std::vector<double>(n, 2.99), std::vector<double>(n, 3.01), gen));
  }

  SECTION("throws with unequal weight sequences") {
    std::size_t n = 20;
    REQUIRE_THROWS(
        reticula::random_expected_degree_sequence_hypergraph<int>(
          std::vector<double>(n, 2.00), std::vector<double>(n, 3.00), gen));
  }

  SECTION("node and edge degree probability") {
    std::size_t ens = 1000;
    std::size_t n = 200;

    std::unordered_map<int, std::size_t> node_degrees;
    std::unordered_map<std::size_t, std::size_t> edge_degrees;
    std::vector<double> weights;
    for (std::size_t i{}; i < n; i++)
      weights.push_back(4.0+static_cast<double>(i%25));

    for (std::size_t i{}; i < ens; i++) {
      auto g = reticula::random_expected_degree_sequence_hypergraph<int>(
          weights, weights, gen);

      for (auto&& v: g.vertices())
        node_degrees[v] += g.degree(v);

      for (auto&& e: g.edges())
        edge_degrees[e.incident_verts().size()]++;
    }

    REQUIRE(reticula::ranges::all_of(node_degrees,
          [ens, &weights](const std::pair<int, std::size_t>& kv) {
            auto& [v, d] = kv;
            double mean = static_cast<double>(ens)*weights[
              static_cast<std::size_t>(v)];
            double sigma = std::sqrt(mean);

            return static_cast<double>(d) >= mean - 3*sigma &&
                    static_cast<double>(d) <= mean + 3*sigma;
          }));


    std::unordered_map<std::size_t, std::size_t> edge_degree_means;
    for (auto&& w: weights)
      edge_degree_means[static_cast<std::size_t>(w)] += ens;
    std::size_t max_d = std::max(
        reticula::ranges::max(
          edge_degree_means | reticula::views::transform([](
              const auto& kv){ return kv.first; })),
        reticula::ranges::max(
          edge_degrees | reticula::views::transform([](
              const auto& kv){ return kv.first; })));

    double sum_theo = 0.0, sum_exp = 0.0;
    for (std::size_t i {}; i <= max_d; i++) {
      sum_theo += static_cast<double>(edge_degree_means[i]);
      sum_exp += static_cast<double>(edge_degrees[i]);
    }

    double curr_theo = 0.0, curr_exp = 0.0;
    double max_diff = 0.0;
    for (std::size_t i {}; i <= max_d; i++) {
      curr_theo += static_cast<double>(edge_degree_means[i]);
      curr_exp += static_cast<double>(edge_degrees[i]);
      max_diff = std::max(max_diff,
          curr_theo/sum_theo - curr_exp/sum_exp);
    }
    REQUIRE(max_diff < 0.1);  // TODO: do a proper Kolmogorov-Smirnov test
  }
}

TEST_CASE("random directed expected degree sequence hypergraph",
    "[reticula::random_directed_expected_degree_sequence_hypergraph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::
            random_directed_expected_degree_sequence_hypergraph<int>(
         std::vector<std::pair<double, double>>(n, {0.0, 0.0}),
        std::vector<std::pair<double, double>>(), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::
            random_directed_expected_degree_sequence_hypergraph<int>(
        std::vector<std::pair<double, double>>(),
        std::vector<std::pair<double, double>>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("works with almost equal weight sequences") {
    std::size_t n = 20;
    REQUIRE_NOTHROW(
        reticula::random_directed_expected_degree_sequence_hypergraph<int>(
          std::vector<std::pair<double, double>>(n, {2.99, 3.00}),
          std::vector<std::pair<double, double>>(n, {3.01, 2.98}), gen));
  }

  SECTION("throws with unequal weight sequences") {
    std::size_t n = 20;
    REQUIRE_THROWS(
        reticula::random_directed_expected_degree_sequence_hypergraph<int>(
          std::vector<std::pair<double, double>>(n, {2.00, 4.00}),
          std::vector<std::pair<double, double>>(n, {3.00, 5.00}), gen));
  }

  SECTION("edge existance probability") {
    std::size_t ens = 1000;
    std::size_t n = 100;

    std::vector<std::pair<double, double>> weights;
    weights.reserve(n);
    for (std::size_t i{}; i < n; i++)
      weights.emplace_back(
          4.0+static_cast<double>(i%25),
          4.0+static_cast<double>(24 - (i%25)));

    std::unordered_map<int, std::pair<std::size_t, std::size_t>> node_degrees;

    for (std::size_t i{}; i < ens; i++) {
      auto g = reticula::
            random_directed_expected_degree_sequence_hypergraph<int>(
          weights, weights, gen);

      for (auto&& v: g.vertices()) {
        node_degrees[v].first += g.in_degree(v);
        node_degrees[v].second += g.out_degree(v);
      }
    }

    REQUIRE(reticula::ranges::all_of(node_degrees,
          [ens, &weights](
              const std::pair<int, std::pair<std::size_t, std::size_t>>& kv) {
            auto& [v, ds] = kv;
            auto& [in_d, out_d] = ds;

            double mean_in = static_cast<double>(ens)*weights[
              static_cast<std::size_t>(v)].first;

            double sigma_in = std::sqrt(mean_in);

            double mean_out = static_cast<double>(ens)*weights[
              static_cast<std::size_t>(v)].second;
            double sigma_out = std::sqrt(mean_out);

            return static_cast<double>(in_d) >= mean_in - 3*sigma_in &&
                    static_cast<double>(in_d) <= mean_in + 3*sigma_in &&
                    static_cast<double>(out_d) >= mean_out - 3*sigma_out &&
                    static_cast<double>(out_d) <= mean_out + 3*sigma_out;
          }));

    // TODO: do a proper Kolmogorov-Smirnov test on edge size distribution
  }
}


TEMPLATE_TEST_CASE("Random uniform hypergraph",
    "[reticula::random_uniform_hypergraph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 10000;
  TestType m = 3;

  SECTION("empty") {
    auto g1 = reticula::random_uniform_hypergraph(n, m, 0, gen);
    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == static_cast<std::size_t>(n));

    auto g2 = reticula::random_uniform_hypergraph(0, 0, 1, gen);
    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("edge sizes") {
    double p = 1e-7;
    reticula::undirected_hypernetwork<TestType> r =
      reticula::random_uniform_hypergraph<TestType>(n, m, p, gen);
    for (auto&& e: r.edges())
      REQUIRE(e.incident_verts().size() == static_cast<std::size_t>(m));
  }

  SECTION("randomness") {
    double p = 1e-7;
    reticula::undirected_hypernetwork<TestType> r =
      reticula::random_uniform_hypergraph<TestType>(n, m, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = 16661.66;  // comb(n, m)*p
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);
  }

  SECTION("edge probability") {
    double p = 0.05;
    TestType n = 20;
    TestType m = 3;
    std::unordered_map<
        reticula::undirected_hyperedge<TestType>, std::size_t> n_edges;
    std::size_t n_trials = 10000;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::undirected_hypernetwork<TestType> r =
        reticula::random_uniform_hypergraph<TestType>(n, m, p, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n)/static_cast<double>(n_trials);
      REQUIRE(p_est > p*0.75);
      REQUIRE(p_est < p*1.25);
    }
  }
}

TEMPLATE_TEST_CASE("Random directed uniform hypergraph",
    "[reticula::random_directed_uniform_hypergraph]",
    std::size_t, int) {
  std::mt19937_64 gen(42);
  TestType n = 500;
  TestType m1 = 3;
  TestType m2 = 4;

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
    reticula::directed_hypernetwork<TestType> r =
      reticula::random_directed_uniform_hypergraph<TestType>(n, m1, m2, p, gen);
    for (auto&& e: r.edges()) {
      REQUIRE(e.mutator_verts().size() == static_cast<std::size_t>(m1));
      REQUIRE(e.mutated_verts().size() == static_cast<std::size_t>(m2));
    }
  }

  SECTION("randomness") {
    double p = 2e-13;
    reticula::directed_hypernetwork<TestType> r =
      reticula::random_directed_uniform_hypergraph<TestType>(n, m1, m2, p, gen);

    REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));

    double mean = 10656.72;
    double sigma = std::sqrt(mean);
    REQUIRE(static_cast<double>(r.edges().size()) > mean - 3*sigma);
    REQUIRE(static_cast<double>(r.edges().size()) < mean + 3*sigma);
  }

  SECTION("edge probability") {
    double p = 0.02;
    TestType n = 5;
    TestType m1 = 3, m2 = 2;
    std::unordered_map<
        reticula::directed_hyperedge<TestType>, std::size_t> n_edges;
    std::size_t n_trials = 10000;
    for (std::size_t i{}; i < n_trials; i++) {
      reticula::directed_hypernetwork<TestType> r =
        reticula::random_directed_uniform_hypergraph<TestType>(
                   n, m1, m2, p, gen);
      for (auto e : r.edges())
        n_edges[e]++;
    }

    for (auto& [e, n] : n_edges) {
      double p_est = static_cast<double>(n)/static_cast<double>(n_trials);
      REQUIRE(p_est > p*0.75);
      REQUIRE(p_est < p*1.25);
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
  double edges = static_cast<double>(n)*static_cast<double>(n-1)/2;

  REQUIRE(static_cast<double>(g.edges().size()) >
      edges*per_event_mean - 3*std::sqrt(edges*per_event_mean));
  REQUIRE(static_cast<double>(g.edges().size()) <
      edges*per_event_mean + 3*std::sqrt(edges*per_event_mean));

  std::unordered_map<
    reticula::undirected_edge<std::size_t>, std::size_t,
    reticula::hash<reticula::undirected_edge<std::size_t>>> counts;
  for (auto&& e: g.edges())
    counts[e.static_projection()]++;

  REQUIRE(reticula::ranges::all_of(counts,
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

  REQUIRE(reticula::ranges::all_of(counts,
        [per_event_mean](
            std::pair<reticula::directed_edge<std::size_t>, std::size_t> kv) {
          auto& [k, count] = kv;
          double sigma = std::sqrt(per_event_mean);
          return static_cast<double>(count) > per_event_mean - 4*sigma &&
                static_cast<double>(count) < per_event_mean + 4*sigma;
        }));
}
