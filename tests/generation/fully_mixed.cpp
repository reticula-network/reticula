#include <catch2/catch_test_macros.hpp>

#include <reticula/generation/fully_mixed.hpp>

TEST_CASE(
  "random fully-mixed temporal network",
  "[reticula::random_fully_mixed_temporal_network]") {
  std::mt19937_64 gen(42);

  std::size_t n = 20;
  double max_t = 500;
  double rate = 2.0;
  auto g = reticula::random_fully_mixed_temporal_network(n, rate, max_t, gen);

  double per_event_mean = max_t * rate;
  double edges = static_cast<double>(n) * static_cast<double>(n - 1) / 2;

  REQUIRE(
    static_cast<double>(g.edges().size()) >
    edges * per_event_mean - 3 * std::sqrt(edges * per_event_mean));
  REQUIRE(
    static_cast<double>(g.edges().size()) <
    edges * per_event_mean + 3 * std::sqrt(edges * per_event_mean));

  std::unordered_map<
    reticula::undirected_edge, std::size_t>
    counts;
  for (auto&& e : g.edges())
    counts[e.static_projection()]++;

  REQUIRE(
    std::ranges::all_of(
      counts,
      [per_event_mean](
        std::pair<reticula::undirected_edge, std::size_t> kv) {
        auto& [k, count] = kv;
        double sigma = std::sqrt(per_event_mean);
        return static_cast<double>(count) > per_event_mean - 4 * sigma &&
               static_cast<double>(count) < per_event_mean + 4 * sigma;
      }));
}

TEST_CASE(
  "random directed fully-mixed temporal network",
  "[reticula::random_directed_fully_mixed_temporal_network]") {
  std::mt19937_64 gen(42);

  std::size_t n = 20;
  double max_t = 500;
  double rate = 2.0;
  auto g =
    reticula::random_directed_fully_mixed_temporal_network(n, rate, max_t, gen);

  double per_event_mean = max_t * rate;
  auto edges = static_cast<double>(n * (n - 1));

  REQUIRE(
    static_cast<double>(g.edges().size()) >
    edges * per_event_mean - 3 * std::sqrt(edges * per_event_mean));
  REQUIRE(
    static_cast<double>(g.edges().size()) <
    edges * per_event_mean + 3 * std::sqrt(edges * per_event_mean));

  std::unordered_map<
    reticula::directed_edge, std::size_t>
    counts;
  for (auto&& e : g.edges())
    counts[e.static_projection()]++;

  REQUIRE(
    std::ranges::all_of(
      counts,
      [per_event_mean](
        std::pair<reticula::directed_edge, std::size_t> kv) {
        auto& [k, count] = kv;
        double sigma = std::sqrt(per_event_mean);
        return static_cast<double>(count) > per_event_mean - 4 * sigma &&
               static_cast<double>(count) < per_event_mean + 4 * sigma;
      }));
}
