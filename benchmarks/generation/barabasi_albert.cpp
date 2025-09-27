#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/barabasi_albert.hpp>

static void BM_Generation_BarabasiAlbert_RandomBarabasiAlbertGraph(
  benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto m = static_cast<reticula::VertexType>(state.range(1));
  std::mt19937_64 gen(42);

  for (auto _ : state) {
    auto net = reticula::random_barabasi_albert_graph(n, m, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_BarabasiAlbert_RandomBarabasiAlbertGraph)
  ->Args({100, 2})
  ->Args({1000, 5})
  ->Args({10000, 10})
  ->Args({50000, 15})
  ->Complexity();
