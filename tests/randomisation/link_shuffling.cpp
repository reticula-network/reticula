#include <iostream>
#include <random>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/algorithms/static_reachability.hpp>
#include <reticula/algorithms/vertex_degree.hpp>
#include <reticula/generation/fully_mixed.hpp>
#include <reticula/networks.hpp>
#include <reticula/operations/projections.hpp>

#include <reticula/randomisation/link_shufflings.hpp>

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

/**
  Returns an empty directed temporal network of 1024 nodes.
*/
reticula::directed_temporal_network empty_temporal_network() {
  return reticula::directed_temporal_network(
    std::views::empty<reticula::directed_temporal_edge>,
    std::views::iota(0uz, 1024uz));
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

TEST_CASE("link shuffling", "[reticula::link_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled = reticula::link_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
  REQUIRE_THAT(
    timestamps(g.edges_cause()),
    RangeEquals(timestamps(shuffled.edges_cause())));
  REQUIRE(
    reticula::static_projection(g).edges().size() ==
    reticula::static_projection(shuffled).edges().size());
  REQUIRE_THAT(
    reticula::static_projection(g).edges(),
    !RangeEquals(reticula::static_projection(shuffled).edges()));
}

TEST_CASE("connected link shuffling", "[reticula::connected_link_shuffling]") {
  std::mt19937_64 gen(42);

  SECTION("with directed temporal network") {
    auto g = random_uneven_temporal_network(gen);

    g = reticula::with_edges(g, {{1, 3, 0}, {5, 6, 12}, {20, 25, 1}});
    auto shuffled = reticula::connected_link_shuffling(g, gen);
    auto static_proj_g = reticula::static_projection(g);
    auto static_proj_shuf = reticula::static_projection(shuffled);

    REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
    REQUIRE_THAT(
      timestamps(g.edges_cause()),
      RangeEquals(timestamps(shuffled.edges_cause())));
    REQUIRE(static_proj_g.edges().size() == static_proj_shuf.edges().size());
    REQUIRE_THAT(static_proj_g.edges(), !RangeEquals(static_proj_shuf.edges()));
    REQUIRE_THAT(
      reticula::weakly_connected_components(static_proj_g),
      UnorderedRangeEquals(
        reticula::weakly_connected_components(static_proj_shuf)));
  }

  SECTION("with undirected temporal networks") {
    auto g1 = reticula::random_fully_mixed_temporal_network(
      static_cast<std::size_t>(2), 1, 1024, gen);
    reticula::undirected_temporal_network g(
      g1.edges_cause(), std::views::iota(0uz, 1024uz));

    g = reticula::with_edges(g, {{1, 3, 0}, {5, 6, 12}, {20, 25, 1}});
    auto shuffled = reticula::connected_link_shuffling(g, gen);
    auto static_proj_g = reticula::static_projection(g);
    auto static_proj_shuf = reticula::static_projection(shuffled);

    REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
    REQUIRE_THAT(
      timestamps(g.edges_cause()),
      RangeEquals(timestamps(shuffled.edges_cause())));
    REQUIRE(static_proj_g.edges().size() == static_proj_shuf.edges().size());
    REQUIRE_THAT(static_proj_g.edges(), !RangeEquals(static_proj_shuf.edges()));
    REQUIRE_THAT(
      reticula::connected_components(static_proj_g),
      UnorderedRangeEquals(reticula::connected_components(static_proj_shuf)));
  }
}

TEST_CASE(
  "topology-constrained link shuffling",
  "[reticula::topology_constrained_link_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  auto shuffled = reticula::topology_constrained_link_shuffling(g, gen);

  REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
  REQUIRE_THAT(
    timestamps(g.edges_cause()),
    RangeEquals(timestamps(shuffled.edges_cause())));
  REQUIRE(
    reticula::static_projection(g).edges().size() ==
    reticula::static_projection(shuffled).edges().size());
  REQUIRE_THAT(
    reticula::static_projection(g).edges(),
    RangeEquals(reticula::static_projection(shuffled).edges()));
}

TEST_CASE(
  "degree-constrained link shuffling",
  "[reticula::degree_constrained_link_shuffling]") {
  std::mt19937_64 gen(42);
  auto g = random_uneven_temporal_network(gen);
  std::vector<reticula::directed_temporal_edge> extra_edges;
  for (auto v : g.vertices())
    if (v > 1)
      extra_edges.emplace_back(v, v - 1, 0);
  g = reticula::with_edges(g, extra_edges);

  SECTION("simple directed") {
    auto shuffled = reticula::degree_constrained_link_shuffling(g, gen);

    REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
    REQUIRE(
      reticula::static_projection(g).edges().size() ==
      reticula::static_projection(shuffled).edges().size());
    REQUIRE_THAT(
      reticula::static_projection(g).edges(),
      !RangeEquals(reticula::static_projection(shuffled).edges()));
    REQUIRE_THAT(
      reticula::in_degree_sequence(reticula::static_projection(g)),
      UnorderedRangeEquals(
        reticula::in_degree_sequence(reticula::static_projection(shuffled))));
    REQUIRE_THAT(
      reticula::out_degree_sequence(reticula::static_projection(g)),
      UnorderedRangeEquals(
        reticula::out_degree_sequence(reticula::static_projection(shuffled))));
    REQUIRE_THAT(
      timestamps(g.edges_cause()),
      RangeEquals(timestamps(shuffled.edges_cause())));
  }

  SECTION("with self link") {
    g = reticula::with_edges(g, {{0, 0, 12}});
    auto shuffled = reticula::degree_constrained_link_shuffling(g, gen);

    for (auto proj = reticula::static_projection(shuffled);
         auto e : proj.edges())
      std::cerr << std::format("{}", e) << "\n";

    REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
    REQUIRE(
      reticula::static_projection(g).edges().size() ==
      reticula::static_projection(shuffled).edges().size());
    REQUIRE_THAT(
      reticula::static_projection(g).edges(),
      !RangeEquals(reticula::static_projection(shuffled).edges()));
    REQUIRE_THAT(
      reticula::in_degree_sequence(reticula::static_projection(g)),
      UnorderedRangeEquals(
        reticula::in_degree_sequence(reticula::static_projection(shuffled))));
    REQUIRE_THAT(
      reticula::out_degree_sequence(reticula::static_projection(g)),
      UnorderedRangeEquals(
        reticula::out_degree_sequence(reticula::static_projection(shuffled))));
    REQUIRE_THAT(
      timestamps(g.edges_cause()),
      RangeEquals(timestamps(shuffled.edges_cause())));
  }
}

TEST_CASE(
  "connected degree-constrained link shuffling",
  "[reticula::connected_degree_constrained_link_shuffling]") {
  std::mt19937_64 gen(42);

  SECTION("with directed temporal network") {
    auto g = random_uneven_temporal_network(gen);
    std::vector<reticula::directed_temporal_edge> extra_edges;
    for (auto v : g.vertices()) {
      if (v >= 20) {
        extra_edges.emplace_back(v, v - 2, static_cast<double>(v));
        extra_edges.emplace_back(v, v - 4, static_cast<double>(v) + 0.5);
      }
    }
    g = reticula::with_edges(g, extra_edges);

    auto shuffled =
      reticula::connected_degree_constrained_link_shuffling(g, gen);
    auto static_proj_g = reticula::static_projection(g);
    auto static_proj_shuf = reticula::static_projection(shuffled);

    REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));

    REQUIRE(static_proj_g.edges().size() == static_proj_shuf.edges().size());
    REQUIRE_THAT(static_proj_g.edges(), !RangeEquals(static_proj_shuf.edges()));
    REQUIRE_THAT(
      reticula::weakly_connected_components(static_proj_g),
      UnorderedRangeEquals(
        reticula::weakly_connected_components(static_proj_shuf)));

    REQUIRE_THAT(
      reticula::in_degree_sequence(static_proj_g),
      UnorderedRangeEquals(reticula::in_degree_sequence(static_proj_shuf)));
    REQUIRE_THAT(
      reticula::out_degree_sequence(static_proj_g),
      UnorderedRangeEquals(reticula::out_degree_sequence(static_proj_shuf)));
  }

  SECTION("with undirected temporal networks") {
    reticula::undirected_temporal_network g(
      std::views::empty<reticula::undirected_temporal_edge>,
      std::views::iota(0uz, 1024uz));
    std::vector<reticula::undirected_temporal_edge> extra_edges;
    for (auto v : g.vertices()) {
      if (v > 20) {
        extra_edges.emplace_back(v, v - 2, static_cast<double>(v));
        extra_edges.emplace_back(v, v - 4, static_cast<double>(v) + 0.5);
      }
    }
    g = reticula::with_edges(g, extra_edges);
    auto shuffled =
      reticula::connected_degree_constrained_link_shuffling(g, gen);
    auto static_proj_g = reticula::static_projection(g);
    auto static_proj_shuf = reticula::static_projection(shuffled);

    REQUIRE_THAT(g.vertices(), RangeEquals(shuffled.vertices()));
    REQUIRE_THAT(
      timestamps(g.edges_cause()),
      RangeEquals(timestamps(shuffled.edges_cause())));

    REQUIRE_THAT(
      timestamps(g.edges_cause()),
      RangeEquals(timestamps(shuffled.edges_cause())));
    REQUIRE(static_proj_g.edges().size() == static_proj_shuf.edges().size());
    REQUIRE_THAT(static_proj_g.edges(), !RangeEquals(static_proj_shuf.edges()));
    REQUIRE_THAT(
      reticula::connected_components(static_proj_g),
      UnorderedRangeEquals(reticula::connected_components(static_proj_shuf)));
    REQUIRE_THAT(
      reticula::degree_sequence(static_proj_g),
      UnorderedRangeEquals(reticula::degree_sequence(static_proj_shuf)));
  }
}
