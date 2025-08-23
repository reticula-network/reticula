#include <benchmark/benchmark.h>

#include <reticula/algorithms/temporal_reachability.hpp>
#include <reticula/networks.hpp>
#include <reticula/temporal_adjacency.hpp>

#include "../random_network.hpp"

using benchmark::ComplexityN;

static void
BM_Algorithms_TemporalReachability_OutCluster_Single(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::out_cluster(net, 0uz, 5.0, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void
BM_Algorithms_TemporalReachability_InCluster_Single(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::in_cluster(net, 0uz, 5.0, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void
BM_Algorithms_TemporalReachability_WeakCluster_Single(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::weak_cluster(net, 0uz, 5.0, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void
BM_Algorithms_TemporalReachability_OutClusters_All(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::out_clusters(net, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void
BM_Algorithms_TemporalReachability_InClusters_All(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::in_clusters(net, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void
BM_Algorithms_TemporalReachability_WeakClusters_All(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::weak_clusters(net, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void BM_Algorithms_TemporalReachability_OutClusterSizes_All(
  benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::out_cluster_sizes(net, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void
BM_Algorithms_TemporalReachability_InClusterSizes_All(benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::in_cluster_sizes(net, adj);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

static void BM_Algorithms_TemporalReachability_OutClusterSizeEstimates_All(
  benchmark::State& state) {
  auto net = create_random_temporal_network(
    static_cast<std::size_t>(state.range(0)), 2.0);

  auto adj = reticula::adjacency::limited_waiting_time(2.1);

  for (auto _ : state) {
    auto result = reticula::out_cluster_size_estimates(net, adj, 1.0, 42);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(static_cast<ComplexityN>(net.edges().size()));
}

BENCHMARK(BM_Algorithms_TemporalReachability_OutCluster_Single)
  ->Arg(32)
  ->Arg(64)
  ->Arg(128)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_InCluster_Single)
  ->Arg(32)
  ->Arg(64)
  ->Arg(128)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_WeakCluster_Single)
  ->Arg(32)
  ->Arg(64)
  ->Arg(128)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_OutClusters_All)
  ->Arg(8)
  ->Arg(11)
  ->Arg(16)
  ->Arg(23)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_InClusters_All)
  ->Arg(8)
  ->Arg(11)
  ->Arg(16)
  ->Arg(23)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_WeakClusters_All)
  ->Arg(8)
  ->Arg(16)
  ->Arg(32)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_OutClusterSizes_All)
  ->Arg(8)
  ->Arg(11)
  ->Arg(16)
  ->Arg(23)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_InClusterSizes_All)
  ->Arg(8)
  ->Arg(11)
  ->Arg(16)
  ->Arg(23)
  ->Complexity();

BENCHMARK(BM_Algorithms_TemporalReachability_OutClusterSizeEstimates_All)
  ->Arg(8)
  ->Arg(11)
  ->Arg(16)
  ->Arg(23)
  ->Complexity();
