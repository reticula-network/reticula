#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_link_activation_temporal_network(
  undirected_network base_network, double max_t, IETDist iet_dist,
  ResDist res_dist, Gen& gen, std::size_t size_hint = 0)
  -> undirected_temporal_network;

template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_link_activation_temporal_network(
  directed_network base_network, double max_t, IETDist iet_dist,
  ResDist res_dist, Gen& gen, std::size_t size_hint)
  -> directed_temporal_network;

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_link_activation_temporal_network(
  undirected_network base_network, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint = 0) -> undirected_temporal_network;

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_link_activation_temporal_network(
  directed_network base_network, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> directed_temporal_network;
} // namespace reticula

#include <vector>

namespace reticula {
template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_link_activation_temporal_network(
  undirected_network base_net, double max_t, IETDist iet_dist, ResDist res_dist,
  Gen& gen, std::size_t size_hint) -> undirected_temporal_network {
  std::vector<undirected_temporal_edge> edges;
  edges.reserve(size_hint);

  for (const auto& e : base_net.edges())
    for (auto t = res_dist(gen); t < max_t; t += iet_dist(gen))
      edges.emplace_back(e, t);

  return undirected_temporal_network(edges, base_net.vertices());
}

template <
  random_number_distribution<double> ResDist,
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_link_activation_temporal_network(
  directed_network base_net, double max_t, IETDist iet_dist, ResDist res_dist,
  Gen& gen, std::size_t size_hint) -> directed_temporal_network {
  std::vector<directed_temporal_edge> edges;
  edges.reserve(size_hint);

  for (const auto& e : base_net.edges())
    for (auto t = res_dist(gen); t < max_t; t += iet_dist(gen))
      edges.emplace_back(e, t);

  return directed_temporal_network(edges, base_net.vertices());
}

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_link_activation_temporal_network(
  undirected_network base_net, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> undirected_temporal_network {
  std::vector<undirected_temporal_edge> edges;
  edges.reserve(size_hint);

  for (const auto& e : base_net.edges())
    for (double t = -max_t; t < max_t; t += iet_dist(gen))
      if (t >= 0)
        edges.emplace_back(e, t);

  return undirected_temporal_network(edges, base_net.vertices());
}

template <
  random_number_distribution<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_link_activation_temporal_network(
  directed_network base_net, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> directed_temporal_network {
  std::vector<directed_temporal_edge> edges;
  edges.reserve(size_hint);

  for (const auto& e : base_net.edges())
    for (double t = -max_t; t < max_t; t += iet_dist(gen))
      if (t >= 0)
        edges.emplace_back(e, t);

  return directed_temporal_network(edges, base_net.vertices());
}
} // namespace reticula
