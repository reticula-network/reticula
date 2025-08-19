#include <benchmark/benchmark.h>

#include <reticula/algorithms/properties.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_Properties_Density_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::density(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Properties_Density_Undirected(benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::density(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

BENCHMARK(BM_Algorithms_Properties_Density_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Properties_Density_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();
