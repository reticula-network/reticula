#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/generation/fully_mixed.hpp>
#include <reticula/operations/projections.hpp>
#include <reticula/randomisation/event_shufflings.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

/**
  Returns a directed temporal network of 1024 nodes with many events between
  nodes 0 and 1.
*/
template <std::uniform_random_bit_generator Gen>
reticula::directed_temporal_network random_uneven_temporal_network(Gen& gen) {
  auto g1 = reticula::random_directed_fully_mixed_temporal_network(
    static_cast<std::size_t>(2), 1, 1024, gen);
  return reticula::directed_temporal_network(
    g1.edges_cause(), std::views::iota(0uz, 1024uz));
}

template <std::ranges::range EdgeRange>
  requires reticula::temporal_network_edge<
    std::ranges::range_value_t<EdgeRange>>
std::vector<std::pair<double, double>> timestamps(const EdgeRange& edge_list) {
  std::vector<std::pair<double, double>> times;
  times.reserve(std::ranges::size(edge_list));
  for (auto& e : edge_list)
    times.emplace_back(e.cause_time(), e.effect_time());

  return times;
}

TEST_CASE("instant event shuffling", "[reticula::instant_event_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled = reticula::instant_event_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
  REQUIRE_THAT(
    timestamps(g.edges_cause()),
    RangeEquals(timestamps(shuffled.edges_cause())));
  REQUIRE(
    reticula::static_projection(g).edges().size() <
    reticula::static_projection(shuffled).edges().size());
}
