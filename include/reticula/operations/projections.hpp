#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
[[nodiscard]]
auto static_projection(const NetT& net)
  -> network<typename NetT::EdgeType::StaticProjectionType>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
[[nodiscard]]
auto directed_projection(const NetT& net) -> directed_network;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
[[nodiscard]]
auto clique_expansion(const NetT& net) -> undirected_network;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_directed_v<NetT>
[[nodiscard]]
auto clique_expansion(const NetT& net) -> directed_network;
} // namespace reticula

#include <ranges>

#include <reticula/views/cartesian_product.hpp>

namespace reticula {

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
auto static_projection(const NetT& net)
  -> network<typename NetT::EdgeType::StaticProjectionType> {
  return network<typename NetT::EdgeType::StaticProjectionType>(
    net.edges_cause() | std::views::transform(
                          [](const auto& e) { return e.static_projection(); }),
    net.vertices());
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto directed_projection(const NetT& net) -> directed_network {
  auto edges =
    net.edges() | std::views::transform([](const auto& e) {
      return views::cartesian_product(e.mutator_verts(), e.mutated_verts()) |
             std::views::filter([](const auto& pair) {
               auto [i, j] = pair;
               return i != j;
             }) |
             std::views::transform([](const auto& pair) {
               auto [i, j] = pair;
               return directed_edge{i, j};
             });
    }) |
    std::views::join;
  return directed_network(edges, net.vertices());
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto clique_expansion(const NetT& net) -> undirected_network {
  auto edges =
    net.edges() | std::views::transform([](const auto& e) {
      return views::cartesian_product(e.mutator_verts(), e.mutated_verts()) |
             std::views::filter([](const auto& pair) {
               auto [i, j] = pair;
               return i < j;
             }) |
             std::views::transform([](const auto& pair) {
               auto [i, j] = pair;
               return undirected_edge{i, j};
             });
    }) |
    std::views::join;
  return undirected_network(edges, net.vertices());
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_directed_v<NetT>
auto clique_expansion(const NetT& net) -> directed_network {
  return directed_projection(net);
}
} // namespace reticula
