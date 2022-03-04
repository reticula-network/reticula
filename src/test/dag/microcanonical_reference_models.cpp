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

TEST_CASE("timeline shuffling",
    "[dag::microcanonical_reference_models::timeline_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::timeline_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), Equals(shuffled.vertices()));
  REQUIRE(g.edges_cause().size() == shuffled.edges_cause().size());
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       Equals(dag::static_projection(shuffled).edges()));
}

TEST_CASE("weight-constrained timeline shuffling",
    "[dag::microcanonical_reference_models::"
    "weight_constrained_timeline_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::
    weight_constrained_timeline_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), Equals(shuffled.vertices()));
  REQUIRE(g.edges_cause().size() == shuffled.edges_cause().size());
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       Equals(dag::static_projection(shuffled).edges()));

  auto [t1_s, t1_e] = dag::cause_time_window(g);
  auto [t2_s, t2_e] = dag::cause_time_window(g);
  REQUIRE(t1_s <= t2_s);
  REQUIRE(t1_e >= t2_e);

  auto tls_before = dag::link_timelines(g),
       tls_after = dag::link_timelines(shuffled);

  std::ranges::sort(tls_before);
  std::ranges::sort(tls_after);

  for (std::size_t i = 0; i < tls_before.size(); i++) {
      REQUIRE(tls_before[i].first == tls_after[i].first);
      REQUIRE(tls_before[i].second.size() == tls_after[i].second.size());
  }
}

TEST_CASE("activity-constrained timeline shuffling",
    "[dag::microcanonical_reference_models::"
    "activity_constrained_timeline_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::
    activity_constrained_timeline_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), Equals(shuffled.vertices()));
  REQUIRE(g.edges_cause().size() == shuffled.edges_cause().size());
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       Equals(dag::static_projection(shuffled).edges()));

  auto tls_before = dag::link_timelines(g),
       tls_after = dag::link_timelines(shuffled);

  std::ranges::sort(tls_before);
  std::ranges::sort(tls_after);

  for (std::size_t i = 0; i < tls_before.size(); i++) {
      REQUIRE(tls_before[i].first == tls_after[i].first);
      REQUIRE(tls_before[i].second.size() == tls_after[i].second.size());
      REQUIRE(tls_before[i].second.front().cause_time() <=
              tls_after[i].second.front().cause_time());
      REQUIRE(tls_before[i].second.back().cause_time() >=
              tls_after[i].second.back().cause_time());
  }
}

TEST_CASE("inter-event shuffling",
    "[dag::microcanonical_reference_models::inter_event_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled =
    dag::microcanonical_reference_models::inter_event_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), Equals(shuffled.vertices()));
  REQUIRE(g.edges_cause().size() == shuffled.edges_cause().size());
  REQUIRE(dag::static_projection(g).edges().size()
      == dag::static_projection(shuffled).edges().size());
  REQUIRE_THAT(dag::static_projection(g).edges(),
       Equals(dag::static_projection(shuffled).edges()));

  auto tls_before = dag::link_timelines(g),
       tls_after = dag::link_timelines(shuffled);

  std::ranges::sort(tls_before);
  std::ranges::sort(tls_after);

  using EdgeT = decltype(g)::EdgeType;

  for (std::size_t i = 0; i < tls_before.size(); i++) {
      REQUIRE(tls_before[i].first == tls_after[i].first);
      REQUIRE(tls_before[i].second.size() == tls_after[i].second.size());
      REQUIRE(tls_before[i].second.front().cause_time() ==
              tls_after[i].second.front().cause_time());

      std::vector<typename EdgeT::TimeType> iets_before, iets_after;
      iets_before.reserve(tls_before[i].second.size());
      for (std::size_t j = 1; j < tls_before[i].second.size(); j++)
        iets_before.push_back(tls_before[i].second[j].cause_time() -
                tls_before[i].second[j-1].cause_time());

      iets_after.reserve(tls_after[i].second.size());
      for (std::size_t j = 1; j < tls_after[i].second.size(); j++)
        iets_after.push_back(tls_after[i].second[j].cause_time() -
                tls_after[i].second[j-1].cause_time());

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