#include <random>
#include <unordered_set>

#include <reticula/networks.hpp>

auto create_random_directed_network(std::size_t n_vertices, std::size_t n_edges)
  -> reticula::directed_network {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);

  std::unordered_set<reticula::directed_edge> edges;
  edges.reserve(n_edges);

  while (edges.size() < n_edges) {
    auto v1 = dist(gen);
    auto v2 = dist(gen);
    if (v1 != v2)
      edges.emplace(dist(gen), dist(gen));
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices};
  return reticula::directed_network(edges, vertices);
}

auto create_random_undirected_network(
  std::size_t n_vertices, std::size_t n_edges) -> reticula::undirected_network {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);

  std::unordered_set<reticula::undirected_edge> edges;
  edges.reserve(n_edges);

  while (edges.size() < n_edges) {
    auto v1 = dist(gen);
    auto v2 = dist(gen);
    if (v1 != v2)
      edges.emplace(v1, v2);
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices};
  return reticula::undirected_network(edges, vertices);
}

auto create_random_dag_network(std::size_t n_vertices, std::size_t n_edges)
  -> reticula::directed_network {
  if (n_vertices < 10)
    throw std::invalid_argument("n_vertices must be at least 10");

  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);
  std::uniform_int_distribution<std::size_t> dist2(1, n_vertices / 10);

  std::unordered_set<reticula::directed_edge> edges;
  edges.reserve(n_edges);

  while (edges.size() < n_edges) {
    auto v1 = dist(gen);
    auto diff = dist2(gen);
    auto v2 = v1 + diff;
    if (v2 < n_vertices)
      edges.emplace(v1, v2);
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices};
  return reticula::directed_network(edges, vertices);
}

auto create_bipartite_network(
  std::size_t n_vertices_a, std::size_t n_vertices_b, std::size_t n_edges)
  -> reticula::undirected_network {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist_a(0, n_vertices_a - 1);
  std::uniform_int_distribution<std::size_t> dist_b(
    n_vertices_a, n_vertices_a + n_vertices_b - 1);

  std::unordered_set<reticula::undirected_edge> edges;
  edges.reserve(n_edges);

  // Create edges only between the two partitions
  while (edges.size() < n_edges) {
    auto v1 = dist_a(gen);
    auto v2 = dist_b(gen);
    edges.emplace(v1, v2);
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices_a + n_vertices_b};
  return reticula::undirected_network(edges, vertices);
}
