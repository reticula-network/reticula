#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/catch_approx.hpp>

#include <reticula/algorithms/properties.hpp>
#include <reticula/generation/fully_mixed.hpp>
#include <reticula/operations/projections.hpp>
#include <reticula/randomisation/timeline_shufflings.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;
using Catch::Approx;

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

TEST_CASE("inter-event shuffling", "[reticula::inter_event_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled = reticula::inter_event_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
  REQUIRE(g.edges_cause().size() == shuffled.edges_cause().size());
  REQUIRE(
    reticula::static_projection(g).edges().size() ==
    reticula::static_projection(shuffled).edges().size());
  REQUIRE_THAT(
    reticula::static_projection(g).edges(),
    RangeEquals(reticula::static_projection(shuffled).edges()));

  auto sp = reticula::static_projection(g);
  auto links = sp.edges();

  for (auto& l : links) {
    auto tls_before = reticula::link_timeline(g, l);
    auto tls_after = reticula::link_timeline(shuffled, l);

    REQUIRE(tls_before.size() == tls_after.size());
    if (tls_before.size() <= 1)
      continue;

    std::vector<double> iets_before, iets_after;
    iets_before.reserve(tls_before.size() - 1);
    for (std::size_t j = 1; j < tls_before.size(); j++)
      iets_before.push_back(
        tls_before[j].cause_time() - tls_before[j - 1].cause_time());

    iets_after.reserve(tls_after.size() - 1);
    for (std::size_t j = 1; j < tls_after.size(); j++)
      iets_after.push_back(
        tls_after[j].cause_time() - tls_after[j - 1].cause_time());

    std::ranges::sort(iets_before);
    std::ranges::sort(iets_after);
    REQUIRE(
      std::ranges::all_of(
        std::views::iota(std::size_t{}, iets_before.size()),
        [&iets_before, &iets_after](std::size_t i) {
          return iets_before[i] == Approx(iets_after[i]);
        }));
  }
}
