#include <algorithm>
#include <initializer_list>
#include <ranges>

#include <reticula/concepts.hpp>
#include <reticula/edges.hpp>
#include <reticula/networks.hpp>
#include <unordered_set>

namespace reticula {
template <network_edge EdgeT>
network<EdgeT>::network(std::initializer_list<EdgeT> edges)
    : network(std::views::all(edges), std::views::empty<VertexType>) {}

template <network_edge EdgeT>
network<EdgeT>::network(
  std::initializer_list<EdgeT> edges, std::initializer_list<VertexType> verts)
    : network(std::views::all(edges), std::views::all(verts)) {}

template <network_edge EdgeT>
auto network<EdgeT>::edges() const -> std::span<const EdgeT> {
  return edges_cause_;
}

template <network_edge EdgeT>
auto network<EdgeT>::edges_cause() const -> std::span<const EdgeT> {
  return edges_cause_;
}

template <network_edge EdgeT>
auto network<EdgeT>::edges_effect() const -> std::span<const EdgeT> {
  if constexpr (instantaneous_undirected)
    return edges_cause_;

  return edges_effect_;
}

template <network_edge EdgeT>
auto network<EdgeT>::vertices() const -> std::span<const VertexType> {
  return verts_;
}

template <network_edge EdgeT>
auto network<EdgeT>::operator==(const network<EdgeT>& other) const noexcept
  -> bool {
  return edges_cause_ == other.edges_cause_ && verts_ == other.verts_;
}

// incidence list functions

template <network_edge EdgeT>
auto network<EdgeT>::in_edges(VertexType v) const -> std::span<const EdgeT> {
  if constexpr (instantaneous_undirected)
    return out_edges(v);
  else {
    auto idx = const_cast<mphf_t&>(offset_map_).lookup(v);
    if (idx >= offsets_.size() || offsets_[idx].vertex != v)
      return {};

    std::span<const EdgeT> in_edges{in_edges_};
    const std::size_t first = offsets_[idx].in_offset;
    const std::size_t count = offsets_[idx + 1].in_offset - first;

    return in_edges.subspan(first, count);
  }
}

template <network_edge EdgeT>
auto network<EdgeT>::out_edges(VertexType v) const -> std::span<const EdgeT> {
  auto idx = const_cast<mphf_t&>(offset_map_).lookup(v);
  if (idx >= offsets_.size() || offsets_[idx].vertex != v)
    return {};

  std::span<const EdgeT> out_edges{out_edges_};
  const std::size_t first = offsets_[idx].out_offset;
  const std::size_t count = offsets_[idx + 1].out_offset - first;

  return out_edges.subspan(first, count);
}

template <network_edge EdgeT>
auto network<EdgeT>::incident_edges(VertexType v) const -> std::vector<EdgeT> {
  if constexpr (instantaneous_undirected) {
    auto out = out_edges(v);
    return std::vector<EdgeT>{out.begin(), out.end()};
  }

  auto in = in_edges(v);
  auto out = out_edges(v);
  std::vector<EdgeT> res;
  res.reserve(in.size() + out.size());
  std::ranges::set_union(in, out, std::back_inserter(res));
  return res;
}

template <network_edge EdgeT>
auto network<EdgeT>::in_degree(VertexType v) const -> std::size_t {
  return in_edges(v).size();
}

template <network_edge EdgeT>
auto network<EdgeT>::out_degree(VertexType v) const -> std::size_t {
  return out_edges(v).size();
}

template <network_edge EdgeT>
auto network<EdgeT>::degree(VertexType v) const -> std::size_t {
  if constexpr (instantaneous_undirected)
    return out_degree(v);

  return incident_edges(v).size();
}

template <network_edge EdgeT>
auto network<EdgeT>::predecessors(VertexType v) const
  -> std::vector<VertexType> {
  if constexpr (instantaneous_undirected)
    return successors(v);
  auto ie = in_edges(v);

  std::unordered_set<VertexType> preds;
  preds.reserve(ie.size());
  for (const auto& e : ie)
    for (const auto& u : e.mutator_verts())
      if (u != v)
        preds.insert(u);

  return {preds.begin(), preds.end()};
}

template <network_edge EdgeT>
auto network<EdgeT>::successors(VertexType v) const -> std::vector<VertexType> {
  auto oe = out_edges(v);

  std::unordered_set<VertexType> succ;
  succ.reserve(oe.size());
  for (const auto& e : oe)
    for (const auto& u : e.mutated_verts())
      if (u != v)
        succ.insert(u);

  return {succ.begin(), succ.end()};
}

template <network_edge EdgeT>
auto network<EdgeT>::neighbours(VertexType v) const -> std::vector<VertexType> {
  if constexpr (instantaneous_undirected)
    return successors(v);

  auto ie = in_edges(v);
  auto oe = out_edges(v);
  std::unordered_set<VertexType> neigh;
  neigh.reserve(ie.size() + oe.size());
  for (const auto& e : ie)
    for (const auto& u : e.mutator_verts())
      if (u != v)
        neigh.insert(u);

  for (const auto& e : oe)
    for (const auto& u : e.mutated_verts())
      if (u != v)
        neigh.insert(u);

  return {neigh.begin(), neigh.end()};
}

template <network_edge EdgeT>
auto network<EdgeT>::vertex_id(VertexType v) const -> std::size_t {
  std::size_t idx = const_cast<mphf_t&>(offset_map_).lookup(v);
  if (idx >= offsets_.size() || offsets_[idx].vertex != v)
    throw std::out_of_range("Vertex not found in the network");
  return idx;
}

template <network_edge EdgeT>
auto network<EdgeT>::id_vertex(std::size_t idx) const -> VertexType {
  if (idx >= offsets_.size())
    throw std::out_of_range("Vertex not found in the network");
  return offsets_[idx].vertex;
}

template <network_edge EdgeT>
auto network<EdgeT>::has_vertex(VertexType v) const -> bool {
  std::size_t idx = const_cast<mphf_t&>(offset_map_).lookup(v);
  return idx < offsets_.size() && offsets_[idx].vertex == v;
}

template <network_edge EdgeT>
auto network<EdgeT>::has_edge(const EdgeType& e) const -> bool {
  // TODO: can be improved by searching incidence lists, but possibility of
  // hyperedges with empty head and/or tail makes it more complex than it should
  // be.
  return std::ranges::binary_search(edges_cause_, e);
}

template class reticula::network<reticula::undirected_edge>;
template class reticula::network<reticula::directed_edge>;

template class reticula::network<reticula::undirected_hyperedge>;
template class reticula::network<reticula::directed_hyperedge>;

template class reticula::network<reticula::undirected_temporal_edge>;
template class reticula::network<reticula::directed_temporal_edge>;
template class reticula::network<reticula::directed_delayed_temporal_edge>;

template class reticula::network<reticula::undirected_temporal_hyperedge>;
template class reticula::network<reticula::directed_temporal_hyperedge>;
template class reticula::network<reticula::directed_delayed_temporal_hyperedge>;
} // namespace reticula
