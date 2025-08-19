#include <benchmark/benchmark.h>
#include <random>
#include <ranges>

#include <reticula/networks.hpp>

namespace {
auto generate_random_edges(std::size_t n_vertices, std::size_t n_edges) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);

  std::vector<reticula::directed_edge> edges;
  edges.reserve(n_edges);

  for (std::size_t i = 0; i < n_edges; ++i)
    edges.emplace_back(dist(gen), dist(gen));

  return edges;
}
} // namespace

static void BM_Networks_Directed_Construction(benchmark::State& state) {
  auto edges = generate_random_edges(
    static_cast<std::size_t>(state.range(0)),
    static_cast<std::size_t>(state.range(1)));

  auto vertices =
    std::ranges::iota_view{0uz, static_cast<std::size_t>(state.range(0))};

  for (auto _ : state) {
    auto net = reticula::directed_network(edges, vertices);
    benchmark::DoNotOptimize(net);
    benchmark::ClobberMemory();
  }

  state.SetComplexityN(state.range(1));
}

BENCHMARK(BM_Networks_Directed_Construction)
  ->Args({100, 500})
  ->Args({1000, 5000})
  ->Args({10000, 50000})
  ->Complexity();
