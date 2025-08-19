#pragma once

#include <cstddef>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <reticula/components.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

#include "reticula/algorithms/detail/scc.hpp"

namespace reticula::detail {
enum struct component_direction { out, in };
template <
  component_direction Dir, component_like Comp,
  std::constructible_from<Comp> Res, network_like NetT,
  std::ranges::bidirectional_range TopoRange>
[[nodiscard]] auto
generic_reachability_components_dag(const NetT& net, TopoRange&& topo)
  -> std::vector<std::pair<VertexType, Res>> {
  std::unordered_map<VertexType, Comp> ongoing_components;
  std::unordered_map<VertexType, std::size_t> in_counts;

  std::vector<std::pair<VertexType, Res>> res;
  res.reserve(net.vertices().size());

  auto in_edges = [&](VertexType vert) {
    if constexpr (Dir == component_direction::in)
      return net.out_edges(vert);
    else
      return net.in_edges(vert);
  };
  auto out_edges = [&](VertexType vert) {
    if constexpr (Dir == component_direction::in)
      return net.in_edges(vert);
    else
      return net.out_edges(vert);
  };

  auto mutator_verts = [&](const NetT::EdgeType& e) {
    if constexpr (Dir == component_direction::in)
      return e.mutated_verts();
    else
      return e.mutator_verts();
  };
  auto mutated_verts = [&](const NetT::EdgeType& e) {
    if constexpr (Dir == component_direction::in)
      return e.mutator_verts();
    else
      return e.mutated_verts();
  };

  auto order = [&] {
    if constexpr (Dir == component_direction::out)
      return topo | std::views::common | std::views::reverse;
    else
      return topo | std::views::all;
  }();

  for (auto vert : order) {
    Comp comp({vert}, 0uz, 0uz);
    ongoing_components.emplace(vert, std::move(comp));

    in_counts.emplace(vert, 0);
    for (auto& e : in_edges(vert))
      in_counts.at(vert) += mutator_verts(e).size();

    for (auto& oe : out_edges(vert)) {
      for (auto& other : mutated_verts(oe)) {
        if constexpr (reticula::exact_component_like<Comp>) {
          if (!ongoing_components.at(vert).contains(other))
            ongoing_components.at(vert).merge(ongoing_components.at(other));
        } else {
          ongoing_components.at(vert).merge(ongoing_components.at(other));
        }
        in_counts.at(other)--;
        if (in_counts.at(other) == 0) {
          res.emplace_back(other, std::move(ongoing_components.at(other)));
          in_counts.erase(other);
          ongoing_components.erase(other);
        }
      }
    }

    if (in_counts.at(vert) == 0) {
      res.emplace_back(vert, ongoing_components.at(vert));
      in_counts.erase(vert);
      ongoing_components.erase(vert);
    }
  }

  return res;
}

template <
  component_direction Dir, component_like Comp,
  std::constructible_from<Comp> Res, network_like NetT>
[[nodiscard]] auto generic_reachability_components(const NetT& net)
  -> std::vector<std::pair<VertexType, Res>> {
  if (auto maybe_topo = try_topological_order(net); maybe_topo)
    return generic_reachability_components_dag<Dir, Comp, Res>(
      net, *maybe_topo);

  auto sccs = scc_tarjan(net, true);
  std::unordered_map<VertexType, std::size_t> scc_idx;
  for (std::size_t i = 0; auto& scc : sccs) {
    for (auto v : scc)
      scc_idx[v] = i;
    ++i;
  }

  auto condensed_edges =
    net.edges() |
    std::views::transform([&](const auto& e) -> directed_hyperedge {
      auto mutator_sccs =
        e.mutator_verts() |
        std::views::transform([&](VertexType v) { return scc_idx.at(v); });
      auto mutated_sccs =
        e.mutated_verts() |
        std::views::transform([&](VertexType v) { return scc_idx.at(v); });
      return {mutator_sccs, mutated_sccs};
    }) |
    std::views::filter([](const directed_hyperedge& e) {
      return !std::ranges::equal(e.mutator_verts(), e.mutated_verts());
    });
  auto condensed_verts = std::ranges::iota_view{VertexType{}, sccs.size()};

  directed_hypernetwork condensed_net(condensed_edges, condensed_verts);
  auto ocs = generic_reachability_components_dag<Dir, component, component>(
    condensed_net, condensed_verts | std::views::reverse);

  // TODO: maybe don't repeat components?
  std::vector<std::pair<VertexType, Res>> res;
  res.reserve(net.vertices().size());
  for (auto& [scc_id, oc] : ocs) {
    Comp expanded{};
    for (auto sid : oc)
      expanded.merge(sccs[sid]);
    for (auto v : sccs[scc_id])
      res.emplace_back(v, expanded);
  }
  return res;
}
} // namespace reticula::detail
