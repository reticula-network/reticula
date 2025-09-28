#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/fully_mixed.hpp>

static void BM_Generation_FullyMixed_RandomFullyMixedTemporalNetwork(
  benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  double rate = 1.0/static_cast<double>(n);
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
  double rate = 1.0/static_cast<double>(n);
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
  ->Args({100, 200})
  ->Args({200, 200})
  ->Args({500, 200})
  ->Complexity();

BENCHMARK(BM_Generation_FullyMixed_RandomDirectedFullyMixedTemporalNetwork)
  ->Args({100, 200})
  ->Args({200, 200})
  ->Args({500, 200})
  ->Complexity();
