#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include <reticula/temporal_edges.hpp>
#include <reticula/temporal_adjacency.hpp>

TEST_CASE("simple (unbounded) temporal adjacency with double times",
    "[reticula::temporal_adjacency::simple]") {
  using EdgeType = reticula::directed_temporal_edge<int, double>;
  STATIC_REQUIRE(reticula::temporal_adjacency::temporal_adjacency<
    reticula::temporal_adjacency::simple<EdgeType>>);

  EdgeType a(1, 2, 3);

  reticula::temporal_adjacency::simple<EdgeType> adj;
  REQUIRE(adj.maximum_linger(1) == std::numeric_limits<
      typename EdgeType::TimeType>::infinity());
  REQUIRE(adj.linger(a, 1) == std::numeric_limits<
      typename EdgeType::TimeType>::infinity());
  REQUIRE(adj.infinite_linger(a, 1));
}

TEST_CASE("simple (unbounded) temporal adjacency with int times",
    "[reticula::temporal_adjacency::simple]") {
  using EdgeType = reticula::directed_temporal_edge<int, int>;
  STATIC_REQUIRE(reticula::temporal_adjacency::temporal_adjacency<
    reticula::temporal_adjacency::simple<EdgeType>>);

  EdgeType a(1, 2, 3);

  reticula::temporal_adjacency::simple<EdgeType> adj;
  REQUIRE(adj.maximum_linger(1) == std::numeric_limits<
      typename EdgeType::TimeType>::max());
  REQUIRE(adj.linger(a, 1) == std::numeric_limits<
      typename EdgeType::TimeType>::max());
  REQUIRE(adj.infinite_linger(a, 1));
}


TEST_CASE("limited_waiting_time temporal adjacency",
    "[reticula::temporal_adjacency::simple]") {
  using EdgeType = reticula::directed_temporal_edge<int, int>;

  STATIC_REQUIRE(reticula::temporal_adjacency::temporal_adjacency<
    reticula::temporal_adjacency::limited_waiting_time<EdgeType>>);

  EdgeType a(1, 2, 3);

  reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj1(2);
  REQUIRE(adj1.maximum_linger(1) == 2);
  REQUIRE(adj1.linger(a, 2) == 2);
  REQUIRE_FALSE(adj1.infinite_linger(a, 2));

  reticula::temporal_adjacency::limited_waiting_time<EdgeType> adj2(3);
  REQUIRE(adj2.maximum_linger(1) == 3);
  REQUIRE(adj2.linger(a, 2) == 3);
  REQUIRE_FALSE(adj2.infinite_linger(a, 2));

  using FPEdgeType = reticula::directed_temporal_edge<int, double>;
  reticula::temporal_adjacency::limited_waiting_time<FPEdgeType> adj3(
    std::numeric_limits<typename FPEdgeType::TimeType>::infinity());

  FPEdgeType b(1, 2, 3);
  REQUIRE(adj3.maximum_linger(1) ==
          std::numeric_limits<typename FPEdgeType::TimeType>::infinity());
  REQUIRE(adj3.linger(b, 2) ==
          std::numeric_limits<typename FPEdgeType::TimeType>::infinity());
  REQUIRE(adj3.infinite_linger(b, 2));
}

TEST_CASE("exponential temporal adjacency",
    "[reticula::temporal_adjacency::exponential]") {
  using EdgeType = reticula::directed_temporal_edge<int, double>;
  typename EdgeType::TimeType dt = 50;

  STATIC_REQUIRE(reticula::temporal_adjacency::temporal_adjacency<
    reticula::temporal_adjacency::exponential<EdgeType>>);

  EdgeType a(1, 2, 3);
  reticula::temporal_adjacency::exponential<EdgeType> adj(1.0/dt, 0);
  auto res = adj.linger(a, 2);

  // reporducible results
  REQUIRE(
      reticula::temporal_adjacency::exponential<EdgeType>(1.0/dt, 0)
        .linger(a, 2) == res);

  std::size_t ens = 10000;
  typename EdgeType::TimeType t = 0;
  for (std::size_t i = 0; i < ens; i++)
    t += reticula::temporal_adjacency::exponential<EdgeType>(
        1.0/dt, i).linger(a, 2);

  double val = t/static_cast<double>(ens);
  double sigma = std::sqrt(dt);

  REQUIRE(val < dt + 3*sigma);
  REQUIRE(val > dt - 3*sigma);

  REQUIRE(adj.maximum_linger(1) == std::numeric_limits<
          typename EdgeType::TimeType>::max());
  REQUIRE_FALSE(adj.infinite_linger(a, 2));
}

TEST_CASE("geometric temporal adjacency",
    "[reticula::temporal_adjacency::geometric]") {
  using EdgeType = reticula::directed_temporal_edge<int, int>;
  double p = 1.0/50.0;

  STATIC_REQUIRE(reticula::temporal_adjacency::temporal_adjacency<
    reticula::temporal_adjacency::geometric<EdgeType>>);

  EdgeType a(1, 2, 3);
  reticula::temporal_adjacency::geometric<EdgeType> adj(p, 0);
  auto res = adj.linger(a, 2);

  // reporducible results
  REQUIRE(
      reticula::temporal_adjacency::geometric<EdgeType>(p, 0)
        .linger(a, 2) == res);

  std::size_t ens = 10000;
  typename EdgeType::TimeType t = 0;
  for (std::size_t i = 0; i < ens; i++)
    t += reticula::temporal_adjacency::geometric<EdgeType>(p, i).linger(a, 2)
      + 1;  // The linger time is inclusive of the last timestamp

  double val = static_cast<double>(t)/static_cast<double>(ens);
  double sigma = std::sqrt(1.0/p);

  REQUIRE(val < 1.0/p + 3.0*sigma);
  REQUIRE(val > 1.0/p - 3.0*sigma);

  REQUIRE(adj.maximum_linger(1) == std::numeric_limits<
          typename EdgeType::TimeType>::max());
  REQUIRE_FALSE(adj.infinite_linger(a, 2));
}
