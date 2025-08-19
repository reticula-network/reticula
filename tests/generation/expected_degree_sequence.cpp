#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/generation/expected_degree_sequence.hpp>

TEST_CASE(
  "random expected degree sequence graph",
  "[reticula::expected_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::expected_degree_sequence_graph(
      std::vector<double>(n, 0.0), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 =
      reticula::expected_degree_sequence_graph(std::vector<double>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("doesn't have a loop if we don't want it") {
    std::size_t n = 20;
    double w = 3.0;

    auto g = reticula::expected_degree_sequence_graph(
      std::vector<double>(n, w), gen, false);
    REQUIRE(std::ranges::none_of(g.edges(), [](const auto& e) {
      return e.incident_verts().size() == 1;
    }));
  }

  SECTION("edge existence probability") {
    std::size_t ens = 800;
    std::size_t n = 15;

    std::unordered_map<
      std::pair<reticula::VertexType, reticula::VertexType>, std::size_t,
      reticula::hash<std::pair<reticula::VertexType, reticula::VertexType>>>
      p;
    p.reserve(n * n);

    SECTION("with self-loops") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = reticula::expected_degree_sequence_graph(
          std::views::iota(reticula::VertexType{}, n), gen, true);
        for (auto v : g.vertices())
          for (auto u : g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      REQUIRE(
        std::ranges::all_of(
          p, [n, ens](
               const std::pair<
                 std::pair<reticula::VertexType, reticula::VertexType>,
                 std::size_t>& kv) {
            auto& [u, v] = kv.first;
            auto wu = static_cast<double>(u);
            auto wv = static_cast<double>(v);
            double s =
              static_cast<double>(n) * static_cast<double>(n - 1) / 2.0;
            double puv = std::min(wu * wv / s, 1.0);
            if (u == v)
              puv *= 2.0;
            double mean = static_cast<double>(ens) * puv;
            double sigma =
              std::sqrt(puv * (1 - puv) * static_cast<double>(ens));

            return static_cast<double>(kv.second) >= mean - 5 * sigma &&
                   static_cast<double>(kv.second) <= mean + 5 * sigma;
          }));
    }

    SECTION("without self-loop") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = reticula::expected_degree_sequence_graph(
          std::views::iota(reticula::VertexType{}, n), gen, false);
        for (auto v : g.vertices())
          for (auto u : g.neighbours(v))
            p[std::make_pair(u, v)]++;
      }

      REQUIRE(
        std::ranges::all_of(
          p, [n, ens](
               const std::pair<
                 std::pair<reticula::VertexType, reticula::VertexType>,
                 std::size_t>& kv) {
            auto& [u, v] = kv.first;
            auto wu = static_cast<double>(u);
            auto wv = static_cast<double>(v);
            double s =
              static_cast<double>(n) * static_cast<double>(n - 1) / 2.0;
            double puv = std::min(wu * wv / s, 1.0);
            if (u == v)
              puv = 0;
            double mean = static_cast<double>(ens) * puv;
            double sigma =
              std::sqrt(puv * (1 - puv) * static_cast<double>(ens));

            return static_cast<double>(kv.second) >= mean - 5 * sigma &&
                   static_cast<double>(kv.second) <= mean + 5 * sigma;
          }));
    }
  }
}

TEST_CASE(
  "random directed expected degree sequence graph",
  "[reticula::directed_expected_degree_sequence_graph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::directed_expected_degree_sequence_graph(
      std::vector<std::pair<double, double>>(n, {0.0, 0.0}), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::directed_expected_degree_sequence_graph(
      std::vector<std::pair<double, double>>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("doesn't have a loop if we don't want it") {
    std::size_t n = 20;
    double w = 3.0;

    auto g = reticula::directed_expected_degree_sequence_graph(
      std::vector<std::pair<double, double>>(n, {w, w}), gen, false);
    REQUIRE(std::ranges::none_of(g.edges(), [](const auto& e) {
      return e.head() == e.tail();
    }));
  }

  SECTION("works with almost equal weight sequences") {
    std::size_t n = 20;
    REQUIRE_NOTHROW(
      reticula::directed_expected_degree_sequence_graph(
        std::vector<std::pair<double, double>>(n, {2.99, 3.01}), gen, false));
  }

  SECTION("throws with unequal weight sequences") {
    std::size_t n = 20;
    REQUIRE_THROWS(
      reticula::directed_expected_degree_sequence_graph(
        std::vector<std::pair<double, double>>(n, {2.0, 3.0}), gen, false));
  }

  SECTION("edge existence probability") {
    std::size_t ens = 800;
    auto weights = std::vector<std::pair<double, double>>{
      {4, 4}, {3, 5}, {4, 4}, {4, 4}, {5, 5}, {3, 5}, {5, 2}, {3, 2}};
    auto n = weights.size();

    std::vector<double> w_in(n), w_out(n);
    for (std::size_t v = 0; v < n; ++v) {
      w_in[v] = weights[v].first;
      w_out[v] = weights[v].second;
    }
    const double s = std::accumulate(w_in.begin(), w_in.end(), 0.0);

    std::unordered_map<
      std::pair<reticula::VertexType, reticula::VertexType>, std::size_t,
      reticula::hash<std::pair<reticula::VertexType, reticula::VertexType>>>
      p;
    p.reserve(n * n);

    SECTION("with self-loops") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g =
          reticula::directed_expected_degree_sequence_graph(weights, gen, true);
        for (auto v : g.vertices())
          for (auto u : g.successors(v))
            p[std::make_pair(u, v)]++;
      }

      for (auto& [e, cnts] : p) {
        auto& [u, v] = e;
        auto wu = static_cast<double>(w_in[u]);
        auto wv = static_cast<double>(w_out[v]);
        double puv = std::min(wu * wv / s, 1.0);
        double mean = static_cast<double>(ens) * puv;
        double sigma = std::sqrt(puv * (1 - puv) * static_cast<double>(ens));
        REQUIRE(static_cast<double>(cnts) >= mean - 5 * sigma);
        REQUIRE(static_cast<double>(cnts) <= mean + 5 * sigma);
      }
    }

    SECTION("without self-loop") {
      for (std::size_t i = 0; i < ens; i++) {
        auto g = reticula::directed_expected_degree_sequence_graph(
          weights, gen, false);
        for (auto v : g.vertices())
          for (auto u : g.successors(v))
            p[std::make_pair(u, v)]++;
      }

      for (auto& [e, cnts] : p) {
        auto& [u, v] = e;
        auto wu = static_cast<double>(w_in[u]);
        auto wv = static_cast<double>(w_out[v]);
        double puv = std::min(wu * wv / s, 1.0);
        if (u == v)
          puv = 0;
        double mean = static_cast<double>(ens) * puv;
        double sigma = std::sqrt(puv * (1 - puv) * static_cast<double>(ens));
        REQUIRE(static_cast<double>(cnts) >= mean - 5 * sigma);
        REQUIRE(static_cast<double>(cnts) <= mean + 5 * sigma);
      }
    }
  }
}

TEST_CASE(
  "random expected degree sequence hypergraph",
  "[reticula::expected_degree_sequence_hypergraph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::expected_degree_sequence_hypergraph(
      std::vector<double>(n, 0.0), std::vector<double>(), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::expected_degree_sequence_hypergraph(
      std::vector<double>(), std::vector<double>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("works with almost equal weight sequences") {
    std::size_t n = 20;
    REQUIRE_NOTHROW(
      reticula::expected_degree_sequence_hypergraph(
        std::vector<double>(n, 2.99), std::vector<double>(n, 3.01), gen));
  }

  SECTION("throws with unequal weight sequences") {
    std::size_t n = 20;
    REQUIRE_THROWS(
      reticula::expected_degree_sequence_hypergraph(
        std::vector<double>(n, 2.00), std::vector<double>(n, 3.00), gen));
  }

  SECTION("node and edge degree probability") {
    std::size_t ens = 800;
    std::size_t n = 50;

    std::unordered_map<reticula::VertexType, std::size_t> node_degrees;
    std::unordered_map<std::size_t, std::size_t> edge_degrees;
    std::vector<double> weights;
    for (std::size_t i{}; i < n; i++)
      weights.push_back(4.0 + static_cast<double>(i % 25));

    for (std::size_t i{}; i < ens; i++) {
      auto g =
        reticula::expected_degree_sequence_hypergraph(weights, weights, gen);

      for (auto&& v : g.vertices())
        node_degrees[v] += g.degree(v);

      for (auto&& e : g.edges())
        edge_degrees[e.incident_verts().size()]++;
    }

    REQUIRE(
      std::ranges::all_of(
        node_degrees,
        [ens,
         &weights](const std::pair<reticula::VertexType, std::size_t>& kv) {
          auto& [v, d] = kv;
          double mean =
            static_cast<double>(ens) * weights[static_cast<std::size_t>(v)];
          double sigma = std::sqrt(mean);

          return static_cast<double>(d) >= mean - 4 * sigma &&
                 static_cast<double>(d) <= mean + 4 * sigma;
        }));

    std::unordered_map<std::size_t, std::size_t> edge_degree_means;
    for (auto&& w : weights)
      edge_degree_means[static_cast<std::size_t>(w)] += ens;
    std::size_t max_d = std::max(
      std::ranges::max(
        edge_degree_means |
        std::views::transform([](const auto& kv) { return kv.first; })),
      std::ranges::max(edge_degrees | std::views::transform([](const auto& kv) {
                         return kv.first;
                       })));

    double sum_theo = 0.0, sum_exp = 0.0;
    for (std::size_t i{}; i <= max_d; i++) {
      sum_theo += static_cast<double>(edge_degree_means[i]);
      sum_exp += static_cast<double>(edge_degrees[i]);
    }

    double curr_theo = 0.0, curr_exp = 0.0;
    double max_diff = 0.0;
    for (std::size_t i{}; i <= max_d; i++) {
      curr_theo += static_cast<double>(edge_degree_means[i]);
      curr_exp += static_cast<double>(edge_degrees[i]);
      max_diff =
        std::max(max_diff, std::abs(curr_theo / sum_theo - curr_exp / sum_exp));
    }
    REQUIRE(max_diff < 0.1);
  }
}

TEST_CASE(
  "random directed expected degree sequence hypergraph",
  "[reticula::random_directed_expected_degree_sequence_hypergraph]") {
  std::mt19937_64 gen(42);

  SECTION("deals well with empty or zero weights") {
    std::size_t n = 20;

    auto g1 = reticula::directed_expected_degree_sequence_hypergraph(
      std::vector<std::pair<double, double>>(n, {0.0, 0.0}),
      std::vector<std::pair<double, double>>(), gen);

    REQUIRE(g1.edges().size() == 0);
    REQUIRE(g1.vertices().size() == n);

    auto g2 = reticula::directed_expected_degree_sequence_hypergraph(
      std::vector<std::pair<double, double>>(),
      std::vector<std::pair<double, double>>(), gen);

    REQUIRE(g2.edges().size() == 0);
    REQUIRE(g2.vertices().size() == 0);
  }

  SECTION("works with almost equal weight sequences") {
    std::size_t n = 20;
    REQUIRE_NOTHROW(
      reticula::directed_expected_degree_sequence_hypergraph(
        std::vector<std::pair<double, double>>(n, {2.99, 3.00}),
        std::vector<std::pair<double, double>>(n, {3.01, 2.98}), gen));
  }

  SECTION("throws with unequal weight sequences") {
    std::size_t n = 20;
    REQUIRE_THROWS(
      reticula::directed_expected_degree_sequence_hypergraph(
        std::vector<std::pair<double, double>>(n, {2.00, 4.00}),
        std::vector<std::pair<double, double>>(n, {3.00, 5.00}), gen));
  }

  SECTION("edge existence probability") {
    std::size_t ens = 600;
    std::size_t n = 50;

    std::vector<std::pair<double, double>> weights;
    weights.reserve(n);
    for (std::size_t i{}; i < n; i++)
      weights.emplace_back(
        4.0 + static_cast<double>(i % 25),
        4.0 + static_cast<double>(24 - (i % 25)));

    std::unordered_map<
      reticula::VertexType, std::pair<std::size_t, std::size_t>>
      node_degrees;
    node_degrees.reserve(n);

    for (std::size_t i{}; i < ens; i++) {
      auto g = reticula::directed_expected_degree_sequence_hypergraph(
        weights, weights, gen);

      for (auto&& v : g.vertices()) {
        node_degrees[v].first += g.in_degree(v);
        node_degrees[v].second += g.out_degree(v);
      }
    }

    REQUIRE(
      std::ranges::all_of(
        node_degrees,
        [ens, &weights](
          const std::pair<
            reticula::VertexType, std::pair<std::size_t, std::size_t>>& kv) {
          auto& [v, ds] = kv;
          auto& [in_d, out_d] = ds;

          double mean_in = static_cast<double>(ens) *
                           weights[static_cast<std::size_t>(v)].first;

          double sigma_in = std::sqrt(mean_in);

          double mean_out = static_cast<double>(ens) *
                            weights[static_cast<std::size_t>(v)].second;
          double sigma_out = std::sqrt(mean_out);

          return static_cast<double>(in_d) >= mean_in - 4 * sigma_in &&
                 static_cast<double>(in_d) <= mean_in + 4 * sigma_in &&
                 static_cast<double>(out_d) >= mean_out - 4 * sigma_out &&
                 static_cast<double>(out_d) <= mean_out + 4 * sigma_out;
        }));
  }
}
