#pragma once

#include <cstddef>
#include <ranges>
#include <vector>

#include <reticula/components.hpp>
#include <reticula/concepts.hpp>

#include "reticula/algorithms/detail/bfs.hpp"
#include "reticula/algorithms/detail/reachability_components.hpp"
#include "reticula/algorithms/detail/scc.hpp"

namespace reticula {

// Weak connectivity

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto weakly_connected_components(const NetT& net, bool singletons = true)
  -> std::vector<component>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto largest_weakly_connected_component(const NetT& net) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto weakly_connected_component(const NetT& net, VertexType v) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_weakly_connected(const NetT& net) -> bool;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_weakly_reachable(const NetT& net, VertexType v1, VertexType v2) -> bool;

// In- and out-components

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_components(const NetT& net)
  -> std::vector<std::pair<VertexType, component>>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_components(const NetT& net)
  -> std::vector<std::pair<VertexType, component>>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_component(const NetT& net, VertexType v) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_component(const NetT& net, VertexType v) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_component_sizes(const NetT& net)
  -> std::vector<std::pair<VertexType, std::size_t>>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_component_sizes(const NetT& net)
  -> std::vector<std::pair<VertexType, std::size_t>>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_component_size_estimates(const NetT& net)
  -> std::vector<std::pair<VertexType, double>>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_component_size_estimates(const NetT& net)
  -> std::vector<std::pair<VertexType, double>>;

// Strong connectivity

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto strongly_connected_components(const NetT& net, bool singletons = true)
  -> std::vector<component>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto largest_strongly_connected_component(const NetT& net) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto strongly_connected_component(const NetT& net, VertexType v) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_strongly_connected(const NetT& net) -> bool;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_strongly_reachable(const NetT& net, VertexType v1, VertexType v2)
  -> bool;

// Undirected connectivity

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto connected_components(const NetT& net, bool singletons = true)
  -> std::vector<component>;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto largest_connected_component(const NetT& net) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto connected_component(const NetT& net, VertexType v) -> component;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto is_connected(const NetT& net) -> bool;

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_reachable(const NetT& net, VertexType source, VertexType destination)
  -> bool;
} // namespace reticula

#include <algorithm>
#include <numeric> // IWYU pragma: keep  // bug in disjoint_set.hpp

#include <ds/disjoint_set.hpp>

#include <reticula/algorithms/dag.hpp>
#include <reticula/components.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::detail {
template <network_like NetT>
auto generic_weakly_connected_components(const NetT& net, bool singletons)
  -> std::vector<component> {
  auto disj_set = ds::disjoint_set<std::size_t>(net.vertices().size());

  std::unordered_map<VertexType, std::size_t> vert_idx;
  if constexpr (!dense_id_network<NetT>) {
    vert_idx.reserve(net.vertices().size());
    for (std::size_t i = 0; auto v : net.vertices())
      vert_idx.emplace(v, i++);
  }

  auto vert_dense_id = [&](VertexType v) -> std::size_t {
    if constexpr (dense_id_network<NetT>)
      return net.vertex_id(v);
    else
      return vert_idx.at(v);
  };

  auto verts = net.vertices();
  auto id_vert = [&](std::size_t idx) -> VertexType {
    if constexpr (dense_id_network<NetT>)
      return net.id_vertex(idx);
    else
      return verts[idx];
  };

  for (auto e : net.edges()) {
    auto mutator_verts = e.mutator_verts();
    auto mutated_verts = e.mutated_verts();
    for (auto v1 : mutator_verts)
      for (auto v2 : mutated_verts)
        if (!is_undirected_v<NetT> || v1 < v2)
          disj_set.merge(vert_dense_id(v1), vert_dense_id(v2));
  }

  auto sets = disj_set.sets(singletons);
  std::vector<component> comp_vector;
  comp_vector.reserve(sets.size());

  for (const auto& [idx, set] : sets) {
    auto& current_set = comp_vector.emplace_back(set.size());
    for (const auto& vert_idx : set)
      current_set.insert(id_vert(vert_idx));
  }

  return comp_vector;
}

struct component_size {
  std::size_t size;

  component_size(const component& c) : size{c.size()} {}
};

struct component_size_estimate {
  double size_estimate;

  component_size_estimate(const component_sketch& c)
      : size_estimate{c.size_estimate()} {};
};
} // namespace reticula::detail

namespace reticula {

// Weak reachability

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto weakly_connected_components(const NetT& net, bool singletons)
  -> std::vector<component> {
  return detail::generic_weakly_connected_components(net, singletons);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto largest_weakly_connected_component(const NetT& net) -> component {
  auto components = detail::generic_weakly_connected_components(net, true);
  if (components.empty())
    return component{};
  return *std::ranges::max_element(
    components,
    [](const component& a, const component& b) { return a.size() < b.size(); });
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto weakly_connected_component(const NetT& net, VertexType v) -> component {
  return detail::breadth_first_search<
    detail::breadth_first_search_direction::undirected>(
    net, v, [](VertexType, const NetT::EdgeType&, VertexType) { return true; },
    0);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_weakly_connected(const NetT& net) -> bool {
  if (net.vertices().empty())
    return true;

  auto comp = detail::breadth_first_search<
    detail::breadth_first_search_direction::undirected>(
    net, net.vertices().front(),
    [](VertexType, const NetT::EdgeType&, VertexType) { return true; }, 0);
  return comp.size() == net.vertices().size();
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_weakly_reachable(const NetT& net, VertexType v1, VertexType v2)
  -> bool {
  if (v1 == v2)
    return true;

  auto comp = detail::breadth_first_search<
    detail::breadth_first_search_direction::undirected>(
    net, v1,
    [v2](VertexType, const NetT::EdgeType&, VertexType v) { return v != v2; },
    0);
  return comp.contains(v2);
}

// in- and out-components

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_components(const NetT& net)
  -> std::vector<std::pair<VertexType, component>> {
  return detail::generic_reachability_components<
    detail::component_direction::in, component, component>(net);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_components(const NetT& net)
  -> std::vector<std::pair<VertexType, component>> {
  return detail::generic_reachability_components<
    detail::component_direction::out, component, component>(net);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_component_sizes(const NetT& net)
  -> std::vector<std::pair<VertexType, std::size_t>> {
  auto ic =
    detail::generic_reachability_components<
      detail::component_direction::in, component, detail::component_size>(net) |
    std::views::transform(
      [](const auto& p) -> std::pair<VertexType, std::size_t> {
        return {p.first, p.second.size};
      });

  return {ic.begin(), ic.end()};
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_component_sizes(const NetT& net)
  -> std::vector<std::pair<VertexType, std::size_t>> {
  auto oc =
    detail::generic_reachability_components<
      detail::component_direction::out, component, detail::component_size>(
      net) |
    std::views::transform(
      [](const auto& p) -> std::pair<VertexType, std::size_t> {
        return {p.first, p.second.size};
      });

  return {oc.begin(), oc.end()};
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_component_size_estimates(const NetT& net)
  -> std::vector<std::pair<VertexType, double>> {
  auto ic =
    detail::generic_reachability_components<
      detail::component_direction::in, component_sketch,
      detail::component_size_estimate>(net) |
    std::views::transform([](const auto& p) -> std::pair<VertexType, double> {
      return {p.first, p.second.size_estimate};
    });

  return {ic.begin(), ic.end()};
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_component_size_estimates(const NetT& net)
  -> std::vector<std::pair<VertexType, double>> {
  auto oc =
    detail::generic_reachability_components<
      detail::component_direction::out, component_sketch,
      detail::component_size_estimate>(net) |
    std::views::transform([](const auto& p) -> std::pair<VertexType, double> {
      return {p.first, p.second.size_estimate};
    });

  return {oc.begin(), oc.end()};
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto in_component(const NetT& net, VertexType v) -> component {
  return detail::breadth_first_search<
    detail::breadth_first_search_direction::backward>(
    net, v, [](VertexType, const NetT::EdgeType&, VertexType) { return true; },
    0);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto out_component(const NetT& net, VertexType v) -> component {
  return detail::breadth_first_search<
    detail::breadth_first_search_direction::forward>(
    net, v, [](VertexType, const NetT::EdgeType&, VertexType) { return true; },
    0);
}

// Strong reachability

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto strongly_connected_components(const NetT& net, bool singletons)
  -> std::vector<component> {
  return detail::scc_tarjan(net, singletons);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto largest_strongly_connected_component(const NetT& net) -> component {
  auto components = detail::scc_tarjan(net, true);
  if (components.empty())
    return component{};
  return *std::ranges::max_element(
    components,
    [](const component& a, const component& b) { return a.size() < b.size(); });
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto strongly_connected_component(const NetT& net, VertexType v) -> component {
  auto oc = detail::breadth_first_search<
    detail::breadth_first_search_direction::forward>(
    net, v, [](VertexType, const NetT::EdgeType&, VertexType) { return true; },
    0);
  auto ic = detail::breadth_first_search<
    detail::breadth_first_search_direction::backward>(
    net, v, [](VertexType, const NetT::EdgeType&, VertexType) { return true; },
    0);
  component scc(std::min(oc.size(), ic.size()));
  for (const auto& u : oc)
    if (ic.contains(u))
      scc.insert(u);
  return scc;
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_strongly_connected(const NetT& net) -> bool {
  if (net.vertices().empty())
    return true;

  auto components = detail::scc_tarjan(net, true);
  return components.size() == 1 &&
         components[0].size() == net.vertices().size();
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_strongly_reachable(const NetT& net, VertexType v1, VertexType v2)
  -> bool {
  return is_reachable(net, v1, v2) && is_reachable(net, v2, v1);
}

// Undirected reachability

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto connected_components(const NetT& net, bool singletons)
  -> std::vector<component> {
  return detail::generic_weakly_connected_components(net, singletons);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto largest_connected_component(const NetT& net) -> component {
  auto components = detail::generic_weakly_connected_components(net, true);
  if (components.empty())
    return component{};
  return *std::ranges::max_element(
    components,
    [](const component& a, const component& b) { return a.size() < b.size(); });
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto connected_component(const NetT& net, VertexType v) -> component {
  return detail::breadth_first_search<
    detail::breadth_first_search_direction::undirected>(
    net, v, [](VertexType, const NetT::EdgeType&, VertexType) { return true; },
    0);
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType> && is_undirected_v<NetT>
auto is_connected(const NetT& net) -> bool {
  if (net.vertices().empty())
    return true;

  auto comp = detail::breadth_first_search<
    detail::breadth_first_search_direction::undirected>(
    net, net.vertices().front(),
    [](VertexType, const NetT::EdgeType&, VertexType) { return true; }, 0);
  return comp.size() == net.vertices().size();
}

template <network_like NetT>
  requires static_network_edge<typename NetT::EdgeType>
auto is_reachable(const NetT& net, VertexType source, VertexType destination)
  -> bool {
  if (source == destination)
    return true;

  auto comp = detail::breadth_first_search<
    detail::breadth_first_search_direction::forward>(
    net, source,
    [destination](VertexType, const NetT::EdgeType&, VertexType v) {
      return v != destination;
    },
    0);
  return comp.contains(destination);
}
} // namespace reticula
