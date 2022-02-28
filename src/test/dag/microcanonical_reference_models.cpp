#include <ranges>
#include <vector>
#include <utility>
#include <random>

#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include <dag/network_concepts.hpp>
#include <dag/random_networks.hpp>
#include <dag/temporal_algorithms.hpp>
#include <dag/algorithms.hpp>
#include <dag/microcanonical_reference_models.hpp>

/**
  Returns a directed temporal network of 1024 nodes  with many events between
  nodes 0 and 1.
*/
template <std::uniform_random_bit_generator Gen>
dag::directed_temporal_network<std::size_t, double>
random_uneven_temporal_network(Gen& gen) {
  auto g1 = dag::random_directed_fully_mixed_temporal_network(
      static_cast<std::size_t>(2), 1, 1024, gen);
  return dag::directed_temporal_network<std::size_t, double>(
      g1.edges_cause(), std::views::iota(
        std::size_t{}, static_cast<std::size_t>(1024)));
}

template <dag::temporal_edge EdgeT>
std::vector<std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>>
timestamps(const std::vector<EdgeT>& edge_list) {
  std::vector<std::pair<
    typename EdgeT::TimeType,
    typename EdgeT::TimeType>> times;
  times.reserve(std::ranges::size(edge_list));
  for (auto& e: edge_list)
    times.emplace_back(e.cause_time(), e.effect_time());

  return times;
}

TEST_CASE("instant event shuffling",
    "[dag::microcanonical_reference_models::instant_event_suffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::instant_event_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(),
      Equals(shuffled.vertices()));
  REQUIRE_THAT(timestamps(g.edges_cause()),
      Equals(timestamps(shuffled.edges_cause())));
  REQUIRE(dag::static_projection(g).edges().size()
      < dag::static_projection(shuffled).edges().size());
}

TEST_CASE("link shuffling",
    "[dag::microcanonical_reference_models::link_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::link_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(),
      Equals(shuffled.vertices()));
  REQUIRE_THAT(timestamps(g.edges_cause()),
      Equals(timestamps(shuffled.edges_cause())));
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       !Equals(dag::static_projection(shuffled).edges()));
}

TEST_CASE("connected link shuffling",
    "[dag::microcanonical_reference_models::connected_link_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);

  g = dag::graph_union(g, {{1, 3, 0}, {5, 6, 12}, {20, 25, 1}});
  auto shuffled =
    dag::microcanonical_reference_models::connected_link_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(),
      Equals(shuffled.vertices()));
  REQUIRE_THAT(timestamps(g.edges_cause()),
      Equals(timestamps(shuffled.edges_cause())));
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       !Equals(dag::static_projection(shuffled).edges()));
  REQUIRE_THAT(dag::weakly_connected_components(
              dag::static_projection(g)),
          UnorderedEquals(
              dag::weakly_connected_components(
                  dag::static_projection(shuffled))));
}

TEST_CASE("topology-constrained link shuffling",
    "[dag::microcanonical_reference_models::"
    "topology_constrained_link_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::topology_constrained_link_shuffling(
            g, gen);

  REQUIRE_THAT(g.vertices(),
      Equals(shuffled.vertices()));
  REQUIRE_THAT(timestamps(g.edges_cause()),
      Equals(timestamps(shuffled.edges_cause())));
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       Equals(dag::static_projection(shuffled).edges()));
}
