#pragma once

#include <ranges>
#include <span>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/temporal_adjacency.hpp>

namespace reticula::event_graph {
template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
[[nodiscard]] auto successors(
  const NetT& net, const typename NetT::EdgeType& e, AdjT& adj,
  bool reduce = false) -> std::vector<typename NetT::EdgeType>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
[[nodiscard]] auto predecessors(
  const NetT& net, const typename NetT::EdgeType& e, AdjT& adj,
  bool reduce = false) -> std::vector<typename NetT::EdgeType>;
} // namespace reticula::event_graph

namespace reticula::event_graph {
namespace detail {
template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
[[nodiscard]] auto vertex_predecessors(
  const NetT& net, const typename NetT::EdgeType& e, VertexType v, AdjT& adj,
  bool reduce = false) -> std::span<const typename NetT::EdgeType> {
  auto in_edges_orig = net.in_edges(v);
  auto in_edges = in_edges_orig | std::views::reverse;

  auto start = std::ranges::upper_bound(
    in_edges, e.cause_time(), std::ranges::greater{},
    [](const auto& edge) { return edge.effect_time(); });

  auto end = start;
  double cutoff = adj.maximum_linger(v);

  bool reducible = is_undirected_v<NetT> && reduce;

  while ((end < in_edges.end()) && adjacent(*end, e) &&
         e.cause_time() - end->effect_time() <= cutoff) {
    if (reducible && end != start && start->cause_time() != end->cause_time())
      return {end.base(), start.base()};
    end++;
  }
  return {end.base(), start.base()};
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
[[nodiscard]] auto vertex_successors(
  const NetT& net, const typename NetT::EdgeType& e, VertexType v, AdjT& adj,
  bool reduce = false) -> std::span<const typename NetT::EdgeType> {
  auto out_edges = net.out_edges(v);

  auto start = std::ranges::upper_bound(
    out_edges, e.effect_time(), std::ranges::less{},
    [](const auto& edge) { return edge.cause_time(); });

  auto end = start;
  double cutoff = adj.linger(e, v);

  bool reducible = is_undirected_v<NetT> && reduce;

  while ((end < out_edges.end()) && adjacent(e, *end) &&
         end->cause_time() - e.effect_time() <= cutoff) {
    if (reducible && end != start && start->cause_time() != end->cause_time())
      return {start, end};
    end++;
  }
  return {start, end};
}
} // namespace detail

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
[[nodiscard]] auto successors(
  const NetT& net, const typename NetT::EdgeType& e, AdjT& adj, bool reduce)
  -> std::vector<typename NetT::EdgeType> {
  std::vector<typename NetT::EdgeType> succ;
  succ.reserve(e.mutated_verts().size());

  for (const auto& v : e.mutated_verts()) {
    std::size_t middle_offset = succ.size();
    auto out_edges = detail::vertex_successors(net, e, v, adj, reduce);
    succ.reserve(succ.size() + out_edges.size());
    succ.insert(succ.end(), out_edges.begin(), out_edges.end());
    std::ranges::inplace_merge(
      succ, std::ranges::next(
              succ.begin(), static_cast<std::ptrdiff_t>(middle_offset)));
  }

  const auto [beg, end] = std::ranges::unique(succ);
  succ.erase(beg, end);
  return succ;
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
[[nodiscard]] auto predecessors(
  const NetT& net, const typename NetT::EdgeType& e, AdjT& adj, bool reduce)
  -> std::vector<typename NetT::EdgeType> {
  std::vector<typename NetT::EdgeType> pred;
  pred.reserve(e.mutator_verts().size());

  for (const auto& v : e.mutator_verts()) {
    std::size_t middle_offset = pred.size();
    auto in_edges = detail::vertex_predecessors(net, e, v, adj, reduce);
    pred.reserve(pred.size() + in_edges.size());
    pred.insert(pred.end(), in_edges.begin(), in_edges.end());
    std::ranges::inplace_merge(
      pred,
      std::ranges::next(
        pred.begin(), static_cast<std::ptrdiff_t>(middle_offset)),
      effect_lt);
  }

  const auto [beg, end] = std::ranges::unique(pred);
  pred.erase(beg, end);
  return pred;
}
} // namespace reticula::event_graph
