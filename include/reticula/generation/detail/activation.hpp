#pragma once

#include <reticula/concepts.hpp>
#include <reticula/edges.hpp>

namespace reticula::detail {
template <static_network_edge EdgeT>
struct temporal_activation;

template <>
struct temporal_activation<undirected_edge> {
  using type = undirected_temporal_edge;
};

template <>
struct temporal_activation<directed_edge> {
  using type = directed_temporal_edge;
};

template <>
struct temporal_activation<undirected_hyperedge> {
  using type = undirected_temporal_hyperedge;
};

template <>
struct temporal_activation<directed_hyperedge> {
  using type = directed_temporal_hyperedge;
};

template <static_network_edge EdgeT>
using temporal_activation_t = typename temporal_activation<EdgeT>::type;
} // namespace reticula::detail

