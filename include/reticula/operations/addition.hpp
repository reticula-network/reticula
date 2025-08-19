#pragma once

#include <initializer_list>

#include <ranges>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
[[nodiscard]]
auto with_vertices(const NetT& g, Range&& vertices)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT>
[[nodiscard]]
auto with_vertices(
  const NetT& g, const std::initializer_list<VertexType> vertices)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<
    std::ranges::range_value_t<Range>, typename NetT::EdgeType>
[[nodiscard]]
auto with_edges(const NetT& g, Range&& edges)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT>
[[nodiscard]]
auto with_edges(
  const NetT& g, const std::initializer_list<typename NetT::EdgeType> edges)
  -> reticula::network<typename NetT::EdgeType>;
} // namespace reticula

#include <reticula/operations/algebraic.hpp>

namespace reticula {
template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
auto with_vertices(const NetT& g, Range&& vertices)
  -> reticula::network<typename NetT::EdgeType> {
  return graph_union(
    g, network<typename NetT::EdgeType>(
         std::views::empty<typename NetT::EdgeType>,
         std::forward<Range>(vertices)));
}

template <network_like NetT>
auto with_vertices(
  const NetT& g, const std::initializer_list<VertexType> vertices)
  -> reticula::network<typename NetT::EdgeType> {
  return with_vertices(g, std::views::all(vertices));
}

template <network_like NetT, std::ranges::input_range Range>
  requires std::convertible_to<
    std::ranges::range_value_t<Range>, typename NetT::EdgeType>
auto with_edges(const NetT& g, Range&& edges)
  -> reticula::network<typename NetT::EdgeType> {
  return graph_union(
    g, network<typename NetT::EdgeType>(
         std::forward<Range>(edges), std::views::empty<VertexType>));
}

template <network_like NetT>
auto with_edges(
  const NetT& g, const std::initializer_list<typename NetT::EdgeType> edges)
  -> reticula::network<typename NetT::EdgeType> {
  return with_edges(g, std::views::all(edges));
}
} // namespace reticula
