#include <benchmark/benchmark.h>
#include <random>
#include <vector>

#include <reticula/algorithms/graphicallity.hpp>

namespace {
auto create_random_degree_sequence(std::size_t n_vertices) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);

  std::vector<std::size_t> degrees;
  degrees.reserve(n_vertices);

  for (std::size_t i = 0; i < n_vertices; ++i) {
    degrees.push_back(dist(gen));
  }

  return degrees;
}

auto create_graphic_degree_sequence(std::size_t n_vertices) {
  std::vector<std::size_t> degrees(n_vertices, 2);

  // Ensure even sum for graphicality
  if ((n_vertices * 2) % 2 != 0 && n_vertices > 0) {
    degrees[0] = 3;
  }

  return degrees;
}

auto create_random_degree_pair_sequence(std::size_t n_vertices) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);

  std::vector<std::pair<std::size_t, std::size_t>> degree_pairs;
  degree_pairs.reserve(n_vertices);

  for (std::size_t i = 0; i < n_vertices; ++i) {
    degree_pairs.emplace_back(dist(gen), dist(gen));
  }

  return degree_pairs;
}

auto create_digraphic_degree_pair_sequence(std::size_t n_vertices) {
  std::vector<std::pair<std::size_t, std::size_t>> degree_pairs;
  degree_pairs.reserve(n_vertices);

  for (std::size_t i = 0; i < n_vertices; ++i) {
    degree_pairs.emplace_back(2, 2);
  }

  return degree_pairs;
}
} // namespace

static void
BM_Algorithms_Graphicallity_IsGraphic_True(benchmark::State& state) {
  auto degrees =
    create_graphic_degree_sequence(static_cast<std::size_t>(state.range(0)));

  for (auto _ : state) {
    auto result = reticula::is_graphic(degrees);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Graphicallity_IsGraphic_False(benchmark::State& state) {
  auto degrees =
    create_random_degree_sequence(static_cast<std::size_t>(state.range(0)));

  for (auto _ : state) {
    auto result = reticula::is_graphic(degrees);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Graphicallity_IsDigraphic_True(benchmark::State& state) {
  auto degree_pairs = create_digraphic_degree_pair_sequence(
    static_cast<std::size_t>(state.range(0)));

  for (auto _ : state) {
    auto result = reticula::is_digraphic(degree_pairs);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0));
}

static void
BM_Algorithms_Graphicallity_IsDigraphic_False(benchmark::State& state) {
  auto degree_pairs = create_random_degree_pair_sequence(
    static_cast<std::size_t>(state.range(0)));

  for (auto _ : state) {
    auto result = reticula::is_digraphic(degree_pairs);
    benchmark::DoNotOptimize(result);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Algorithms_Graphicallity_IsGraphic_True)
  ->Arg(100)
  ->Arg(1000)
  ->Arg(10000)
  ->Arg(100000)
  ->Complexity();

BENCHMARK(BM_Algorithms_Graphicallity_IsGraphic_False)
  ->Arg(100)
  ->Arg(1000)
  ->Arg(10000)
  ->Arg(100000)
  ->Complexity();

BENCHMARK(BM_Algorithms_Graphicallity_IsDigraphic_True)
  ->Arg(100)
  ->Arg(1000)
  ->Arg(10000)
  ->Arg(100000)
  ->Complexity();

BENCHMARK(BM_Algorithms_Graphicallity_IsDigraphic_False)
  ->Arg(100)
  ->Arg(1000)
  ->Arg(10000)
  ->Arg(100000)
  ->Complexity();
