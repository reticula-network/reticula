#pragma once

#include <random>
#include <unordered_set>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto random_gnm_graph(VertexType n, VertexType m, Gen& gen)
  -> undirected_network;

template <std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto random_directed_gnm_graph(VertexType n, VertexType m, Gen& gen)
  -> directed_network;
} // namespace reticula

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_gnm_graph(VertexType n, VertexType m, Gen& gen)
  -> undirected_network {
  if (n == 0 && m == 0)
    return {};
  if (m == 0)
    return undirected_network(
      std::views::empty<undirected_edge>, std::views::iota(VertexType{}, n));

  auto pair_n = n * (n - 1) / 2;
  if (m > pair_n)
    throw std::invalid_argument(
      "number of edges m should be less than or equal to n*(n-1)/2");

  std::unordered_set<VertexType> edge_ranks;
  edge_ranks.reserve(m);

  std::uniform_int_distribution<VertexType> dist;
  for (VertexType j = pair_n - m; j < pair_n; ++j) {
    auto t = dist(gen, decltype(dist)::param_type{0, j});
    if (!edge_ranks.insert(t).second)
      edge_ranks.insert(j);
  }

  std::vector<VertexType> rank_vec(edge_ranks.begin(), edge_ranks.end());
  std::ranges::sort(rank_vec);

  std::vector<undirected_edge> edges;
  edges.reserve(m);

  VertexType u = 0;
  VertexType start_u = 0;
  VertexType length = n - 1;

  for (auto rank : rank_vec) {
    while (length > 0 && start_u + length <= rank) {
      start_u += length;
      --length;
      ++u;
    }

    VertexType v = rank - start_u + u + 1;
    edges.emplace_back(u, v);
  }

  return undirected_network(edges, std::views::iota(VertexType{}, n));
}

template <std::uniform_random_bit_generator Gen>
auto random_directed_gnm_graph(VertexType n, VertexType m, Gen& gen)
  -> directed_network {
  if (n == 0 && m == 0)
    return {};
  if (m == 0)
    return directed_network(
      std::views::empty<directed_edge>, std::views::iota(VertexType{}, n));

  auto pair_n = n * (n - 1);
  if (m > pair_n)
    throw std::invalid_argument(
      "number of edges m should be less than or equal to n*(n-1)");

  std::unordered_set<VertexType> edge_ranks;
  edge_ranks.reserve(m);

  std::uniform_int_distribution<VertexType> dist;
  for (VertexType j = pair_n - m; j < pair_n; ++j) {
    auto t = dist(gen, decltype(dist)::param_type{0, j});
    if (!edge_ranks.insert(t).second)
      edge_ranks.insert(j);
  }

  std::vector<VertexType> rank_vec(edge_ranks.begin(), edge_ranks.end());
  std::ranges::sort(rank_vec);

  std::vector<directed_edge> edges;
  edges.reserve(m);

  for (auto rank : rank_vec) {
    VertexType u = rank / (n - 1);
    VertexType r = rank % (n - 1);
    VertexType v = r + (r >= u ? 1 : 0);
    edges.emplace_back(u, v);
  }

  return directed_network(edges, std::views::iota(VertexType{}, n));
}
} // namespace reticula
