namespace dag {

  template <typename EdgeT>
  void network<EdgeT>::add_edges(const std::vector<EdgeT> es) {
    for (const auto &e: es) add_edge(e);
  }

  template <typename EdgeT>
  std::ostream &operator<<(std::ostream &os,
      network<EdgeT> const &n) {
    os << "{";
    std::string separator;
    for (const auto &e: n.edges()) {
      os << separator << e;
      separator = ", ";
    }
    os << "}";
    return os;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType, size_t>
  network<EdgeT>::in_degree() const {
    auto edges = in_edges();
    std::unordered_map<typename network<EdgeT>::VertexType, size_t> degrees;
    for (const auto &p: edges)
      degrees[p.first] = p.second.size();
    return degrees;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType, size_t>
  network<EdgeT>::out_degree() const {
    auto edges = out_edges();
    std::unordered_map<typename network<EdgeT>::VertexType, size_t> degrees;
    for (const auto &p: edges)
      degrees[p.first] = p.second.size();
    return degrees;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType, size_t>
    network<EdgeT>::degree() const {
    auto edges = incident_edges();
    std::unordered_map<typename network<EdgeT>::VertexType, size_t> degrees;
    for (const auto &p: edges)
      degrees[p.first] = p.second.size();
    return degrees;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType,
    std::unordered_set<EdgeT>>
  network<EdgeT>::in_edges() const {
    std::unordered_map<typename network<EdgeT>::VertexType,
      std::unordered_set<EdgeT>> ins;

    for (const auto &v: vertices())
      ins[v] = std::unordered_set<EdgeT>();

    for (const auto &e: edge_list)
      ins[e.head_vert()].insert(e);
    return ins;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType,
    std::unordered_set<EdgeT>>
  network<EdgeT>::out_edges() const {
    std::unordered_map<network<EdgeT>::VertexType,
      std::unordered_set<EdgeT>> outs;

    for (const auto &v: vertices())
      outs[v] = std::unordered_set<EdgeT>();

    for (const auto &e: edge_list)
      outs[e.tail_vert()].insert(e);
    return outs;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType,
    std::unordered_set<EdgeT>>
  network<EdgeT>::incident_edges() const {
    std::unordered_map<network<EdgeT>::VertexType,
      std::unordered_set<EdgeT>> incs;
    for (const auto &e: edge_list) {
      incs[e.v1].insert(e);
      incs[e.v2].insert(e);
    }
    return incs;
  }


  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType,
    std::unordered_set<typename network<EdgeT>::VertexType>>
  network<EdgeT>::predecessors() const {
    std::unordered_map<typename network<EdgeT>::VertexType,
      std::unordered_set<typename network<EdgeT>::VertexType>> res;
    for (const auto &e: edge_list)
      res[e.head_vert()].insert(e.tail_vert());
    return res;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType,
    std::unordered_set<typename network<EdgeT>::VertexType>>
  network<EdgeT>::successors() const {
    std::unordered_map<typename network<EdgeT>::VertexType,
      std::unordered_set<typename network<EdgeT>::VertexType>> res;
    for (const auto &e: edge_list)
      res[e.tail_vert()].insert(e.head_vert());
    return res;
  }

  template <typename EdgeT>
  std::unordered_map<typename network<EdgeT>::VertexType,
    std::unordered_set<typename network<EdgeT>::VertexType>>
  network<EdgeT>::neighbours() const {
    std::unordered_map<typename network<EdgeT>::VertexType,
      std::unordered_set<typename network<EdgeT>::VertexType>> res;
    for (const auto &e: edge_list) {
      res[e.v1].insert(e.v2);
      res[e.v2].insert(e.v1);
    }
    return res;
  }

  template <typename EdgeT>
  std::unordered_set<EdgeT>
  network<EdgeT>::in_edges(network<EdgeT>::VertexType vert) const {
    std::unordered_set<EdgeT> ins;
    for (const auto &e: edge_list)
      if (e.is_in_incident(vert)) ins.insert(e);
    return ins;
  }


  template <typename EdgeT>
  std::unordered_set<EdgeT>
  network<EdgeT>::out_edges(network<EdgeT>::VertexType vert) const {
    std::unordered_set<EdgeT> outs;
    for (const auto &e: edge_list)
      if (e.is_out_incident(vert)) outs.insert(e);
    return outs;
  }



  template <typename EdgeT>
  std::unordered_set<EdgeT>
  network<EdgeT>::incident_edges(network<EdgeT>::VertexType vert) const {
    std::unordered_set<EdgeT> incs;
    for (const auto &e: edge_list)
      if (e.is_incident(vert)) incs.insert(e);
    return incs;
  }


  template <typename EdgeT>
  std::unordered_set<typename network<EdgeT>::VertexType>
  network<EdgeT>::vertices() const {
    std::unordered_set<network<EdgeT>::VertexType> verts;
    for (const auto &e: edge_list) {
      verts.insert(e.v1);
      verts.insert(e.v2);
    }
    return verts;
  }
}
