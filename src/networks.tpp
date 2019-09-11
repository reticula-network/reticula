namespace dag {
  template <typename EdgeT>
  network<EdgeT>::network(const std::vector<EdgeT>& edges) : _edges{edges} {
    std::sort(_edges.begin(), _edges.end());
    _edges.erase(std::unique(_edges.begin(), _edges.end()), _edges.end());
    _edges.shrink_to_fit();

    for (const auto& e: _edges) {
      for (auto&& v: e.mutator_verts())
        _out_edges[v].push_back(e);
      for (auto&& v: e.mutated_verts())
        _in_edges[v].push_back(e);
    }

    for (auto&& p: _in_edges) {
      std::sort(p.second.begin(), p.second.end(),
          [](const EdgeT& a, const EdgeT& b){ return effect_lt(a, b); });
      p.second.erase(std::unique(p.second.begin(), p.second.end()),
          p.second.end());
      p.second.shrink_to_fit();
    }

    for (auto&& p: _out_edges) {
      std::sort(p.second.begin(), p.second.end());
      p.second.erase(std::unique(p.second.begin(), p.second.end()),
          p.second.end());
      p.second.shrink_to_fit();
    }
  }

  template <typename EdgeT>
  size_t network<EdgeT>::in_degree(
      const typename EdgeT::VertexType& v) const {
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
    return incident_edges(v).size();
  }

  template <typename EdgeT>
  std::vector<EdgeT>
  network<EdgeT>::in_edges(
      const typename EdgeT::VertexType& v) const {
    auto p = _in_edges.find(v);
    if (p == _in_edges.end())
      return std::vector<EdgeT>();
    else
      return p->second;
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
  std::vector<EdgeT>
  network<EdgeT>::incident_edges(
      const typename EdgeT::VertexType& v) const {
    std::vector<EdgeT> inc(in_edges(v));
    std::vector<EdgeT> out(out_edges(v));
    inc.insert(inc.end(), out.begin(), out.end());
    std::sort(inc.begin(), inc.end());
    inc.erase(std::unique(inc.begin(), inc.end()), inc.end());
    return inc;
  }



  template <typename EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::predecessors(const typename EdgeT::VertexType& v) const {
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
    std::vector<typename EdgeT::VertexType> inc(predecessors(v));
    std::vector<typename EdgeT::VertexType> succ(successors(v));

    inc.insert(inc.end(), succ.begin(), succ.end());
    std::sort(inc.begin(), inc.end());
    inc.erase(std::unique(inc.begin(), inc.end()), inc.end());
    return inc;
  }

  template <typename EdgeT>
  const std::vector<EdgeT>&
  network<EdgeT>::edges() const {
    return _edges;
  }

  template <typename EdgeT>
  std::vector<typename EdgeT::VertexType>
  network<EdgeT>::vertices() const {
    std::unordered_set<network<EdgeT>::VertexType> verts;
    verts.reserve(_in_edges.size());
    for (const auto &p: _in_edges)
      verts.insert(p.first);
    for (const auto &p: _out_edges)
      verts.insert(p.first);
    return std::vector<typename EdgeT::VertexType>(verts.begin(), verts.end());
  }
}  // namespace dag
