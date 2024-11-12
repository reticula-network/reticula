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

    for (const auto& e: _edges_cause) {
      for (auto&& v: e.mutator_verts())
        _out_edges[v].push_back(e);

      if (!instantaneous_undirected)
        for (auto&& v: e.mutated_verts())
          _in_edges[v].push_back(e);
    }

    std::unordered_set<typename EdgeT::VertexType,
      hash<typename EdgeT::VertexType>> verts_set;
    for (auto&& v: _in_edges | ranges::views::keys)
      verts_set.insert(v);
    for (auto&& v: _out_edges | ranges::views::keys)
      verts_set.insert(v);

    // adding the supplemental set of verts
    for (const auto& v: verts)
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

    return in_edges(v).size();
  }

  template <network_edge EdgeT>
  size_t network<EdgeT>::out_degree(
      const typename EdgeT::VertexType& v) const {
    return out_edges(v).size();
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
