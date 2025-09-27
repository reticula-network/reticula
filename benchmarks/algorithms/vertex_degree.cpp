#include <benchmark/benchmark.h>

#include <reticula/algorithms/vertex_degree.hpp>

#include "../random_network.hpp"

static void
BM_Algorithms_VertexDegree_InDegreeSequence_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto degrees = reticula::in_degree_sequence(net);
    benchmark::DoNotOptimize(degrees);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_VertexDegree_OutDegreeSequence_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto degrees = reticula::out_degree_sequence(net);
    benchmark::DoNotOptimize(degrees);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_VertexDegree_DegreeSequence_Undirected(benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto degrees = reticula::degree_sequence(net);
    benchmark::DoNotOptimize(degrees);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_VertexDegree_InDegree_Single_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));
  std::size_t vertex_idx = 0;

  for (auto _ : state) {
    auto degree = reticula::in_degree(net, vertex_idx);
    benchmark::DoNotOptimize(degree);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Algorithms_VertexDegree_InDegreeSequence_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_VertexDegree_OutDegreeSequence_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_VertexDegree_DegreeSequence_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_VertexDegree_InDegree_Single_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();
