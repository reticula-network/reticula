#include <benchmark/benchmark.h>

#include <reticula/algorithms/distance.hpp>
#include <reticula/algorithms/static_reachability.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_Distance_ShortestPathLengthsFrom_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::size_t source = 0;

  for (auto _ : state) {
    auto lengths = reticula::shortest_path_lengths_from(net, source);
    benchmark::DoNotOptimize(lengths);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Distance_ShortestPathLengthsTo_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::size_t destination = 0;

  for (auto _ : state) {
    auto lengths = reticula::shortest_path_lengths_to(net, destination);
    benchmark::DoNotOptimize(lengths);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Algorithms_Distance_ShortestPathLengthsFrom_Undirected(
  benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::size_t source = 0;

  for (auto _ : state) {
    auto lengths = reticula::shortest_path_lengths_from(net, source);
    benchmark::DoNotOptimize(lengths);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Algorithms_Distance_Diameter_Directed(benchmark::State& state) {
  auto net = create_strong_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto diam = reticula::diameter(net);
    benchmark::DoNotOptimize(diam);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Distance_Diameter_Undirected(benchmark::State& state) {
  auto net = create_strong_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto diam = reticula::diameter(net);
    benchmark::DoNotOptimize(diam);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Distance_OutEccentricity_Directed(benchmark::State& state) {
  auto net = create_strong_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::size_t vertex = 0;

  for (auto _ : state) {
    auto ecc = reticula::out_eccentricity(net, vertex);
    benchmark::DoNotOptimize(ecc);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Distance_InEccentricity_Directed(benchmark::State& state) {
  auto net = create_strong_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::size_t vertex = 0;

  for (auto _ : state) {
    auto ecc = reticula::in_eccentricity(net, vertex);
    benchmark::DoNotOptimize(ecc);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Distance_Eccentricity_Undirected(benchmark::State& state) {
  auto net = create_connected_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::size_t vertex = 0;

  for (auto _ : state) {
    auto ecc = reticula::eccentricity(net, vertex);
    benchmark::DoNotOptimize(ecc);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Algorithms_Distance_OutRadius_Directed(benchmark::State& state) {
  auto net = create_strong_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto radius = reticula::out_radius(net);
    benchmark::DoNotOptimize(radius);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Algorithms_Distance_InRadius_Directed(benchmark::State& state) {
  auto net = create_strong_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto radius = reticula::in_radius(net);
    benchmark::DoNotOptimize(radius);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Algorithms_Distance_Radius_Undirected(benchmark::State& state) {
  auto net = create_connected_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto radius = reticula::radius(net);
    benchmark::DoNotOptimize(radius);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Algorithms_Distance_ShortestPathLengthsFrom_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_ShortestPathLengthsTo_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_ShortestPathLengthsFrom_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_Diameter_Directed)
  ->Args({50, 300})
  ->Args({100, 600})
  ->Args({200, 1200})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_Diameter_Undirected)
  ->Args({50, 300})
  ->Args({100, 600})
  ->Args({200, 1200})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_OutEccentricity_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_InEccentricity_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_Eccentricity_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_OutRadius_Directed)
  ->Args({50, 300})
  ->Args({100, 600})
  ->Args({200, 1200})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_InRadius_Directed)
  ->Args({50, 300})
  ->Args({100, 600})
  ->Args({200, 1200})
  ->Complexity();

BENCHMARK(BM_Algorithms_Distance_Radius_Undirected)
  ->Args({50, 300})
  ->Args({100, 600})
  ->Args({200, 1200})
  ->Complexity();
