#include <limits>

namespace dag {
  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  temporal_component<EdgeT, AdjT>::temporal_component(
      AdjT adj, std::size_t size_hint, std::size_t /* seed */) :
    _adj(adj), _lifetime(
        std::numeric_limits<typename EdgeT::TimeType>::max(),
        std::numeric_limits<typename EdgeT::TimeType>::min()) {
    if (size_hint > 0)
      _events.reserve(size_hint);
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  temporal_component<EdgeT, AdjT>::temporal_component(
      std::initializer_list<EdgeT> events, AdjT adj,
      std::size_t size_hint, std::size_t seed) :
    temporal_component(std::vector(events), adj, size_hint, seed) {}

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
  temporal_component<EdgeT, AdjT>::temporal_component(
      const Range& events, AdjT adj,
      std::size_t size_hint, std::size_t /* seed */) :
    _adj(adj), _lifetime(
        std::numeric_limits<typename EdgeT::TimeType>::max(),
        std::numeric_limits<typename EdgeT::TimeType>::min()) {
    if (size_hint == 0) {
      if constexpr (std::ranges::sized_range<Range>)
        _events.reserve(std::ranges::size(events));
    } else {
      _events.reserve(size_hint);
    }

    for (auto& e: events)
      insert(e);
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  void temporal_component<EdgeT, AdjT>::insert(const EdgeT& e) {
    _events.insert(e);
    typename EdgeT::TimeType max =
      std::numeric_limits<typename EdgeT::TimeType>::max(),
      effect = e.effect_time();
    _lifetime.first = std::min(_lifetime.first, effect);
    for (auto& v: e.mutated_verts()) {
      typename EdgeT::TimeType linger = _adj.linger(e, v);
      if (max - effect <= linger) {
        _ints[v].insert(effect, max);
        _lifetime.second = max;
      } else {
        _ints[v].insert(effect, effect + linger);
        _lifetime.second = std::max(_lifetime.second, effect + linger);
      }
    }
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
  void temporal_component<EdgeT, AdjT>::insert(const Range& events) {
    for (auto& e: events)
      insert(e);
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  void temporal_component<EdgeT, AdjT>::merge(
      const temporal_component<EdgeT, AdjT>& c) {
    _events.insert(c.begin(), c.end());

    for (auto& [v, int_set]: c._ints)
      _ints[v].merge(int_set);

    _lifetime = std::make_pair(
        std::min(c._lifetime.first, _lifetime.first),
        std::max(c._lifetime.second, _lifetime.second));
  }


  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_component<EdgeT, AdjT>::operator==(
      const temporal_component<EdgeT, AdjT>& c) const {
    return _events == c._events &&  _ints == c._ints;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  std::size_t temporal_component<EdgeT, AdjT>::size() const {
    return _events.size();
  }


  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_component<EdgeT, AdjT>::contains(const EdgeT& e) const {
    return _events.contains(e);
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_component<EdgeT, AdjT>::covers(
      typename EdgeT::VertexType v, typename EdgeT::TimeType t) const {
    if (_ints.contains(v))
      return _ints.at(v).covers(t);
    else
      return false;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_component<EdgeT, AdjT>::empty() const {
    return _events.empty();
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  temporal_component<EdgeT, AdjT>::IteratorType
  temporal_component<EdgeT, AdjT>::begin() const {
    return _events.begin();
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  temporal_component<EdgeT, AdjT>::IteratorType
  temporal_component<EdgeT, AdjT>::end() const {
    return _events.end();
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  const std::unordered_map<
    typename EdgeT::VertexType,
    interval_set<typename EdgeT::TimeType>,
    hash<typename EdgeT::VertexType>>&
  temporal_component<EdgeT, AdjT>::interval_sets() const {
    return _ints;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  temporal_component<EdgeT, AdjT>::lifetime() const {
    return _lifetime;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  std::size_t temporal_component<EdgeT, AdjT>::volume() const {
    return _ints.size();
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  typename EdgeT::TimeType temporal_component<EdgeT, AdjT>::mass() const {
    typename EdgeT::TimeType total {};

    for (auto& [v, int_set]: _ints)
      total += int_set.cover();

    return total;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  temporal_component_size<EdgeT, AdjT>::temporal_component_size(
      const temporal_component<EdgeT, AdjT>& c) :
    _size(c.size()), _lifetime(c.lifetime()),
    _mass(c.mass()), _volume(c.volume()) {}

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  std::size_t temporal_component_size<EdgeT, AdjT>::size() const {
    return _size;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  temporal_component_size<EdgeT, AdjT>::lifetime() const {
    return _lifetime;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  std::size_t
  temporal_component_size<EdgeT, AdjT>::volume() const {
    return _volume;
  }

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  typename EdgeT::TimeType
  temporal_component_size<EdgeT, AdjT>::mass() const {
    return _mass;
  }
}  // namespace dag
