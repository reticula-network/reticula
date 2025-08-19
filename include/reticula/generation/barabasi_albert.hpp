#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_barabasi_albert_graph(VertexType n, VertexType m, Gen& gen)
  -> undirected_network;
}

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_barabasi_albert_graph(VertexType n, VertexType m, Gen& gen)
  -> undirected_network {
  if (n <= m || m < 1)
    throw std::invalid_argument("BA network must have m >= 1 and n > m");

  std::vector<undirected_edge> edges;
  edges.reserve(m * (n - m));

  std::vector<VertexType> repeated_nodes;
  repeated_nodes.reserve(2 * m * (n - m));

  for (VertexType i = 0; i < m; i++) {
    edges.emplace_back(i, m);
    repeated_nodes.push_back(i);
    repeated_nodes.push_back(m);
  }

  std::unordered_set<VertexType> targets;
  targets.reserve(static_cast<std::size_t>(m));

  for (VertexType u = m + 1; u < n; u++) {
    targets.clear();

    std::uniform_int_distribution<std::size_t> dist(
      0, repeated_nodes.size() - 1);

    while (targets.size() < static_cast<std::size_t>(m))
      targets.insert(repeated_nodes[dist(gen)]);

    for (const auto& v : targets) {
      edges.emplace_back(u, v);
      repeated_nodes.push_back(v);
      repeated_nodes.push_back(u);
    }
  }

  return undirected_network(edges, std::views::iota(VertexType{}, n));
}
} // namespace reticula
