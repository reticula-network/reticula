#ifndef INCLUDE_RETICULA_NETWORKS_HPP_
#define INCLUDE_RETICULA_NETWORKS_HPP_

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "ranges.hpp"
#include "network_concepts.hpp"
#include "static_edges.hpp"
#include "temporal_edges.hpp"
#include "static_hyperedges.hpp"
#include "temporal_hyperedges.hpp"

namespace reticula {
  /**
    Generic network class, storing a set of edges with fast access to (in- and
    out-) incident edges to each node, neighbours and degrees.

    @tparam EdgeT Edge type for this network. Different edge types encapsulate
    different behaviour in networks, e.g. a directed network is a network with
    directed edges.
   */
  template <network_edge EdgeT>
  class network {
  public:
    /**
      Type of the edges in the network.
     */
    using EdgeType = EdgeT;

    /**
      Type used for labelling vertices, derived from EdgeType of network.
     */
    using VertexType = typename EdgeType::VertexType;

    network() = default;

    /**
      Create a network from a range of edges. This variation is specifically
      created so that a brace-enclosed initializer list can be used to
      initialize the class.
     */
    network(std::initializer_list<EdgeType> edges);

    /**
      Create a network from a range of edges and a supplementary set of
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
      Create a network from a range of edges.
     */
    template <ranges::input_range EdgeRange>
    requires std::convertible_to<
      ranges::range_value_t<EdgeRange>, EdgeT>
    explicit network(EdgeRange&& edges);

    /**
      Create a network from a range of edges and a supplementary range of
      vertices.

      @param edges A range consisting of all edges present in the network.
      @param verts The range of vertices. It is used to supplement the vertices
      present in the provided set of edges, i.e. it only needs to contain
      vertices that have no incident edges.
     */
    template <
      ranges::input_range EdgeRange,
      ranges::input_range VertRange>
    requires
      std::convertible_to<
        ranges::range_value_t<EdgeRange>, EdgeT> &&
      std::convertible_to<
        ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
    explicit network(EdgeRange&& edges, VertRange&& verts);

    /**
      list of unique vertices participating at least in one event in the
      network sorted by operator<.
     */
    [[nodiscard]]
    const std::vector<VertexType>& vertices() const;

    /**
      List of unique edges in the network sorted by operator<.
     */
    [[nodiscard]]
    const std::vector<EdgeType>& edges() const;

    /**
      List of unique edges in the network sorted by operator<.
     */
    [[nodiscard]]
    const std::vector<EdgeType>& edges_cause() const;

    /**
      List of unique edges in the network sorted by effect_lt.
     */
    [[nodiscard]]
    const std::vector<EdgeType>& edges_effect() const;

    /**
      List of edges in network incident to `vert`, i.e. 'vert' is mutated by
      them. Edges are sorted by `effect_lt(e1, e2)`.
     */
    [[nodiscard]]
    std::vector<EdgeType> in_edges(const VertexType& vert) const;

    /**
      List of edges in network which `vert` is incident to, i.e. where 'vert' is
      a mutator of. Edges are sorted by `operator<(e1, e2)`.
     */
    [[nodiscard]]
    std::vector<EdgeType> out_edges(const VertexType& vert) const;

    /**
      Map of vertices and edges in network that are incident to them, i.e.
      'vert' is mutated by them. Edges are sorted by `effect_lt(e1, e2)`.
     */
    [[nodiscard]]
    const std::unordered_map<
      VertexType, std::vector<EdgeType>,
      hash<VertexType>>&
    in_edges() const;

    /**
      Map of vertices and edges in network which they are incident to, i.e.
      where 'vert' is a mutator of. Edges are sorted by `operator<(e1, e2)`.
     */
    [[nodiscard]]
    const std::unordered_map<
      VertexType, std::vector<EdgeType>,
      hash<VertexType>>&
    out_edges() const;

    /**
      List of edges in network which `vert` is a participant, i.e. where 'vert'
      is a mutator of or is mutated by that edge. Edges are sorted by
      `operator<(e1, e2)`.
     */
    [[nodiscard]]
    std::vector<EdgeType> incident_edges(const VertexType& vert) const;

    /**
      Number of edges incident to `vert`. Similart to `in_edges(vert).size()`
     */
    [[nodiscard]]
    size_t in_degree(const VertexType& vert) const;

    /**
      Number of edges that `vert` is incident to. Similart to
      `out_edges(vert).size()`
     */
    [[nodiscard]]
    size_t out_degree(const VertexType& vert) const;

    /**
      Number of edges that `vert` participates in. Similart to
      `incident_edges(vert).size()`
     */
    [[nodiscard]]
    size_t degree(const VertexType& vert) const;



    /**
      List of vertices that are mutators in at least one edge where 'v' is
      mutated.
     */
    [[nodiscard]]
    std::vector<VertexType> predecessors(const VertexType& v) const;

    /**
      List of vertices that are mutated in at least one edge where 'v' is a
      mutator.
     */
    [[nodiscard]]
    std::vector<VertexType> successors(const VertexType& v) const;

    /**
      List of vertices that participate in at least one edge with 'v'.
     */
    [[nodiscard]]
    std::vector<VertexType> neighbours(const VertexType& v) const;

    /**
      Returns a graph that is the union (not the disjoint union) of this graph
      and the argument.
    */
    [[nodiscard]]
    network<EdgeT> union_with(const network<EdgeT>& other) const;

    /**
      Two networks are equal if their set of vertices and edges are equal.
    */
    [[nodiscard]]
    bool operator==(const network<EdgeT>& other) const;

    [[nodiscard]]
    bool operator!=(const network<EdgeT>& other) const = default;

  private:
    std::vector<EdgeType> _edges_cause;
    std::vector<EdgeType> _edges_effect;
    std::vector<VertexType> _verts;
    std::unordered_map<
      VertexType, std::vector<EdgeType>,
      hash<VertexType>> _in_edges;
    std::unordered_map<
      VertexType, std::vector<EdgeType>,
      hash<VertexType>> _out_edges;

    static constexpr bool instantaneous_undirected =
      is_instantaneous_v<EdgeType> && is_undirected_v<EdgeType>;
  };

  template <network_edge EdgeT>
  struct is_instantaneous<network<EdgeT>> {
    static constexpr bool value = is_instantaneous_v<EdgeT>;
  };

  template <network_edge EdgeT>
  struct is_undirected<network<EdgeT>> {
    static constexpr bool value = is_undirected_v<EdgeT>;
  };

  template <network_edge EdgeT>
  struct is_directed<network<EdgeT>> {
    static constexpr bool value = is_directed_v<EdgeT>;
  };

  template <network_edge EdgeT>
  struct is_dyadic<network<EdgeT>> {
    static constexpr bool value = is_dyadic_v<EdgeT>;
  };

  /**
    Directed network class. network class with directed edges.
   */
  template <class VertT>
  using directed_network = network<directed_edge<VertT>>;

  /**
    Undirected network class. network class with undirected edges.
   */
  template <class VertT>
  using undirected_network = network<undirected_edge<VertT>>;

  /**
    Directed temporal network class. network class with directed temporal edges.
   */
  template <class VertT, class TimeT>
  using directed_temporal_network =
    network<directed_temporal_edge<VertT, TimeT>>;

  /**
    Directed delayed temporal network class. network class with directed delayed
    temporal edges.
   */
  template <class VertT, class TimeT>
  using directed_delayed_temporal_network =
    network<directed_delayed_temporal_edge<VertT, TimeT>>;

  /**
    Undirected temporal network class. network class with undirected temporal
    edges.
   */
  template <typename VertT, typename TimeT>
  using undirected_temporal_network =
    network<undirected_temporal_edge<VertT, TimeT>>;


  /**
    Directed hypernetwork class. network class with directed hyperedges.
   */
  template <class VertT>
  using directed_hypernetwork = network<directed_hyperedge<VertT>>;

  /**
    Undirected hypernetwork class. network class with undirected hyperedges.
   */
  template <class VertT>
  using undirected_hypernetwork = network<undirected_hyperedge<VertT>>;

  /**
    Directed temporal hypernetwork class. network class with directed temporal
    hyperedges.
   */
  template <class VertT, class TimeT>
  using directed_temporal_hypernetwork =
    network<directed_temporal_hyperedge<VertT, TimeT>>;

  /**
    Directed delayed temporal hypernetwork class. network class with directed
    delayed temporal hyperedges.
   */
  template <class VertT, class TimeT>
  using directed_delayed_temporal_hypernetwork =
    network<directed_delayed_temporal_hyperedge<VertT, TimeT>>;

  /**
    Undirected temporal hypernetwork class. network class with undirected
    temporal hyperedges.
   */
  template <typename VertT, typename TimeT>
  using undirected_temporal_hypernetwork =
    network<undirected_temporal_hyperedge<VertT, TimeT>>;
}  // namespace reticula

// Implementation
#include <unordered_set>

namespace reticula {
  template <network_edge EdgeT>
  network<EdgeT>::network(std::initializer_list<EdgeT> edges)
  : network(
      std::vector<EdgeT>(edges),
      std::vector<typename EdgeT::VertexType>()) {}

  template <network_edge EdgeT>
  network<EdgeT>::network(
      std::initializer_list<EdgeT> edges,
      std::initializer_list<typename EdgeT::VertexType> verts)
  : network(
      std::vector<EdgeT>(edges),
      std::vector<typename EdgeT::VertexType>(verts)) {}

  template <network_edge EdgeT>
  template <ranges::input_range EdgeRange>
  requires std::convertible_to<ranges::range_value_t<EdgeRange>, EdgeT>
  network<EdgeT>::network(EdgeRange&& edges)
  : network(edges, std::vector<typename EdgeT::VertexType>()) {}

  template <network_edge EdgeT>
  template <
    ranges::input_range EdgeRange,
    ranges::input_range VertRange>
  requires
    std::convertible_to<ranges::range_value_t<EdgeRange>, EdgeT> &&
    std::convertible_to<
      ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
  network<EdgeT>::network(EdgeRange&& edges, VertRange&& verts) {
    if constexpr (ranges::sized_range<EdgeRange>)
      _edges_cause.reserve(ranges::size(edges));
    ranges::copy(edges, std::back_inserter(_edges_cause));
    std::sort(_edges_cause.begin(), _edges_cause.end());
    _edges_cause.erase(std::unique(_edges_cause.begin(), _edges_cause.end()),
        _edges_cause.end());
    _edges_cause.shrink_to_fit();

    if (!instantaneous_undirected) {
      _edges_effect = _edges_cause;
      std::sort(_edges_effect.begin(), _edges_effect.end(),
            [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
    }

    std::unordered_set<typename EdgeT::VertexType,
      hash<typename EdgeT::VertexType>> verts_set;
    if (ranges::sized_range<VertRange>)
      verts_set.reserve(ranges::size(verts));
    for (const auto& v: verts)
      verts_set.insert(v);

    _out_edges.reserve(verts_set.size());
    if (!instantaneous_undirected)
      _in_edges.reserve(verts_set.size());

    for (const auto& e: _edges_cause) {
      for (auto&& v: e.mutator_verts())
        _out_edges[v].push_back(e);

      if (!instantaneous_undirected)
        for (auto&& v: e.mutated_verts())
          _in_edges[v].push_back(e);
    }

    for (auto&& v: _in_edges | ranges::views::keys)
      verts_set.insert(v);
    for (auto&& v: _out_edges | ranges::views::keys)
      verts_set.insert(v);

    _verts = std::vector<typename EdgeT::VertexType>(
        verts_set.begin(), verts_set.end());
    ranges::sort(_verts);

    if (!instantaneous_undirected) {
      for (auto&& [v, e_list]: _in_edges) {
        std::sort(e_list.begin(), e_list.end(),
            [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
        e_list.erase(std::unique(e_list.begin(), e_list.end()),
            e_list.end());
        e_list.shrink_to_fit();
      }
    }

    for (auto&& [v, e_list]: _out_edges) {
      std::sort(e_list.begin(), e_list.end());
      e_list.erase(std::unique(e_list.begin(), e_list.end()),
          e_list.end());
      e_list.shrink_to_fit();
    }
  }

  template <network_edge EdgeT>
  size_t network<EdgeT>::in_degree(
      const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return out_degree(v);

    auto p = _in_edges.find(v);
    if (p == _in_edges.end())
      return 0;
    else
      return p->second.size();
  }

  template <network_edge EdgeT>
  size_t network<EdgeT>::out_degree(
      const typename EdgeT::VertexType& v) const {
    auto p = _out_edges.find(v);
    if (p == _out_edges.end())
      return 0;
    else
      return p->second.size();
  }

  template <network_edge EdgeT>
  size_t network<EdgeT>::degree(
      const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return out_degree(v);

    return incident_edges(v).size();
  }

  template <network_edge EdgeT>
  std::vector<EdgeT>
  network<EdgeT>::in_edges(
      const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return out_edges(v);

    auto p = _in_edges.find(v);
    if (p == _in_edges.end())
      return std::vector<EdgeT>();
    else
      return p->second;
  }

  template <network_edge EdgeT>
  const std::unordered_map<
    typename EdgeT::VertexType, std::vector<EdgeT>,
    hash<typename EdgeT::VertexType>>&
  network<EdgeT>::in_edges() const {
    if (instantaneous_undirected)
      return _out_edges;
    return _in_edges;
  }

  template <network_edge EdgeT>
  std::vector<EdgeT>
  network<EdgeT>::out_edges(
      const typename EdgeT::VertexType& v) const {
    auto p = _out_edges.find(v);
    if (p == _out_edges.end())
      return std::vector<EdgeT>();
    else
      return p->second;
  }

  template <network_edge EdgeT>
  const std::unordered_map<
    typename EdgeT::VertexType, std::vector<EdgeT>,
    hash<typename EdgeT::VertexType>>&
  network<EdgeT>::out_edges() const {
    return _out_edges;
  }

  template <network_edge EdgeT>
  std::vector<EdgeT>
  network<EdgeT>::incident_edges(
      const typename EdgeT::VertexType& v) const {
    std::vector<EdgeT> inc(out_edges(v));

    if (!instantaneous_undirected) {
      auto in = in_edges(v);
      inc.insert(inc.end(), in.begin(), in.end());

      std::sort(inc.begin(), inc.end());
      inc.erase(std::unique(inc.begin(), inc.end()), inc.end());
    }

    return inc;
  }

  template <network_edge EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::predecessors(const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return successors(v);

    std::unordered_set<
      typename EdgeT::VertexType,
      hash<typename EdgeT::VertexType>> preds;
    auto p = _in_edges.find(v);
    if (p != _in_edges.end()) {
      preds.reserve(p->second.size());
      for (auto&& e: p->second)
        for (auto&& u: e.mutator_verts())
          if (u != v) preds.insert(u);
    }
    return std::vector<typename EdgeT::VertexType>(preds.begin(), preds.end());
  }

  template <network_edge EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::successors(const typename EdgeT::VertexType& v) const {
    std::unordered_set<
      typename EdgeT::VertexType,
      hash<typename EdgeT::VertexType>> succ;
    auto p = _out_edges.find(v);
    if (p != _out_edges.end()) {
      succ.reserve(p->second.size());
      for (auto&& e: p->second)
        for (auto&& u: e.mutated_verts())
          if (u != v) succ.insert(u);
    }
    return std::vector<typename EdgeT::VertexType>(succ.begin(), succ.end());
  }

  template <network_edge EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::neighbours(const typename EdgeT::VertexType& v) const {
    std::vector<typename EdgeT::VertexType> inc(successors(v));

    if (!instantaneous_undirected) {
      std::vector<typename EdgeT::VertexType> pred(predecessors(v));
      inc.insert(inc.end(), pred.begin(), pred.end());

      std::sort(inc.begin(), inc.end());
      inc.erase(std::unique(inc.begin(), inc.end()), inc.end());
    }
    return inc;
  }

  template <network_edge EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges() const {
    return _edges_cause;
  }

  template <network_edge EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges_cause() const {
    return _edges_cause;
  }

  template <network_edge EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges_effect() const {
    if (instantaneous_undirected)
      return _edges_cause;

    return _edges_effect;
  }

  template <network_edge EdgeT>
  const std::vector<typename EdgeT::VertexType>&
  network<EdgeT>::vertices() const {
    return _verts;
  }

  template <network_edge EdgeT>
  network<EdgeT>
  network<EdgeT>::union_with(const network<EdgeT>& other) const {
    network<EdgeT> res(*this);
    if (!instantaneous_undirected) {
      for (auto& [v, es]: other._in_edges) {
        auto& res_edges = res._in_edges[v];
        auto mid = res_edges.insert(res_edges.end(), es.begin(), es.end());
        if (mid > res_edges.begin()) {
          std::inplace_merge(
              res_edges.begin(),
              mid, res_edges.end(),
              [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
          auto to_erase = ranges::unique(res_edges);
          res_edges.erase(to_erase.begin(), to_erase.end());
        }
      }
    }

    for (auto& [v, es]: other._out_edges) {
      auto& res_edges = res._out_edges[v];
      auto mid = res_edges.insert(res_edges.end(), es.begin(), es.end());
      if (mid > res_edges.begin()) {
        std::inplace_merge(
            res_edges.begin(),
            mid, res_edges.end());
        auto to_erase = ranges::unique(res_edges);
        res_edges.erase(to_erase.begin(), to_erase.end());
      }
    }

    if (!instantaneous_undirected) {
      auto mid_effect = res._edges_effect.insert(
          res._edges_effect.end(),
          other._edges_effect.begin(),
          other._edges_effect.end());
      std::inplace_merge(
          res._edges_effect.begin(),
          mid_effect,
          res._edges_effect.end(),
          [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
      auto to_erase_effect = ranges::unique(res._edges_effect);
      res._edges_effect.erase(to_erase_effect.begin(), to_erase_effect.end());
    }

    auto mid_cause = res._edges_cause.insert(
        res._edges_cause.end(),
        other._edges_cause.begin(),
        other._edges_cause.end());
    std::inplace_merge(
        res._edges_cause.begin(),
        mid_cause,
        res._edges_cause.end());
    auto to_erase_cause = ranges::unique(res._edges_cause);
    res._edges_cause.erase(to_erase_cause.begin(), to_erase_cause.end());

    auto mid_verts = res._verts.insert(
        res._verts.end(),
        other._verts.begin(),
        other._verts.end());
    std::inplace_merge(
        res._verts.begin(),
        mid_verts,
        res._verts.end());
    auto to_erase_verts = ranges::unique(res._verts);
    res._verts.erase(to_erase_verts.begin(), to_erase_verts.end());

    return res;
  }

  template <network_edge EdgeT>
  bool network<EdgeT>::operator==(const network<EdgeT>& other) const {
    return _edges_cause == other._edges_cause && _verts == other._verts;
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_NETWORKS_HPP_
