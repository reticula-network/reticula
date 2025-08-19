#include <benchmark/benchmark.h>

#include <reticula/algorithms/bipartite.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_Bipartite_IsBipartite_True(benchmark::State& state) {
  auto net = create_bipartite_network(
    static_cast<std::size_t>(state.range(0)) / 2,
    static_cast<std::size_t>(state.range(0)) / 2,
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_bipartite(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Bipartite_IsBipartite_False(benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_bipartite(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_Bipartite_TryTwoColouring_True(benchmark::State& state) {
  auto net = create_bipartite_network(
    static_cast<std::size_t>(state.range(0)) / 2,
    static_cast<std::size_t>(state.range(0)) / 2,
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::try_two_colouring(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_Bipartite_TryTwoColouring_False(benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::try_two_colouring(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Bipartite_TwoColouring_True(benchmark::State& state) {
  auto net = create_bipartite_network(
    static_cast<std::size_t>(state.range(0)) / 2,
    static_cast<std::size_t>(state.range(0)) / 2,
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::two_colouring(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

BENCHMARK(BM_Algorithms_Bipartite_IsBipartite_True)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Bipartite_IsBipartite_False)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Bipartite_TryTwoColouring_True)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Bipartite_TryTwoColouring_False)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Bipartite_TwoColouring_True)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();
