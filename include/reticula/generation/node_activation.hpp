#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_node_activation_temporal_network(
  undirected_network base_network, double max_t, IETDist iet_dist,
  ResDist res_dist, Gen& gen, std::size_t size_hint = 0)
  -> undirected_temporal_network;

template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_node_activation_temporal_network(
  directed_network base_network, double max_t, IETDist iet_dist,
  ResDist res_dist, Gen& gen, std::size_t size_hint)
  -> directed_temporal_network;

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_node_activation_temporal_network(
  undirected_network base_network, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint = 0) -> undirected_temporal_network;

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_node_activation_temporal_network(
  directed_network base_network, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> directed_temporal_network;
} // namespace reticula

#include <vector>

namespace reticula {

template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_node_activation_temporal_network(
  undirected_network base_net, double max_t, IETDist iet_dist, ResDist res_dist,
  Gen& gen, std::size_t size_hint) -> undirected_temporal_network {
  std::vector<undirected_temporal_edge> edges;
  if (size_hint > 0)
    edges.reserve(size_hint);

  for (const auto& v : base_net.vertices()) {
    auto links = base_net.out_edges(v);
    if (links.empty())
      continue;

    std::uniform_int_distribution<std::size_t> idx(0, links.size() - 1);
    for (auto t = res_dist(gen); t < max_t; t += iet_dist(gen))
      edges.emplace_back(links[idx(gen)], t);
  }

  return undirected_temporal_network{edges, base_net.vertices()};
}

template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_node_activation_temporal_network(
  directed_network base_net, double max_t, IETDist iet_dist, ResDist res_dist,
  Gen& gen, std::size_t size_hint) -> directed_temporal_network {
  std::vector<directed_temporal_edge> edges;
  if (size_hint > 0)
    edges.reserve(size_hint);

  for (const auto& v : base_net.vertices()) {
    auto links = base_net.out_edges(v);
    if (links.empty())
      continue;

    std::uniform_int_distribution<std::size_t> idx(0, links.size() - 1);
    for (auto t = res_dist(gen); t < max_t; t += iet_dist(gen))
      edges.emplace_back(links[idx(gen)], t);
  }

  return directed_temporal_network{edges, base_net.vertices()};
}

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_node_activation_temporal_network(
  undirected_network base_net, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> undirected_temporal_network {
  std::vector<undirected_temporal_edge> edges;
  if (size_hint > 0)
    edges.reserve(size_hint);

  for (const auto& v : base_net.vertices()) {
    auto links = base_net.out_edges(v);
    if (links.empty())
      continue;

    for (double t = -max_t; t < max_t; t += iet_dist(gen))
      if (t >= 0)
        edges.emplace_back(
          links[std::uniform_int_distribution<std::size_t>(0, links.size() - 1)(
            gen)],
          t);
  }

  return undirected_temporal_network{edges, base_net.vertices()};
}

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_node_activation_temporal_network(
  directed_network base_net, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> directed_temporal_network {
  std::vector<directed_temporal_edge> edges;
  if (size_hint > 0)
    edges.reserve(size_hint);

  for (const auto& v : base_net.vertices()) {
    auto links = base_net.out_edges(v);
    if (links.empty())
      continue;

    for (double t = -max_t; t < max_t; t += iet_dist(gen))
      if (t >= 0)
        edges.emplace_back(
          links[std::uniform_int_distribution<std::size_t>(0, links.size() - 1)(
            gen)],
          t);
  }

  return directed_temporal_network{edges, base_net.vertices()};
}

} // namespace reticula
