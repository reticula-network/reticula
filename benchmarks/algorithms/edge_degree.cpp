#include <benchmark/benchmark.h>

#include <reticula/algorithms/edge_degree.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_EdgeDegree_EdgeDegreeSequence_Undirected(
  benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto degrees = reticula::edge_degree_sequence(net);
    benchmark::DoNotOptimize(degrees);
  }

  state.SetComplexityN(state.range(1));
}

BENCHMARK(BM_Algorithms_EdgeDegree_EdgeDegreeSequence_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();
