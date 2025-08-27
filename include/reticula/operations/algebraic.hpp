#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT1, network_like NetT2>
[[nodiscard]]
auto cartesian_product(const NetT1& g1, const NetT2& g2);

template <network_like NetT1, network_like NetT2>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
[[nodiscard]]
auto disjoint_union(const NetT1& g1, const NetT2& g2)
  -> reticula::network<typename NetT1::EdgeType>;

template <network_like NetT1, network_like NetT2>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
[[nodiscard]]
auto graph_union(const NetT1& g1, const NetT2& g2)
  -> reticula::network<typename NetT1::EdgeType>;

template <network_like NetT1, network_like NetT2>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
[[nodiscard]]
auto graph_intersection(const NetT1& g1, const NetT2& g2)
  -> reticula::network<typename NetT1::EdgeType>;

template <static_network_like NetT>
  requires is_dyadic_v<NetT>
[[nodiscard]]
auto complement_graph(const NetT& g)
  -> reticula::network<typename NetT::EdgeType>;
} // namespace reticula

#include <reticula/operations/relabel.hpp>
#include <reticula/views/cartesian_product.hpp>
#include <reticula/views/chain.hpp>

namespace reticula {
// template <network_like NetT1, network_like NetT2>
// auto cartesian_product(const NetT1& g1, const NetT2& g2) {
// }

template <network_like NetT1, network_like NetT2>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
auto disjoint_union(const NetT1& g1, const NetT2& g2)
  -> reticula::network<typename NetT1::EdgeType> {
  std::unordered_map<VertexType, VertexType> map1;
  map1.reserve(g1.vertices().size());
  VertexType i = 0;
  for (const auto& v : g1.vertices())
    map1[v] = i++;

  std::unordered_map<VertexType, VertexType> map2;
  map2.reserve(g2.vertices().size());
  for (const auto& v : g2.vertices())
    map2[v] = i++;

  return graph_union(relabel(g1, map1), relabel(g2, map2));
}

template <network_like NetT1, network_like NetT2>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
auto graph_union(const NetT1& g1, const NetT2& g2)
  -> reticula::network<typename NetT1::EdgeType> {
  return reticula::network<typename NetT1::EdgeType>(
    reticula::chain_view(g1.edges(), g2.edges()),
    reticula::chain_view(g1.vertices(), g2.vertices()));
}

template <network_like NetT1, network_like NetT2>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
auto graph_intersection(const NetT1& g1, const NetT2& g2)
  -> reticula::network<typename NetT1::EdgeType> {
  // TODO: Avoid allocations by using views.
  std::vector<VertexType> vertices;
  vertices.reserve(g1.vertices().size() + g2.vertices().size());
  std::ranges::set_intersection(
    g1.vertices(), g2.vertices(), std::back_inserter(vertices));

  std::vector<typename NetT1::EdgeType> edges;
  edges.reserve(g1.edges().size() + g2.edges().size());
  std::ranges::set_intersection(
    g1.edges(), g2.edges(), std::back_inserter(edges));

  return reticula::network<typename NetT1::EdgeType>(edges, vertices);
}

template <static_network_like NetT>
  requires is_dyadic_v<NetT>
auto complement_graph(const NetT& g)
  -> reticula::network<typename NetT::EdgeType> {
  if (g.vertices().size() < 2)
    return reticula::network<typename NetT::EdgeType>(
      std::views::empty<typename NetT::EdgeType>, g.vertices());

  // TODO: Avoid allocations by using views.
  std::vector<typename NetT::EdgeType> edges;
  if constexpr (is_undirected_v<NetT>)
    edges.reserve(
      g.vertices().size() * (g.vertices().size() - 1) / 2 - g.edges().size());
  else
    edges.reserve(
      g.vertices().size() * (g.vertices().size() - 1) - g.edges().size());

  for (auto v1 : g.vertices()) {
    for (auto v2 : g.vertices()) {
      if constexpr (is_undirected_v<NetT>)
        if (v2 >= v1)
          break;

      if (v2 == v1)
        continue;

      typename NetT::EdgeType e{v1, v2};
      if (!g.has_edge(e))
        edges.emplace_back(e);
    }
  }

  return reticula::network<typename NetT::EdgeType>(edges, g.vertices());
}
} // namespace reticula
