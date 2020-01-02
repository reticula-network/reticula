#include <set>

namespace dag {
  template <typename EdgeT>
  network<EdgeT>::network(const std::vector<EdgeT>& edges)
  : _edges_cause{edges} {
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

  template <typename EdgeT>
  size_t network<EdgeT>::in_degree(
      const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return out_degree(v);

    return in_edges(v).size();
  }

  template <typename EdgeT>
  size_t network<EdgeT>::out_degree(
      const typename EdgeT::VertexType& v) const {
    return out_edges(v).size();
  }

  template <typename EdgeT>
  size_t network<EdgeT>::degree(
      const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return out_degree(v);

    return incident_edges(v).size();
  }

  template <typename EdgeT>
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

  template <typename EdgeT>
  const std::unordered_map<typename EdgeT::VertexType, std::vector<EdgeT>>&
  network<EdgeT>::in_edges() const {
    if (instantaneous_undirected)
      return _out_edges;
    return _in_edges;
  }

  template <typename EdgeT>
  std::vector<EdgeT>
  network<EdgeT>::out_edges(
      const typename EdgeT::VertexType& v) const {
    auto p = _out_edges.find(v);
    if (p == _out_edges.end())
      return std::vector<EdgeT>();
    else
      return p->second;
  }

  template <typename EdgeT>
  const std::unordered_map<typename EdgeT::VertexType, std::vector<EdgeT>>&
  network<EdgeT>::out_edges() const {
    return _out_edges;
  }

  template <typename EdgeT>
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



  template <typename EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::predecessors(const typename EdgeT::VertexType& v) const {
    if (instantaneous_undirected)
      return successors(v);

    std::unordered_set<typename EdgeT::VertexType> preds;
    auto p = _in_edges.find(v);
    if (p != _in_edges.end()) {
      preds.reserve(p->second.size());
      for (auto&& e: p->second)
        for (auto&& u: e.mutator_verts())
          if (u != v) preds.insert(u);
    }
    return std::vector<typename EdgeT::VertexType>(preds.begin(), preds.end());
  }

  template <typename EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::successors(const typename EdgeT::VertexType& v) const {
    std::unordered_set<typename EdgeT::VertexType> succ;
    auto p = _out_edges.find(v);
    if (p != _out_edges.end()) {
      succ.reserve(p->second.size());
      for (auto&& e: p->second)
        for (auto&& u: e.mutated_verts())
          if (u != v) succ.insert(u);
    }
    return std::vector<typename EdgeT::VertexType>(succ.begin(), succ.end());
  }

  template <typename EdgeT>
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

  template <typename EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges() const {
    return _edges_cause;
  }

  template <typename EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges_cause() const {
    return _edges_cause;
  }

  template <typename EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges_effect() const {
    if (instantaneous_undirected)
      return _edges_cause;

    return _edges_effect;
  }

  template <typename EdgeT>
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
}  // namespace dag
