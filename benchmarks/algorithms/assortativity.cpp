#include <benchmark/benchmark.h>
#include <random>

#include <reticula/algorithms/assortativity.hpp>

#include "../random_network.hpp"

static void BM_Algorithms_Assortativity_DegreeAssortativity_Undirected(benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::degree_assortativity(net);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Assortativity_DegreeAssortativity_Directed_InIn(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::degree_assortativity(net, reticula::direction::in, reticula::direction::in);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Assortativity_DegreeAssortativity_Directed_OutOut(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::degree_assortativity(net, reticula::direction::out, reticula::direction::out);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Assortativity_DegreeAssortativity_Directed_InOut(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  for (auto _ : state) {
    auto result = reticula::degree_assortativity(net, reticula::direction::in, reticula::direction::out);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Assortativity_AttributeAssortativity_Undirected(benchmark::State& state) {
  auto net = create_random_undirected_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::mt19937 gen(42);
  std::uniform_real_distribution<double> attr_dist(0.0, 1.0);
  
  auto attr_fun = [&gen, &attr_dist](reticula::VertexType) mutable {
    return attr_dist(gen);
  };

  for (auto _ : state) {
    auto result = reticula::attribute_assortativity(net, attr_fun);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

static void BM_Algorithms_Assortativity_AttributeAssortativity_Directed(benchmark::State& state) {
  auto net = create_random_directed_network(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  std::mt19937 gen1(42), gen2(43);
  std::uniform_real_distribution<double> attr_dist(0.0, 1.0);
  
  auto mutator_attr_fun = [&gen1, &attr_dist](reticula::VertexType) mutable {
    return attr_dist(gen1);
  };
  
  auto mutated_attr_fun = [&gen2, &attr_dist](reticula::VertexType) mutable {
    return attr_dist(gen2);
  };

  for (auto _ : state) {
    auto result = reticula::attribute_assortativity(net, mutator_attr_fun, mutated_attr_fun);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0) + state.range(1));
}

BENCHMARK(BM_Algorithms_Assortativity_DegreeAssortativity_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Assortativity_DegreeAssortativity_Directed_InIn)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Assortativity_DegreeAssortativity_Directed_OutOut)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Assortativity_DegreeAssortativity_Directed_InOut)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Assortativity_AttributeAssortativity_Undirected)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();

BENCHMARK(BM_Algorithms_Assortativity_AttributeAssortativity_Directed)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();
