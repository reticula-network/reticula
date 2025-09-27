#include <benchmark/benchmark.h>

#include <reticula/generation/watts_strogatz.hpp>
#include <random>

static void BM_Generation_WattsStrogatz_RandomWattsStrogatzGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto k = static_cast<reticula::VertexType>(state.range(1));
  double beta = 0.1;
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_watts_strogatz_graph(n, k, beta, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_WattsStrogatz_RandomWattsStrogatzGraph)
  ->Args({100, 4})
  ->Args({1000, 6})
  ->Args({5000, 8})
  ->Args({10000, 10})
  ->Complexity();
