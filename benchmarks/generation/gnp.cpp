#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/gnp.hpp>

static void BM_Generation_Gnp_RandomGnpGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  double p = 10.0 / static_cast<double>(state.range(0));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_gnp_graph(n, p, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Gnp_RandomDirectedGnpGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  double p = 10.0 / static_cast<double>(state.range(0));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_directed_gnp_graph(n, p, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_Gnp_RandomGnpGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Complexity();

BENCHMARK(BM_Generation_Gnp_RandomDirectedGnpGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Complexity();
