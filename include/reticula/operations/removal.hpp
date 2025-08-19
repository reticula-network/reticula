#pragma once

#include <initializer_list>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT>
[[nodiscard]]
auto without_isolated_vertices(const NetT& g)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
[[nodiscard]]
auto without_vertices(const NetT& g, Range&& vertices)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT>
[[nodiscard]]
auto without_vertices(
  const NetT& g, const std::initializer_list<VertexType> vertices)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<
    std::ranges::range_value_t<Range>, typename NetT::EdgeType>
[[nodiscard]]
auto without_edges(const NetT& g, Range&& edges)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT>
[[nodiscard]]
auto without_edges(
  const NetT& g, const std::initializer_list<typename NetT::EdgeType> edges)
  -> reticula::network<typename NetT::EdgeType>;
} // namespace reticula

namespace reticula {
template <network_like NetT>
[[nodiscard]]
auto without_isolated_vertices(const NetT& g)
  -> reticula::network<typename NetT::EdgeType> {
  return without_vertices(
    g, g.vertices() | std::views::filter([&g](const auto& v) {
         return std::ranges::empty(g.out_edges(v)) &&
                std::ranges::empty(g.in_edges(v));
       }));
}

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
auto without_vertices(const NetT& g, Range&& vertices)
  -> reticula::network<typename NetT::EdgeType> {
  std::unordered_set<VertexType> vertex_set(
    std::ranges::begin(vertices), std::ranges::end(vertices));
  return reticula::network<typename NetT::EdgeType>{
    g.edges() | std::views::filter([&vertex_set](const auto& e) {
      for (auto v : e.mutator_verts())
        if (vertex_set.contains(v))
          return false;
      for (auto v : e.mutated_verts())
        if (vertex_set.contains(v))
          return false;
      return true;
    }),
    g.vertices() | std::views::filter([&vertex_set](const auto& v) {
      return !vertex_set.contains(v);
    })};
}

template <network_like NetT>
auto without_vertices(
  const NetT& g, const std::initializer_list<VertexType> vertices)
  -> reticula::network<typename NetT::EdgeType> {
  return without_vertices(g, std::views::all(vertices));
}

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<
    std::ranges::range_value_t<Range>, typename NetT::EdgeType>
auto without_edges(const NetT& g, Range&& edges)
  -> reticula::network<typename NetT::EdgeType> {
  std::unordered_set<typename NetT::EdgeType> edge_set(
    std::ranges::begin(edges), std::ranges::end(edges));
  return reticula::network<typename NetT::EdgeType>{
    g.edges() | std::views::filter(
                  [&edge_set](const auto& e) { return !edge_set.contains(e); }),
    g.vertices()};
}

template <network_like NetT>
auto without_edges(
  const NetT& g, const std::initializer_list<typename NetT::EdgeType> edges)
  -> reticula::network<typename NetT::EdgeType> {
  return without_edges(g, std::views::all(edges));
}
} // namespace reticula
