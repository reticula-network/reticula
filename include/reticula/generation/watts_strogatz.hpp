#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_watts_strogatz_graph(
  VertexType n, VertexType k, double beta, Gen& gen) -> undirected_network;
}

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_watts_strogatz_graph(
  VertexType n, VertexType k, double beta, Gen& gen) -> undirected_network {
  if (k >= n)
    throw std::invalid_argument("k must be less than n");

  if (k % 2 != 0)
    throw std::invalid_argument("k must be divisible by two");

  if (beta < 0.0 || beta > 1.0)
    throw std::invalid_argument("rewiring probability p should be in [0,1]");

  if (n == 0)
    return {};

  std::vector<std::unordered_set<VertexType>> adj(n);

  std::bernoulli_distribution rewire(beta);
  std::uniform_int_distribution<VertexType> vert_dist(0, n - 1);

  std::vector<undirected_edge> edges;
  edges.reserve(n * k / 2);

  for (VertexType i = {}; i < n; ++i) {
    auto& ai = adj[static_cast<std::size_t>(i)];
    for (VertexType d = 1; d <= k / 2; ++d) {
      VertexType j = (i + d) % n;
      VertexType v = j;

      if (beta > 0.0 && rewire(gen)) {
        VertexType r = vert_dist(gen);
        while (r == i || ai.contains(r))
          r = vert_dist(gen);
        v = r;
      }

      if (!ai.contains(v)) {
        adj[i].insert(v);
        adj[v].insert(i);

        edges.emplace_back(i, v);
      }
    }
  }

  return undirected_network(edges, std::views::iota(VertexType{}, n));
}
} // namespace reticula
