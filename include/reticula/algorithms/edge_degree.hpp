#pragma once

#include <vector>

#include <reticula/concepts.hpp>

namespace reticula {
template <network_edge EdgeT>
auto edge_in_degree(const EdgeT& edge) -> VertexType;

template <network_edge EdgeT>
auto edge_out_degree(const EdgeT& edge) -> VertexType;

template <network_edge EdgeT>
auto edge_incident_degree(const EdgeT& edge) -> VertexType;

template <network_edge EdgeT>
  requires is_undirected_v<EdgeT>
auto edge_degree(const EdgeT& edge) -> VertexType;

template <network_like NetT>
auto edge_in_degree_sequence(const NetT& net) -> std::vector<VertexType>;

template <network_like NetT>
auto edge_out_degree_sequence(const NetT& net) -> std::vector<VertexType>;

template <network_like NetT>
auto edge_in_out_degree_pair_sequence(const NetT& net)
  -> std::vector<std::pair<VertexType, VertexType>>;

template <network_like NetT>
auto edge_incident_degree_sequence(const NetT& net) -> std::vector<VertexType>;

template <network_like NetT>
  requires is_undirected_v<NetT>
auto edge_degree_sequence(const NetT& net) -> std::vector<VertexType>;
} // namespace reticula

namespace reticula {
template <network_edge EdgeT>
auto edge_in_degree(const EdgeT& edge) -> VertexType {
  return static_cast<VertexType>(edge.mutator_verts().size());
}

template <network_edge EdgeT>
auto edge_out_degree(const EdgeT& edge) -> VertexType {
  return static_cast<VertexType>(edge.mutated_verts().size());
}

template <network_edge EdgeT>
auto edge_incident_degree(const EdgeT& edge) -> VertexType {
  return static_cast<VertexType>(edge.incident_verts().size());
}

template <network_edge EdgeT>
  requires is_undirected_v<EdgeT>
auto edge_degree(const EdgeT& edge) -> VertexType {
  return edge_incident_degree(edge);
}

template <network_like NetT>
auto edge_in_degree_sequence(const NetT& net) -> std::vector<VertexType> {
  std::vector<VertexType> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_in_degree(edge));

  return degrees;
}

template <network_like NetT>
auto edge_out_degree_sequence(const NetT& net) -> std::vector<VertexType> {
  std::vector<VertexType> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_out_degree(edge));

  return degrees;
}

template <network_like NetT>
auto edge_in_out_degree_pair_sequence(const NetT& net)
  -> std::vector<std::pair<VertexType, VertexType>> {
  std::vector<std::pair<VertexType, VertexType>> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.emplace_back(edge_in_degree(edge), edge_out_degree(edge));

  return degrees;
}

template <network_like NetT>
auto edge_incident_degree_sequence(const NetT& net) -> std::vector<VertexType> {
  std::vector<VertexType> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_incident_degree(edge));

  return degrees;
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto edge_degree_sequence(const NetT& net) -> std::vector<VertexType> {
  std::vector<VertexType> degrees;
  degrees.reserve(net.edges().size());

  for (const auto& edge : net.edges())
    degrees.push_back(edge_degree(edge));

  return degrees;
}
} // namespace reticula
