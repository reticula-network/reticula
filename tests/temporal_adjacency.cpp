#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include <reticula/edges.hpp>
#include <reticula/temporal_adjacency.hpp>

TEST_CASE(
  "simple (unbounded) temporal adjacency", "[reticula::adjacency::simple]") {
  using EdgeType = reticula::directed_temporal_edge;
  STATIC_REQUIRE(
    reticula::adjacency::adjacency<reticula::adjacency::simple, EdgeType>);

  EdgeType a(1, 2, 3);

  reticula::adjacency::simple adj;
  REQUIRE(adj.maximum_linger(1) == std::numeric_limits<double>::infinity());
  REQUIRE(adj.linger(a, 1) == std::numeric_limits<double>::infinity());
}

TEST_CASE(
  "limited_waiting_time temporal adjacency", "[reticula::adjacency::simple]") {
  using EdgeType = reticula::directed_temporal_edge;

  STATIC_REQUIRE(
    reticula::adjacency::adjacency<
      reticula::adjacency::limited_waiting_time, EdgeType>);

  EdgeType a(1, 2, 3);

  reticula::adjacency::limited_waiting_time adj1(2.5);
  REQUIRE(adj1.maximum_linger(1) == 2.5);
  REQUIRE(adj1.linger(a, 2) == 2.5);
}

TEST_CASE(
  "exponential temporal adjacency", "[reticula::adjacency::exponential]") {
  using EdgeType = reticula::directed_temporal_edge;
  double dt = 50;

  STATIC_REQUIRE(
    reticula::adjacency::adjacency<reticula::adjacency::exponential, EdgeType>);

  EdgeType a(1, 2, 3);
  reticula::adjacency::exponential adj(1.0 / dt, 0);
  auto res = adj.linger(a, 2);

  // reproducible results
  REQUIRE(reticula::adjacency::exponential(1.0 / dt, 0).linger(a, 2) == res);

  std::size_t ens = 10000;
  double t = 0;
  for (std::size_t i = 0; i < ens; i++)
    t += reticula::adjacency::exponential(1.0 / dt, i).linger(a, 2);

  double val = t / static_cast<double>(ens);
  double sigma = std::sqrt(dt);

  REQUIRE(val < dt + 3 * sigma);
  REQUIRE(val > dt - 3 * sigma);
}

TEST_CASE("geometric temporal adjacency", "[reticula::adjacency::geometric]") {
  using EdgeType = reticula::directed_temporal_edge;
  double p = 1.0 / 50.0;

  STATIC_REQUIRE(
    reticula::adjacency::adjacency<reticula::adjacency::geometric, EdgeType>);

  EdgeType a(1, 2, 3);
  reticula::adjacency::geometric adj(p, 0);
  auto res = adj.linger(a, 2);

  // reproducible results
  REQUIRE(reticula::adjacency::geometric(p, 0).linger(a, 2) == res);

  std::size_t ens = 10000;
  double t = 0;
  for (std::size_t i = 0; i < ens; i++)
    t += reticula::adjacency::geometric(p, i).linger(a, 2) +
         1; // The linger time is inclusive of the last timestamp

  double val = static_cast<double>(t) / static_cast<double>(ens);
  double sigma = std::sqrt(1.0 / p);

  REQUIRE(val < 1.0 / p + 3.0 * sigma);
  REQUIRE(val > 1.0 / p - 3.0 * sigma);

  REQUIRE(adj.maximum_linger(1) == std::numeric_limits<double>::infinity());
}
