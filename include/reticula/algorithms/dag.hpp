#pragma once

#include <optional>
#include <vector>

#include <reticula/concepts.hpp>

namespace reticula {
/**
  Tries to sorts the vertices of `dir` in a way that for every directed edge in
  that graph, the head node appears after the tail node in the result.

  Returns std::nullopt in if the graph has cycles.

  @param dir Directed network in question
*/
template <static_network_like NetT>
  requires is_directed_v<NetT>
[[nodiscard]] auto try_topological_order(const NetT& net)
  -> std::optional<std::vector<VertexType>>;

/**
  Sorts the vertices of `net` in a way that for every directed edge in that
  graph, the head node appears after the tail node in the result.

  Throws not_acyclic_error if the graph has cycles.

  @param dir Directed network in question
*/
template <static_network_like NetT>
  requires is_directed_v<NetT>
[[nodiscard]] auto topological_order(const NetT& net)
  -> std::vector<VertexType>;

/**
  Returns true if the directed graph contains no cycles. This is detemined by
  trying to find a topologial ordering or the edges. If you intend to get a
  topological ordering of the nodes anyway, use
  `reticula::try_topological_order`.

  @param net Directed network in question
*/
template <static_network_like NetT>
  requires is_directed_v<NetT>
[[nodiscard]] auto is_acyclic(const NetT& net) -> bool;
} // namespace reticula

#include <stack>
#include <unordered_map>

#include <reticula/utils.hpp>

namespace reticula {
template <static_network_like NetT>
  requires is_directed_v<NetT>
[[nodiscard]] auto try_topological_order(const NetT& net)
  -> std::optional<std::vector<VertexType>> {
  auto verts = net.vertices();

  std::vector<VertexType> topo;
  topo.reserve(verts.size());

  std::stack<VertexType, std::vector<VertexType>> stack;

  // in order for this to work with hypergraphs, we need to count sum of the
  // number of mutator_verts of all in_edges, not in-degree, i.e. the number
  // of in-edges or even the number predecessor:
  std::unordered_map<VertexType, std::size_t> in_counts;
  in_counts.reserve(verts.size());

  for (auto& e : net.edges())
    for (auto v : e.mutated_verts())
      in_counts[v] += e.mutator_verts().size();

  for (auto& v : verts)
    if (!in_counts.contains(v))
      stack.push(v);

  while (!stack.empty()) {
    auto v = stack.top();
    stack.pop();
    topo.push_back(v);
    for (const auto& e : net.out_edges(v))
      for (const auto& ov : e.mutated_verts())
        if (--in_counts[ov] == 0)
          stack.push(ov);
  }

  if (topo.size() < verts.size())
    return std::nullopt;

  return topo;
}

template <static_network_like NetT>
  requires is_directed_v<NetT>
[[nodiscard]] auto topological_order(const NetT& net)
  -> std::vector<VertexType> {
  auto maybe_topo = try_topological_order(net);
  if (maybe_topo)
    return *maybe_topo;
  else
    throw not_acyclic_error("The network is not acyclic.");
}

template <static_network_like NetT>
  requires is_directed_v<NetT>
[[nodiscard]] auto is_acyclic(const NetT& net) -> bool {
  return try_topological_order(net).has_value();
}
} // namespace reticula
