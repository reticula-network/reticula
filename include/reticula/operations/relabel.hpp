#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT>
[[nodiscard]]
auto relabel(const NetT& g) -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, mapping<VertexType, VertexType> MapT>
[[nodiscard]]
auto relabel(const NetT& g, MapT&& mapping)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, std::invocable<VertexType> MapF>
  requires std::convertible_to<
    std::invoke_result_t<MapF, VertexType>, VertexType>
[[nodiscard]]
auto relabel(const NetT& g, const MapF& mapping)
  -> reticula::network<typename NetT::EdgeType>;
} // namespace reticula

namespace reticula {
template <network_like NetT>
auto relabel(const NetT& g) -> reticula::network<typename NetT::EdgeType> {
  std::unordered_map<VertexType, VertexType> new_labels;
  new_labels.reserve(g.vertices().size());

  for (VertexType i{}; const auto& v : g.vertices())
    new_labels.emplace(v, i++);

  return relabel(g, new_labels);
}

template <network_like NetT, std::invocable<VertexType> MapF>
  requires std::convertible_to<
    std::invoke_result_t<MapF, VertexType>, VertexType>
auto relabel(const NetT& g, const MapF& mapping)
  -> reticula::network<typename NetT::EdgeType> {
  return network<typename NetT::EdgeType>{
    g.edges() | std::views::transform([&mapping](const auto& e) {
      return edge_relabel<typename NetT::EdgeType>{}(e, mapping);
    }),
    g.vertices() | std::views::transform(mapping)};
}

template <network_like NetT, mapping<VertexType, VertexType> MapT>
auto relabel(const NetT& g, const MapT& mapping)
  -> reticula::network<typename NetT::EdgeType> {
  return relabel(g, [&](const VertexType& v) {
    auto it = mapping.find(v);
    if (it != mapping.end())
      return it->second;
    return v;
  });
}
} // namespace reticula
