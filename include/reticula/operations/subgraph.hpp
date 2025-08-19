#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
[[nodiscard]]
auto vertex_induced_subgraph(const NetT& g, Range&& vertices)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT>
[[nodiscard]]
auto vertex_induced_subgraph(
  const NetT& g, const std::initializer_list<VertexType> vertices)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<
    std::ranges::range_value_t<Range>, typename NetT::EdgeType>
[[nodiscard]]
auto edge_induced_subgraph(const NetT& g, Range&& edges)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT>
[[nodiscard]]
auto edge_induced_subgraph(
  const NetT& g, const std::initializer_list<typename NetT::EdgeType> edges)
  -> reticula::network<typename NetT::EdgeType>;
} // namespace reticula

namespace reticula {
template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
auto vertex_induced_subgraph(const NetT& g, Range&& vertices)
  -> reticula::network<typename NetT::EdgeType> {
  std::unordered_set<VertexType> vertex_set(
    std::ranges::begin(vertices), std::ranges::end(vertices));
  return reticula::network<typename NetT::EdgeType>{
    g.edges() | std::views::filter([&vertex_set](const auto& e) {
      for (auto v : e.mutator_verts())
        if (!vertex_set.contains(v))
          return false;
      for (auto v : e.mutated_verts())
        if (!vertex_set.contains(v))
          return false;
      return true;
    }),
    g.vertices() | std::views::filter([&vertex_set](const auto& v) {
      return vertex_set.contains(v);
    })};
}

template <network_like NetT>
auto vertex_induced_subgraph(
  const NetT& g, const std::initializer_list<VertexType> vertices)
  -> reticula::network<typename NetT::EdgeType> {
  return vertex_induced_subgraph(g, std::views::all(vertices));
}

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<
    std::ranges::range_value_t<Range>, typename NetT::EdgeType>
auto edge_induced_subgraph(const NetT& g, Range&& edges)
  -> reticula::network<typename NetT::EdgeType> {
  std::unordered_set<typename NetT::EdgeType> edge_set(
    std::ranges::begin(edges), std::ranges::end(edges));
  std::unordered_set<VertexType> vertex_set;
  for (const auto& e : edge_set) {
    for (auto v : e.mutator_verts())
      vertex_set.insert(v);
    for (auto v : e.mutated_verts())
      vertex_set.insert(v);
  }

  return reticula::network<typename NetT::EdgeType>{
    g.edges() | std::views::filter(
                  [&edge_set](const auto& e) { return edge_set.contains(e); }),
    g.vertices() | std::views::filter([&vertex_set](const auto& v) {
      return vertex_set.contains(v);
    })};
}

template <network_like NetT>
auto edge_induced_subgraph(
  const NetT& g, const std::initializer_list<typename NetT::EdgeType> edges)
  -> reticula::network<typename NetT::EdgeType> {
  return edge_induced_subgraph(g, std::views::all(edges));
}
} // namespace reticula
