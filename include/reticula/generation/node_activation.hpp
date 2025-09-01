#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

#include "reticula/generation/detail/activation.hpp"

namespace reticula {
template <
  static_network_edge EdgeT, point_process<double> IETDist,
  point_process<double> ResDist, std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_node_activation_temporal_network(
  const network<EdgeT>& base_network, double max_t, IETDist iet_dist,
  ResDist res_dist, Gen& gen, std::size_t size_hint = 0)
  -> network<detail::temporal_activation_t<EdgeT>>;

template <
  static_network_edge EdgeT, point_process<double> IETDist,
  std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_node_activation_temporal_network_with_burn_in(
  const network<EdgeT>& base_network, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint = 0) -> network<detail::temporal_activation_t<EdgeT>>;
} // namespace reticula

#include <vector>

namespace reticula {

template <
  static_network_edge EdgeT, point_process<double> IETDist,
  point_process<double> ResDist, std::uniform_random_bit_generator Gen>
auto random_node_activation_temporal_network(
  const network<EdgeT>& base_net, double max_t, IETDist iet_dist,
  ResDist res_dist, Gen& gen, std::size_t size_hint)
  -> network<detail::temporal_activation_t<EdgeT>> {
  std::vector<detail::temporal_activation_t<EdgeT>> edges;
  if (size_hint > 0)
    edges.reserve(size_hint);

  for (const auto& v : base_net.vertices()) {
    auto links = base_net.out_edges(v);
    if (links.empty())
      continue;

    std::uniform_int_distribution<std::size_t> idx(0, links.size() - 1);
    for (auto t = static_cast<double>(res_dist(gen)); t < max_t;
         t += static_cast<double>(iet_dist(gen)))
      edges.emplace_back(links[idx(gen)], t);
  }

  return network<detail::temporal_activation_t<EdgeT>>{
    edges, base_net.vertices()};
}

template <
  static_network_edge EdgeT, point_process<double> IETDist,
  std::uniform_random_bit_generator Gen>
auto random_node_activation_temporal_network_with_burn_in(
  const network<EdgeT>& base_net, double max_t, IETDist iet_dist, Gen& gen,
  std::size_t size_hint) -> network<detail::temporal_activation_t<EdgeT>> {
  std::vector<detail::temporal_activation_t<EdgeT>> edges;
  if (size_hint > 0)
    edges.reserve(size_hint);

  for (const auto& v : base_net.vertices()) {
    auto links = base_net.out_edges(v);
    if (links.empty())
      continue;

    for (double t = -max_t; t < max_t; t += static_cast<double>(iet_dist(gen)))
      if (t >= 0)
        edges.emplace_back(
          links[std::uniform_int_distribution<std::size_t>(0, links.size() - 1)(
            gen)],
          t);
  }

  return network<detail::temporal_activation_t<EdgeT>>{
    edges, base_net.vertices()};
}
} // namespace reticula
