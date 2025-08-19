#pragma once

#include <algorithm>
#include <initializer_list>
#include <ranges>
#include <span>
#include <unordered_set>
#include <vector>

#include <BooPHF.h>

#include <reticula/concepts.hpp>
#include <reticula/edges.hpp>

namespace reticula {
/**
  Generic network class. Internally, the network class stores:
  1. An edge list, which is a sorted list of edges in the network, and
  2. An incidence list, which is a sorted list of edges incident to each vertex.

  @tparam EdgeT Edge type for this network. Different edge types encapsulate
  different behaviour in networks, e.g. a directed network is a network with
  directed edges.
 */
template <network_edge EdgeT>
class network {
public:
  using EdgeType = EdgeT;

  network() = default;
  /**
      Create an network from a range of edges. This variation is specifically
      created so that a brace-enclosed initializer list can be used to
      initialize the class.
     */
  network(std::initializer_list<EdgeType> edges);

  /**
    Create an network from a range of edges and a supplementary set of
    vertices. This variation is specifically created so that a brace-enclosed
    initializer list can be used to initialize the class.

    @param edges A range consisting of all edges present in the network.
    @param verts The range of vertices. It is used to supplement the vertices
    present in the provided set of edges, i.e. it only needs to contain
    vertices that have no incident edges.
   */
  network(
    std::initializer_list<EdgeType> edges,
    std::initializer_list<VertexType> verts);

  /**
    Create an network from a range of edges.
   */
  template <std::ranges::input_range EdgeRange>
    requires std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT>
  explicit network(EdgeRange&& edges);

  /**
    Create an network from a range of edges and a supplementary range of
    vertices.

    @param edges A range consisting of all edges present in the network.
    @param verts The range of vertices. It is used to supplement the vertices
    present in the provided set of edges, i.e. it only needs to contain
    vertices that have no incident edges.
   */
  template <
    std::ranges::input_range EdgeRange, std::ranges::input_range VertRange>
    requires std::convertible_to<
               std::ranges::range_value_t<EdgeRange>, EdgeT> &&
             std::convertible_to<
               std::ranges::range_value_t<VertRange>, VertexType>
  explicit network(EdgeRange&& edges, VertRange&& verts);

  /**
    list of unique vertices in the network sorted by operator<.
   */
  [[nodiscard]]
  auto vertices() const -> std::span<const VertexType>;

  /**
    List of unique edges in the network sorted by operator<.
   */
  [[nodiscard]]
  auto edges() const -> std::span<const EdgeType>;

  /**
    List of unique edges in the network sorted by operator<.
   */
  [[nodiscard]]
  auto edges_cause() const -> std::span<const EdgeType>;

  /**
    List of unique edges in the network sorted by effect_lt.
   */
  [[nodiscard]]
  auto edges_effect() const -> std::span<const EdgeType>;

  /**
    List of edges in network incident to `vert`, i.e. 'vert' is mutated by
    them. Edges are sorted by `effect_lt(e1, e2)`.
   */
  [[nodiscard]]
  auto in_edges(VertexType vert) const -> std::span<const EdgeType>;

  /**
    List of edges in network which `vert` is incident to, i.e. where 'vert' is
    a mutator of. Edges are sorted by `operator<(e1, e2)`.
   */
  [[nodiscard]]
  auto out_edges(VertexType vert) const -> std::span<const EdgeType>;

  /**
    List of edges in network which `vert` is a participant, i.e. where 'vert'
    is a mutator of or is mutated by that edge. Edges are sorted by
    `operator<(e1, e2)`.
   */
  [[nodiscard]]
  auto incident_edges(VertexType vert) const -> std::vector<EdgeType>;

  /**
    Number of edges incident to `vert`. Similart to `in_edges(vert).size()`
   */
  [[nodiscard]]
  auto in_degree(VertexType vert) const -> std::size_t;

  /**
    Number of edges that `vert` is incident to. Similart to
    `out_edges(vert).size()`
   */
  [[nodiscard]]
  auto out_degree(VertexType vert) const -> std::size_t;

  /**
    Number of edges that `vert` participates in. Similart to
    `incident_edges(vert).size()`
   */
  [[nodiscard]]
  auto degree(VertexType vert) const -> std::size_t;

  /**
    List of vertices that are mutators in at least one edge where 'v' is
    mutated.
   */
  [[nodiscard]]
  auto predecessors(VertexType v) const -> std::vector<VertexType>;

  /**
    List of vertices that are mutated in at least one edge where 'v' is a
    mutator.
   */
  [[nodiscard]]
  auto successors(VertexType v) const -> std::vector<VertexType>;

  /**
    List of vertices that participate in at least one edge with 'v'.
   */
  [[nodiscard]]
  auto neighbours(VertexType v) const -> std::vector<VertexType>;

  /**
    Two networks are equal if their set of vertices and edges are equal.
  */
  [[nodiscard]]
  auto operator==(const network<EdgeT>& other) const noexcept -> bool;

  [[nodiscard]]
  auto operator!=(const network<EdgeT>& other) const noexcept -> bool = default;

  [[nodiscard]]
  auto vertex_id(VertexType v) const -> std::size_t;

  [[nodiscard]]
  auto id_vertex(std::size_t idx) const -> VertexType;

  auto has_vertex(VertexType v) const -> bool;
  auto contains(VertexType v) const -> bool;

  auto has_edge(const EdgeType& e) const -> bool;

private:
  static constexpr bool instantaneous_undirected =
    is_instantaneous_v<EdgeType> && is_undirected_v<EdgeType>;

  // edge list(s)
  std::vector<EdgeType> edges_cause_;
  std::vector<EdgeType> edges_effect_;
  std::vector<VertexType> verts_;

  // incidence list(s)
  std::vector<EdgeType> in_edges_;
  std::vector<EdgeType> out_edges_;

  struct offsets {
    VertexType vertex;
    [[no_unique_address]]
    std::conditional_t<instantaneous_undirected, std::monostate, std::size_t>
      in_offset;
    std::size_t out_offset;

    [[nodiscard]] auto operator==(const offsets& other) const noexcept -> bool;
  };
  std::vector<offsets> offsets_;

  using mphf_t =
    boomphf::mphf<VertexType, boomphf::SingleHashFunctor<VertexType>>;
  mphf_t offset_map_;
};

template <network_edge EdgeT>
template <std::ranges::input_range EdgeRange>
  requires std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT>
network<EdgeT>::network(EdgeRange&& edges)
    : network{
        std::forward<EdgeRange>(edges),
        std::ranges::empty_view<VertexType>{}} {}

template <network_edge EdgeT>
template <
  std::ranges::input_range EdgeRange, std::ranges::input_range VertRange>
  requires std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT> &&
           std::convertible_to<
             std::ranges::range_value_t<VertRange>, VertexType>
network<EdgeT>::network(EdgeRange&& edges, VertRange&& verts) {
  if constexpr (std::ranges::sized_range<EdgeRange>)
    edges_cause_.reserve(std::ranges::size(edges));

  std::ranges::copy(
    std::forward<EdgeRange>(edges), std::back_inserter(edges_cause_));
  std::ranges::sort(edges_cause_);
  const auto [b, e] = std::ranges::unique(edges_cause_);
  edges_cause_.erase(b, e);
  edges_cause_.shrink_to_fit();

  std::unordered_set<VertexType> verts_set;
  std::unordered_map<VertexType, std::size_t> out_counts;
  std::unordered_map<VertexType, std::size_t> in_counts;
  if constexpr (std::ranges::sized_range<VertRange>) {
    verts_set.reserve(std::ranges::size(verts));
    out_counts.reserve(std::ranges::size(verts));
    if constexpr (!instantaneous_undirected)
      in_counts.reserve(std::ranges::size(verts));
  }

  for (auto&& v : verts)
    verts_set.insert(v);

  for (const auto& e : edges_cause_) {
    for (auto&& v : e.mutator_verts()) {
      ++out_counts[v];
      verts_set.insert(v);
    }

    if constexpr (!instantaneous_undirected) {
      for (auto&& v : e.mutated_verts()) {
        ++in_counts[v];
        verts_set.insert(v);
      }
    }
  }
  verts_.reserve(verts_set.size());
  std::ranges::copy(verts_set, std::back_inserter(verts_));
  std::ranges::sort(verts_);

  offset_map_ = mphf_t{verts_.size(), verts_, 1, 2.0, false, false, 0.03f};
  offsets_.resize(verts_.size() + 1);
  for (unsigned long v : verts_) {
    const auto idx = offset_map_.lookup(v);
    if constexpr (!instantaneous_undirected)
      offsets_[idx] = {v, in_counts[v], out_counts[v]};
    else
      offsets_[idx] = {v, std::monostate{}, out_counts[v]};
  }

  std::size_t acc_out = 0, acc_in = 0;
  for (auto&& o : offsets_) {
    if constexpr (!instantaneous_undirected) {
      auto cnt_in = o.in_offset;
      o.in_offset = acc_in;
      acc_in += cnt_in;
    }

    auto cnt_out = o.out_offset;
    o.out_offset = acc_out;
    acc_out += cnt_out;
  }

  out_edges_.resize(acc_out);
  std::vector<std::size_t> cursor(verts_.size(), 0uz);
  for (const auto& e : edges_cause_) {
    for (auto&& v : e.mutator_verts()) {
      const auto idx = offset_map_.lookup(v);
      out_edges_[offsets_[idx].out_offset + cursor[idx]] = e;
      ++cursor[idx];
    }
  }

  if constexpr (!instantaneous_undirected) {
    edges_effect_ = edges_cause_;
    std::ranges::sort(edges_effect_, effect_lt);

    std::ranges::fill(cursor, 0uz);

    in_edges_.resize(acc_in);
    for (const auto& e : edges_effect_) {
      for (auto&& v : e.mutated_verts()) {
        const auto idx = offset_map_.lookup(v);
        in_edges_[offsets_[idx].in_offset + cursor[idx]] = e;
        ++cursor[idx];
      }
    }
  }
}

extern template class network<directed_edge>;
using directed_network = network<directed_edge>;
extern template class network<undirected_edge>;
using undirected_network = network<undirected_edge>;

extern template class network<directed_hyperedge>;
using directed_hypernetwork = network<directed_hyperedge>;
extern template class network<undirected_hyperedge>;
using undirected_hypernetwork = network<undirected_hyperedge>;

extern template class network<undirected_temporal_edge>;
using undirected_temporal_network = network<undirected_temporal_edge>;
extern template class network<directed_temporal_edge>;
using directed_temporal_network = network<directed_temporal_edge>;
extern template class network<directed_delayed_temporal_edge>;
using directed_delayed_temporal_network =
  network<directed_delayed_temporal_edge>;

extern template class network<undirected_temporal_hyperedge>;
using undirected_temporal_hypernetwork = network<undirected_temporal_hyperedge>;
extern template class network<directed_temporal_edge>;
using directed_temporal_hypernetwork = network<directed_temporal_hyperedge>;
extern template class network<directed_delayed_temporal_edge>;
using directed_delayed_temporal_hypernetwork =
  network<directed_delayed_temporal_hyperedge>;
} // namespace reticula
