#include <benchmark/benchmark.h>

#include <reticula/algorithms/static_reachability.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_StaticReachability_IsConnected_Undirected(
  benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_connected(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_IsStronglyConnected_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_strongly_connected(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_IsWeaklyConnected_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_weakly_connected(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_StronglyConnectedComponents_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::strongly_connected_components(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_WeaklyConnectedComponents_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::weakly_connected_components(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_ConnectedComponents_Undirected(
  benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::connected_components(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_LargestStronglyConnectedComponent_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::largest_strongly_connected_component(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_LargestWeaklyConnectedComponent_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::largest_weakly_connected_component(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_LargestConnectedComponent_Undirected(
  benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::largest_connected_component(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_IsReachable_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_reachable(net, 0, 12);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_IsReachable_Undirected(
  benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_reachable(net, 0, 12);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_IsStronglyReachable_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_strongly_reachable(net, 0, 12);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_IsWeaklyReachable_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::is_weakly_reachable(net, 0, 12);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_InComponent_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_component(net, 0);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_OutComponent_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_component(net, 0);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_InComponents_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_components(net);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_OutComponents_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_components(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_InComponentSizes_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_component_sizes(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_InComponentSizeEstimates_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_component_size_estimates(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_OutComponentSizes_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_component_sizes(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_OutComponentSizeEstimates_Directed(
  benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_component_size_estimates(net);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_InComponent_DAG(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_component(net, 0);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_OutComponent_DAG(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_component(net, 0);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_InComponents_DAG(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_components(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_OutComponents_DAG(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_components(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void
BM_Algorithms_StaticReachability_InComponentSizes_DAG(benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_component_sizes(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_InComponentSizeEstimates_DAG(
  benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::in_component_size_estimates(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_OutComponentSizes_DAG(
  benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_component_sizes(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_StaticReachability_OutComponentSizeEstimates_DAG(
  benchmark::State& state) {
  auto net = create_random_dag_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::out_component_size_estimates(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

BENCHMARK(BM_Algorithms_StaticReachability_IsConnected_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_IsStronglyConnected_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_IsWeaklyConnected_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_StronglyConnectedComponents_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_WeaklyConnectedComponents_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_ConnectedComponents_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(
  BM_Algorithms_StaticReachability_LargestStronglyConnectedComponent_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(
  BM_Algorithms_StaticReachability_LargestWeaklyConnectedComponent_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_LargestConnectedComponent_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_IsReachable_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_IsReachable_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_IsStronglyReachable_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_IsWeaklyReachable_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponent_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponent_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponents_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponents_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponentSizes_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponentSizes_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponentSizeEstimates_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponentSizeEstimates_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponent_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponent_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Args({100000, 500000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponents_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponents_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponentSizes_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponentSizes_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_InComponentSizeEstimates_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_StaticReachability_OutComponentSizeEstimates_DAG)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();
