namespace dag {
  template <network_edge EdgeT>
  component<EdgeT>::component(
      std::size_t size_hint, std::size_t /* seed */) {
    if (size_hint > 0)
      _verts.reserve(size_hint);
  }


  template <network_edge EdgeT>
  void component<EdgeT>::insert(const EdgeT& e) {
    for (auto& v: e.incident_verts())
      _verts.insert(v);
  }

  template <network_edge EdgeT>
  void component<EdgeT>::insert(const typename EdgeT::VertexType& v) {
    _verts.insert(v);
  }

  template <network_edge EdgeT>
  void component<EdgeT>::merge(const component<EdgeT>& e) {
    _verts.insert(e.begin(), e.end());
  }

  template <network_edge EdgeT>
  bool component<EdgeT>::contains(const typename EdgeT::VertexType& v) const {
    return _verts.contains(v);
  }

  template <network_edge EdgeT>
  bool component<EdgeT>::operator==(const component<EdgeT>& other) const {
    return _verts == other._verts;
  }

  template <network_edge EdgeT>
  std::size_t component<EdgeT>::size() const {
    return _verts.size();
  }

  template <network_edge EdgeT>
  std::unordered_set<typename EdgeT::VertexType>::const_iterator
  component<EdgeT>::begin() const {
    return _verts.begin();
  }

  template <network_edge EdgeT>
  std::unordered_set<typename EdgeT::VertexType>::const_iterator
  component<EdgeT>::end() const {
    return _verts.end();
  }


  template <network_edge EdgeT>
  component_sketch<EdgeT>::component_sketch(
      std::size_t /* size_hint */, std::size_t seed) {
    // create dense HLL sketches, we are making the assumption that the
    // component size is not too small
    _verts(true, seed);
    _edges(true, seed);
  }

  template <network_edge EdgeT>
  void component_sketch<EdgeT>::insert(const EdgeT& e) {
    _edges.insert(e);
    for (auto& v: e.incident_verts())
      _verts.insert(v);
  }

  template <network_edge EdgeT>
  void component_sketch<EdgeT>::insert(const typename EdgeT::VertexType& v) {
    _verts.insert(v);
  }

  template <network_edge EdgeT>
  void component_sketch<EdgeT>::merge(const component_sketch<EdgeT>& e) {
    _verts.merge(e._verts);
    _edges.merge(e._edges);
  }


  template <network_edge EdgeT>
  double component_sketch<EdgeT>::size_estimate() const {
    return _verts.estimate();
  }

  template <network_edge EdgeT>
  double component_sketch<EdgeT>::edge_count_estimate() const {
    return _edges.estimate();
  }
}  // namespace dag
