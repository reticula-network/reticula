#include <cmath>
#include <limits>

#include <catch2/catch.hpp>

#include <dag/temporal_edges.hpp>
#include <dag/temporal_adjacency.hpp>

TEST_CASE("simple (unbounded) temporal adjacency",
    "[dag::temporal_adjacency::simple]") {
  using EdgeType = dag::directed_temporal_edge<int, double>;
  STATIC_REQUIRE(dag::temporal_adjacency::temporal_adjacency<
    dag::temporal_adjacency::simple<EdgeType>>);

  EdgeType a(1, 2, 3);

  dag::temporal_adjacency::simple<EdgeType> adj;
  REQUIRE(adj.cutoff_time(a) == std::numeric_limits<
      typename EdgeType::TimeType>::infinity());
}


TEST_CASE("limited_waiting_time temporal adjacency",
    "[dag::temporal_adjacency::simple]") {
  using EdgeType = dag::directed_temporal_edge<int, int>;

  STATIC_REQUIRE(dag::temporal_adjacency::temporal_adjacency<
    dag::temporal_adjacency::limited_waiting_time<EdgeType>>);

  EdgeType a(1, 2, 3);
  dag::temporal_adjacency::limited_waiting_time<EdgeType> adj1(2);
  dag::temporal_adjacency::limited_waiting_time<EdgeType> adj2(3);

  REQUIRE(adj1.cutoff_time(a) == 5);
  REQUIRE(adj2.cutoff_time(a) == 6);
}

TEST_CASE("exponential temporal adjacency",
    "[dag::temporal_adjacency::exponential]") {
  using EdgeType = dag::directed_temporal_edge<int, double>;
  typename EdgeType::TimeType dt = 50;

  STATIC_REQUIRE(dag::temporal_adjacency::temporal_adjacency<
    dag::temporal_adjacency::exponential<EdgeType>>);

  EdgeType a(1, 2, 3);
  dag::temporal_adjacency::exponential<EdgeType> adj(dt, 0);
  auto res = adj.cutoff_time(a);

  // reporducible results
  REQUIRE(
      dag::temporal_adjacency::exponential<EdgeType>(dt, 0)
        .cutoff_time(a) == res);

  std::size_t ens = 10000;
  typename EdgeType::TimeType t = 0;
  for (std::size_t i = 0; i < ens; i++)
    t +=
      dag::temporal_adjacency::exponential<EdgeType>(dt, i).cutoff_time(a) -
        a.effect_time();

  double val = static_cast<double>(t)/static_cast<double>(ens);
  double mean = static_cast<double>(dt);
  double sigma = std::sqrt(mean);

  REQUIRE(val < mean + 3*sigma);
  REQUIRE(val > mean - 3*sigma);
}
