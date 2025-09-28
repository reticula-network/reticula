#include <benchmark/benchmark.h>

#include <reticula/generation/deterministic.hpp>

static void BM_Generation_Deterministic_PathGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));

  for (auto _ : state) {
    auto net = reticula::path_graph(n);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Deterministic_CycleGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));

  for (auto _ : state) {
    auto net = reticula::cycle_graph(n);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Deterministic_StarGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));

  for (auto _ : state) {
    auto net = reticula::star_graph(n);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Deterministic_WheelGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));

  for (auto _ : state) {
    auto net = reticula::wheel_graph(n);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Generation_Deterministic_CompleteGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));

  for (auto _ : state) {
    auto net = reticula::complete_graph(n);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Generation_Deterministic_CompleteDirectedGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));

  for (auto _ : state) {
    auto net = reticula::complete_directed_graph(n);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Generation_Deterministic_CompleteBipartiteGraph(benchmark::State& state) {
  auto n1 = static_cast<reticula::VertexType>(state.range(0));
  auto n2 = static_cast<reticula::VertexType>(state.range(1));

  for (auto _ : state) {
    auto net = reticula::complete_bipartite_graph(n1, n2);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Generation_Deterministic_RegularRingLattice(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  auto k = static_cast<reticula::VertexType>(state.range(1));

  for (auto _ : state) {
    auto net = reticula::regular_ring_lattice(n, k);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Generation_Deterministic_SquareGridGraph(benchmark::State& state) {
  auto n = static_cast<reticula::VertexType>(state.range(0));
  std::vector<reticula::VertexType> dimensions = {n, n};

  for (auto _ : state) {
    auto net = reticula::square_grid_graph(dimensions);
    benchmark::DoNotOptimize(net);
  }

  state.SetComplexityN(state.range(0) * state.range(0));
}

BENCHMARK(BM_Generation_Deterministic_PathGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Args({100000})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_CycleGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Args({100000})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_StarGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Args({100000})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_WheelGraph)
  ->Args({100})
  ->Args({1000})
  ->Args({10000})
  ->Args({100000})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_CompleteGraph)
  ->Args({50})
  ->Args({100})
  ->Args({200})
  ->Args({400})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_CompleteDirectedGraph)
  ->Args({50})
  ->Args({100})
  ->Args({200})
  ->Args({400})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_CompleteBipartiteGraph)
  ->Args({50, 50})
  ->Args({100, 100})
  ->Args({200, 200})
  ->Args({400, 400})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_RegularRingLattice)
  ->Args({100, 6})
  ->Args({1000, 6})
  ->Args({10000, 6})
  ->Args({50000, 6})
  ->Complexity();

BENCHMARK(BM_Generation_Deterministic_SquareGridGraph)
  ->Args({10})
  ->Args({32})
  ->Args({100})
  ->Args({200})
  ->Complexity();
