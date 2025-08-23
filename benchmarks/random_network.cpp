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
      edges.emplace(v1, v2);
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices};
  return reticula::directed_network(edges, vertices);
}

auto create_strong_random_directed_network(
  std::size_t n_vertices, std::size_t n_edges) -> reticula::directed_network {
  if (n_vertices < 3)
    throw std::invalid_argument("n_vertices must be at least 3");

  std::unordered_set<reticula::directed_edge> edges;
  for (std::size_t i = 0; i < n_vertices; ++i)
    edges.emplace(i, (i + 1) % n_vertices);

  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);
  while (edges.size() < n_edges) {
    auto v1 = dist(gen);
    auto v2 = dist(gen);
    if (v1 != v2)
      edges.emplace(v1, v2);
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

auto create_connected_random_undirected_network(
  std::size_t n_vertices, std::size_t n_edges) -> reticula::undirected_network {
  if (n_vertices < 3)
    throw std::invalid_argument("n_vertices must be at least 3");

  std::unordered_set<reticula::undirected_edge> edges;
  for (std::size_t i = 0; i < n_vertices - 1; ++i)
    edges.emplace(i, i + 1);

  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> dist(0, n_vertices - 1);
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

auto create_random_temporal_network(std::size_t n_vertices, double mean_iet)
  -> reticula::undirected_temporal_network {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> vertex_dist(0, n_vertices - 1);
  double t_max = 10.0;
  double n_links =
    static_cast<double>(n_vertices) * static_cast<double>(n_vertices - 1) / 2.0;
  double n_events = (t_max / mean_iet) * n_links;
  std::uniform_real_distribution<double> time_dist(0.0, 100.0);

  std::unordered_set<reticula::undirected_temporal_edge> edges;
  edges.reserve(static_cast<std::size_t>(n_events));

  while (edges.size() < static_cast<std::size_t>(n_events)) {
    auto v1 = vertex_dist(gen);
    auto v2 = vertex_dist(gen);
    if (v1 != v2)
      edges.emplace(v1, v2, time_dist(gen));
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices};
  return reticula::undirected_temporal_network(edges, vertices);
}

auto create_random_directed_temporal_network(
  std::size_t n_vertices, double mean_iet)
  -> reticula::directed_temporal_network {
  std::mt19937 gen(42);
  std::uniform_int_distribution<std::size_t> vertex_dist(0, n_vertices - 1);
  std::uniform_real_distribution<double> time_dist(0.0, 100.0);

  double t_max = 10.0;
  double n_links =
    static_cast<double>(n_vertices) * static_cast<double>(n_vertices - 1);
  double n_events = (t_max / mean_iet) * n_links;

  std::unordered_set<reticula::directed_temporal_edge> edges;
  edges.reserve(static_cast<std::size_t>(n_events));

  while (edges.size() < static_cast<std::size_t>(n_events)) {
    auto v1 = vertex_dist(gen);
    auto v2 = vertex_dist(gen);
    if (v1 != v2)
      edges.emplace(v1, v2, time_dist(gen));
  }

  auto vertices = std::ranges::iota_view{0uz, n_vertices};
  return reticula::directed_temporal_network(edges, vertices);
}
