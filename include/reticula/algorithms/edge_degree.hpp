#pragma once

#include <cstddef>
#include <vector>

#include <reticula/concepts.hpp>

namespace reticula {
template <network_edge EdgeT>
auto edge_in_degree(const EdgeT& edge) -> std::size_t;

template <network_edge EdgeT>
auto edge_out_degree(const EdgeT& edge) -> std::size_t;

template <network_edge EdgeT>
auto edge_incident_degree(const EdgeT& edge) -> std::size_t;

template <network_edge EdgeT>
  requires is_undirected_v<EdgeT>
auto edge_degree(const EdgeT& edge) -> std::size_t;

template <network_like NetT>
auto edge_in_degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
auto edge_out_degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
auto edge_in_out_degree_pair_sequence(const NetT& net)
  -> std::vector<std::pair<std::size_t, std::size_t>>;

template <network_like NetT>
auto edge_incident_degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
  requires is_undirected_v<NetT>
auto edge_degree_sequence(const NetT& net) -> std::vector<std::size_t>;
} // namespace reticula

namespace reticula {
template <network_edge EdgeT>
auto edge_in_degree(const EdgeT& edge) -> std::size_t {
  return edge.mutator_verts().size();
}

template <network_edge EdgeT>
auto edge_out_degree(const EdgeT& edge) -> std::size_t {
  return edge.mutated_verts().size();
}

template <network_edge EdgeT>
auto edge_incident_degree(const EdgeT& edge) -> std::size_t {
  return edge.incident_verts().size();
}

template <network_edge EdgeT>
  requires is_undirected_v<EdgeT>
auto edge_degree(const EdgeT& edge) -> std::size_t {
  return edge_incident_degree(edge);
}

template <network_like NetT>
auto edge_in_degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_in_degree(edge));

  return degrees;
}

template <network_like NetT>
auto edge_out_degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_out_degree(edge));

  return degrees;
}

template <network_like NetT>
auto edge_in_out_degree_pair_sequence(const NetT& net)
  -> std::vector<std::pair<std::size_t, std::size_t>> {
  std::vector<std::pair<std::size_t, std::size_t>> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.emplace_back(edge_in_degree(edge), edge_out_degree(edge));

  return degrees;
}

template <network_like NetT>
auto edge_incident_degree_sequence(const NetT& net)
  -> std::vector<std::size_t> {
  std::vector<std::size_t> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_incident_degree(edge));

  return degrees;
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto edge_degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_degree(edge));

  return degrees;
}
} // namespace reticula
