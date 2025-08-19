#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_gnp_graph(VertexType n, double p, Gen& gen) -> undirected_network;

template <std::uniform_random_bit_generator Gen>
auto random_directed_gnp_graph(VertexType n, double p, Gen& gen)
  -> directed_network;
} // namespace reticula

#include <reticula/generation/deterministic.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_gnp_graph(VertexType n, double p, Gen& gen) -> undirected_network {
  if (p > 1.0 || p < 0.0)
    throw std::invalid_argument(
      "edge probability p should be in [0,1] interval");

  if (n == 0 || p == 0)
    return undirected_network(
      std::views::empty<undirected_edge>, std::views::iota(VertexType{}, n));

  if (p == 1.0)
    return complete_graph(n);

  std::vector<undirected_edge> edges;
  edges.reserve(
    static_cast<std::size_t>(
      static_cast<double>(n - 1) * static_cast<double>(n / 2) * p));

  std::uniform_real_distribution rd;

  double lp = std::log1p(-p);
  VertexType v = 1;
  auto w = static_cast<VertexType>(std::floor(std::log1p(-rd(gen)) / lp));

  while (v < n) {
    if (w < v) {
      edges.emplace_back(v, w);
      double lr = std::log1p(-rd(gen));
      w = w + static_cast<VertexType>(1 + std::floor(lr / lp));
    } else {
      w -= v;
      ++v;
    }
  }

  return undirected_network(edges, std::views::iota(VertexType{}, n));
}

template <std::uniform_random_bit_generator Gen>
auto random_directed_gnp_graph(VertexType n, double p, Gen& gen)
  -> directed_network {
  if (p > 1.0 || p < 0.0)
    throw std::invalid_argument(
      "edge probability p should be in [0,1] interval");

  if (n == 0 || p == 0)
    return directed_network(
      std::views::empty<directed_edge>, std::views::iota(VertexType{}, n));

  if (p == 1.0)
    return complete_directed_graph(n);

  std::vector<directed_edge> edges;
  edges.reserve(
    static_cast<std::size_t>(
      static_cast<double>(n - 1) * static_cast<double>(n) * p));

  std::uniform_real_distribution rd;

  double lp = std::log1p(-p);
  VertexType v = 0;
  auto w = static_cast<VertexType>(std::floor(std::log1p(-rd(gen)) / lp));

  while (v < n) {
    while (w >= (n - 1) && v < n) {
      w -= (n - 1);
      ++v;
    }
    if (v >= n)
      break;

    VertexType u = (w < v) ? w : (w + 1);
    edges.emplace_back(v, u);

    double lr = std::log1p(-rd(gen));
    w += static_cast<VertexType>(1 + std::floor(lr / lp));
  }

  return directed_network(edges, std::views::iota(VertexType{}, n));
}
} // namespace reticula
