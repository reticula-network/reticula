#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/fully_mixed.hpp>

static void BM_Generation_FullyMixed_RandomFullyMixedTemporalNetwork(
  benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  double rate = 0.1;
  auto max_t = static_cast<double>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net =
      reticula::random_fully_mixed_temporal_network(n, rate, max_t, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0) * state.range(1));
}

static void BM_Generation_FullyMixed_RandomDirectedFullyMixedTemporalNetwork(
  benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  double rate = 0.1;
  auto max_t = static_cast<double>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_directed_fully_mixed_temporal_network(
      n, rate, max_t, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0) * state.range(1));
}

BENCHMARK(BM_Generation_FullyMixed_RandomFullyMixedTemporalNetwork)
  ->Args({100, 1000})
  ->Args({200, 2000})
  ->Args({500, 5000})
  ->Args({1000, 10000})
  ->Complexity();

BENCHMARK(BM_Generation_FullyMixed_RandomDirectedFullyMixedTemporalNetwork)
  ->Args({100, 1000})
  ->Args({200, 2000})
  ->Args({500, 5000})
  ->Args({1000, 10000})
  ->Complexity();
