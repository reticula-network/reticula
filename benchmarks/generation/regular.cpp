#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/regular.hpp>

static void
BM_Generation_Regular_TryRandomRegularGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto k = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::try_random_regular_graph(n, k, gen, 1000);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Regular_RandomRegularGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto k = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_regular_graph(n, k, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Generation_Regular_TryRandomDirectedRegularGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto k = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::try_random_directed_regular_graph(n, k, gen, 1000);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Generation_Regular_RandomDirectedRegularGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto k = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_directed_regular_graph(n, k, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_Regular_TryRandomRegularGraph)
  ->Args({100, 4})
  ->Args({500, 6})
  ->Args({1000, 8})
  ->Args({2000, 10})
  ->Complexity();

BENCHMARK(BM_Generation_Regular_RandomRegularGraph)
  ->Args({100, 4})
  ->Args({500, 6})
  ->Args({1000, 8})
  ->Args({2000, 10})
  ->Complexity();

BENCHMARK(BM_Generation_Regular_TryRandomDirectedRegularGraph)
  ->Args({100, 4})
  ->Args({500, 6})
  ->Args({1000, 8})
  ->Args({2000, 10})
  ->Complexity();

BENCHMARK(BM_Generation_Regular_RandomDirectedRegularGraph)
  ->Args({100, 4})
  ->Args({500, 6})
  ->Args({1000, 8})
  ->Args({2000, 10})
  ->Complexity();
