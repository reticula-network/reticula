#include <benchmark/benchmark.h>

#include <random>
#include <reticula/generation/degree_sequence.hpp>

static void
BM_Generation_DegreeSequence_DegreeSequenceGraph(benchmark::State& state) {
  auto n = static_cast<std::size_t>(state.range(0));
  std::mt19937_64 gen(42);

  std::vector<reticula::VertexType> degrees(n, 4);

  for (auto _ : state) {
    auto net = reticula::degree_sequence_graph(degrees, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Generation_DegreeSequence_TryDegreeSequenceGraph(benchmark::State& state) {
  auto n = static_cast<std::size_t>(state.range(0));
  std::mt19937_64 gen(42);

  std::vector<reticula::VertexType> degrees(n, 4);

  for (auto _ : state) {
    auto net = reticula::try_degree_sequence_graph(degrees, gen, 100);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_DegreeSequence_DirectedDegreeSequenceGraph(
  benchmark::State& state) {
  auto n = static_cast<std::size_t>(state.range(0));
  std::mt19937_64 gen(42);

  std::vector<std::pair<reticula::VertexType, reticula::VertexType>> degrees;
  degrees.reserve(n);
  for (std::size_t i = 0; i < n; i++) {
    degrees.emplace_back(2, 2);
  }

  for (auto _ : state) {
    auto net = reticula::directed_degree_sequence_graph(degrees, gen);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_DegreeSequence_TryDirectedDegreeSequenceGraph(
  benchmark::State& state) {
  auto n = static_cast<std::size_t>(state.range(0));
  std::mt19937_64 gen(42);

  std::vector<std::pair<reticula::VertexType, reticula::VertexType>> degrees;
  degrees.reserve(n);
  for (std::size_t i = 0; i < n; i++) {
    degrees.emplace_back(2, 2);
  }

  for (auto _ : state) {
    auto net = reticula::try_directed_degree_sequence_graph(degrees, gen, 100);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Generation_DegreeSequence_DegreeSequenceGraph)
  ->Args({100})
  ->Args({500})
  ->Args({1000})
  ->Args({2000})
  ->Complexity();

BENCHMARK(BM_Generation_DegreeSequence_TryDegreeSequenceGraph)
  ->Args({100})
  ->Args({500})
  ->Args({1000})
  ->Args({2000})
  ->Complexity();

BENCHMARK(BM_Generation_DegreeSequence_DirectedDegreeSequenceGraph)
  ->Args({100})
  ->Args({500})
  ->Args({1000})
  ->Args({2000})
  ->Complexity();

BENCHMARK(BM_Generation_DegreeSequence_TryDirectedDegreeSequenceGraph)
  ->Args({100})
  ->Args({500})
  ->Args({1000})
  ->Args({2000})
  ->Complexity();
