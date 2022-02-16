#include <set>

namespace dag {
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
  template <std::ranges::input_range EdgeRange>
  requires std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT>
  network<EdgeT>::network(const EdgeRange& edges)
  : network(edges, std::vector<typename EdgeT::VertexType>()) {}

  template <network_edge EdgeT>
  template <
    std::ranges::input_range EdgeRange,
    std::ranges::input_range VertRange>
  requires
    std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT> &&
    std::convertible_to<
      std::ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
  network<EdgeT>::network(const EdgeRange& edges, const VertRange& verts) {
    if constexpr (std::ranges::sized_range<EdgeRange>)
      _edges_cause.reserve(std::ranges::size(edges));
    std::ranges::copy(edges, std::back_inserter(_edges_cause));
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

    // adding the supplemental set of verts
    for (const auto& v: verts) {
      _out_edges.try_emplace(v);
      if (!instantaneous_undirected)
        _in_edges.try_emplace(v);
    }

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
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::vertices() const {
    std::set<network<EdgeT>::VertexType> verts;
    if (!instantaneous_undirected)
      for (const auto &p: _in_edges)
        verts.insert(p.first);
    for (const auto &p: _out_edges)
      verts.insert(p.first);
    return std::vector<typename EdgeT::VertexType>(verts.begin(), verts.end());
  }

  template <network_edge EdgeT>
  network<EdgeT>
  network<EdgeT>::union_with(const network<EdgeT>& other) const {
    network<EdgeT> res(*this);
    for (auto& [v, es]: other._in_edges) {
      auto& res_edges = res._in_edges[v];
      std::size_t original_size = res_edges.size();
      if (original_size > 0) {
        auto mid = res_edges.insert(res_edges.end(), es.begin(), es.end());
        res.inplace_merge(
            res_edges.begin(),
            mid, res_edges.end(),
            [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
        res_edges.erase(std::ranges::unique(res_edges,
            [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); }));
      }
    }

    for (auto& [v, es]: other._out_edges) {
      auto& res_edges = res._out_edges[v];
      std::size_t original_size = res_edges.size();
      if (original_size > 0) {
        auto mid = res_edges.insert(res_edges.end(), es.begin(), es.end());
        res.inplace_merge(
            res_edges.begin(),
            mid, res_edges.end());
        res_edges.erase(std::ranges::unique(res_edges));
      }
    }

    auto mid_effect = res._edges_effect.insert(res._edges_effect.end(),
        other._edges_effect.begin(), other._edges_effect.end());
    std::inplace_merge(
        res._edges_effect.begin(),
        mid_effect,
        res._edges_effect.end(),
        [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
    res._edges_effect.erase(std::ranges::unique(res._edges_effect,
            [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); }));

    auto mid_cause = res._edges_cause.insert(res._edges_cause.end(),
        other._edges_cause.begin(), other._edges_cause.end());
    std::inplace_merge(
        res._edges_cause.begin(),
        mid_cause,
        res._edges_cause.end());
    res._edges_cause.erase(std::ranges::unique(res._edges_cause));

    return res;
  }
}  // namespace dag
