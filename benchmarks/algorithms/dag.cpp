#include <benchmark/benchmark.h>

#include <reticula/algorithms/dag.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_DAG_IsAcyclic_True(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_acyclic(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_DAG_IsAcyclic_False(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_acyclic(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_DAG_TryTopologicalOrder_True(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::try_topological_order(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_DAG_TryTopologicalOrder_False(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::try_topological_order(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_DAG_TopologicalOrder_True(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::topological_order(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

BENCHMARK(BM_Algorithms_DAG_IsAcyclic_True)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_DAG_IsAcyclic_False)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_DAG_TryTopologicalOrder_True)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_DAG_TryTopologicalOrder_False)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_DAG_TopologicalOrder_True)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();
