#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/gnm.hpp>

static void BM_Generation_Gnm_RandomGnmGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto m = n*10;
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_gnm_graph(n, m, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Gnm_RandomDirectedGnmGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto m = n*10;
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_directed_gnm_graph(n, m, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_Gnm_RandomGnmGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Complexity();

BENCHMARK(BM_Generation_Gnm_RandomDirectedGnmGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Complexity();
