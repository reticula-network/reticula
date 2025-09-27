#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/gnm.hpp>

static void BM_Generation_Gnm_RandomGnmGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto m = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_gnm_graph(n, m, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Gnm_RandomDirectedGnmGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto m = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_directed_gnm_graph(n, m, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_Gnm_RandomGnmGraph)
  ->Args({100, 200})
  ->Args({1000, 2000})
  ->Args({5000, 10000})
  ->Args({10000, 20000})
  ->Complexity();

BENCHMARK(BM_Generation_Gnm_RandomDirectedGnmGraph)
  ->Args({100, 400})
  ->Args({1000, 4000})
  ->Args({5000, 20000})
  ->Args({10000, 40000})
  ->Complexity();
