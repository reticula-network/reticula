#include "../include/reticula/networks.hpp"

namespace reticula {
  template <network_vertex VertT>
  component<VertT>::component(std::size_t size_hint) {
    if (size_hint > 0)
      _verts.reserve(size_hint);
  }

  template <network_vertex VertT>
  component<VertT>::component(
      std::initializer_list<VertT> verts,
      std::size_t size_hint) :
    component(std::vector(verts), size_hint) {}

  template <network_vertex VertT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, VertT>
  component<VertT>::component(
      Range&& verts,
      std::size_t size_hint) {
    if (size_hint == 0) {
      if constexpr (ranges::sized_range<Range>)
        _verts.reserve(ranges::size(verts));
    } else {
      _verts.reserve(size_hint);
    }

    for (auto&& v: verts)
      _verts.insert(v);
  }


  template <network_vertex VertT>
  void component<VertT>::insert(const VertT& v) {
    _verts.insert(v);
  }


  template <network_vertex VertT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, VertT>
  void component<VertT>::insert(Range&& verts) {
    for (auto&& v: verts)
      _verts.insert(v);
  }

  template <network_vertex VertT>
  void component<VertT>::merge(const component<VertT>& c) {
    _verts.insert(c.begin(), c.end());
  }

  template <network_vertex VertT>
  bool component<VertT>::contains(const VertT& v) const {
    return _verts.contains(v);
  }

  template <network_vertex VertT>
  bool component<VertT>::empty() const {
    return _verts.empty();
  }

  template <network_vertex VertT>
  std::size_t component<VertT>::size() const {
    return _verts.size();
  }

  template <network_vertex VertT>
  typename component<VertT>::IteratorType
  component<VertT>::begin() const {
    return _verts.begin();
  }

  template <network_vertex VertT>
  typename component<VertT>::IteratorType
  component<VertT>::end() const {
    return _verts.end();
  }

  template <network_vertex VertT>
  component_sketch<VertT>::component_sketch(std::size_t seed) :
    _verts(true, seed) {}

  template <network_vertex VertT>
  component_sketch<VertT>::component_sketch(
      std::initializer_list<VertT> verts,
      std::size_t seed) :
    component_sketch(std::vector(verts), seed) {}

  template <network_vertex VertT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, VertT>
  component_sketch<VertT>::component_sketch(
      Range&& verts, std::size_t seed) :
    _verts(true, seed) {
    for (auto&& v: verts)
      _verts.insert(v);
  }

  template <network_vertex VertT>
  void component_sketch<VertT>::insert(const VertT& v) {
    _verts.insert(v);
  }

  template <network_vertex VertT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, VertT>
  void component_sketch<VertT>::insert(Range&& verts) {
    for (auto& v: verts)
      _verts.insert(v);
  }

  template <network_vertex VertT>
  void component_sketch<VertT>::merge(const component_sketch<VertT>& e) {
    _verts.merge(e._verts);
  }

  template <network_vertex VertT>
  double component_sketch<VertT>::size_estimate() const {
    return _verts.estimate();
  }

  template <network_vertex VertT>
  component_size<VertT>::component_size(const component<VertT>& c) :
    _verts(c.size()) {}

  template <network_vertex VertT>
  std::size_t component_size<VertT>::size() const {
    return _verts;
  }

  template <network_vertex VertT>
  component_size_estimate<VertT>::component_size_estimate(
      const component_sketch<VertT>& c) :
    _verts(c.size_estimate()) {}

  template <network_vertex VertT>
  double component_size_estimate<VertT>::size_estimate() const {
    return _verts;
  }
}  // namespace reticula
