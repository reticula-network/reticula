#include <limits>
#include <cmath>

namespace reticula {
  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>::temporal_cluster(
      AdjT adj, std::size_t size_hint) :
    _adj(adj), _lifetime(
        std::numeric_limits<typename EdgeT::TimeType>::max(),
        std::numeric_limits<typename EdgeT::TimeType>::min()) {
    if (size_hint > 0)
      _events.reserve(size_hint);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>::temporal_cluster(
      std::initializer_list<EdgeT> events, AdjT adj,
      std::size_t size_hint) :
    temporal_cluster(std::vector(events), adj, size_hint) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, EdgeT>
  temporal_cluster<EdgeT, AdjT>::temporal_cluster(
      Range&& events, AdjT adj, std::size_t size_hint) :
    _adj(adj), _lifetime(
        std::numeric_limits<typename EdgeT::TimeType>::max(),
        std::numeric_limits<typename EdgeT::TimeType>::min()) {
    if (size_hint == 0) {
      if constexpr (ranges::sized_range<Range>)
        _events.reserve(ranges::size(events));
    } else {
      _events.reserve(size_hint);
    }

    for (auto&& e: events)
      insert(e);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  void temporal_cluster<EdgeT, AdjT>::insert(const EdgeT& e) {
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

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, EdgeT>
  void temporal_cluster<EdgeT, AdjT>::insert(Range&& events) {
    for (auto&& e: events)
      insert(e);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  void temporal_cluster<EdgeT, AdjT>::merge(
      const temporal_cluster<EdgeT, AdjT>& c) {
    _events.insert(c.begin(), c.end());

    for (auto& [v, int_set]: c._ints)
      _ints[v].merge(int_set);

    _lifetime = std::make_pair(
        std::min(c._lifetime.first, _lifetime.first),
        std::max(c._lifetime.second, _lifetime.second));
  }


  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_cluster<EdgeT, AdjT>::operator==(
      const temporal_cluster<EdgeT, AdjT>& c) const {
    return _events == c._events &&  _ints == c._ints;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::size_t temporal_cluster<EdgeT, AdjT>::size() const {
    return _events.size();
  }


  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_cluster<EdgeT, AdjT>::contains(const EdgeT& e) const {
    return _events.contains(e);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_cluster<EdgeT, AdjT>::covers(
      typename EdgeT::VertexType v, typename EdgeT::TimeType t) const {
    if (_ints.contains(v))
      return _ints.at(v).covers(t);
    else
      return false;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool temporal_cluster<EdgeT, AdjT>::empty() const {
    return _events.empty();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>::IteratorType
  temporal_cluster<EdgeT, AdjT>::begin() const {
    return _events.begin();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>::IteratorType
  temporal_cluster<EdgeT, AdjT>::end() const {
    return _events.end();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  const std::unordered_map<
    typename EdgeT::VertexType,
    interval_set<typename EdgeT::TimeType>,
    hash<typename EdgeT::VertexType>>&
  temporal_cluster<EdgeT, AdjT>::interval_sets() const {
    return _ints;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  temporal_cluster<EdgeT, AdjT>::lifetime() const {
    return _lifetime;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::size_t temporal_cluster<EdgeT, AdjT>::volume() const {
    return _ints.size();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  typename EdgeT::TimeType temporal_cluster<EdgeT, AdjT>::mass() const {
    typename EdgeT::TimeType total {};

    for (auto& [v, int_set]: _ints)
      total += int_set.cover();

    return total;
  }


  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster_size<EdgeT, AdjT>::temporal_cluster_size(
      const temporal_cluster<EdgeT, AdjT>& c) :
    _size(c.size()), _lifetime(c.lifetime()),
    _mass(c.mass()), _volume(c.volume()) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::size_t temporal_cluster_size<EdgeT, AdjT>::size() const {
    return _size;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  temporal_cluster_size<EdgeT, AdjT>::lifetime() const {
    return _lifetime;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::size_t
  temporal_cluster_size<EdgeT, AdjT>::volume() const {
    return _volume;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  typename EdgeT::TimeType
  temporal_cluster_size<EdgeT, AdjT>::mass() const {
    return _mass;
  }


  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster_sketch<EdgeT, AdjT>::temporal_cluster_sketch(
      AdjT adj, EdgeT::TimeType temporal_resolution, std::size_t seed) :
      _dt(temporal_resolution), _adj(adj), _lifetime(
          std::numeric_limits<typename EdgeT::TimeType>::max(),
          std::numeric_limits<typename EdgeT::TimeType>::min()),
      _events(true, static_cast<std::uint64_t>(seed)),
      _verts(true, static_cast<std::uint64_t>(seed)),
      _times(true, static_cast<std::uint64_t>(seed)) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster_sketch<EdgeT, AdjT>::temporal_cluster_sketch(
      std::initializer_list<EdgeT> events, AdjT adj,
      EdgeT::TimeType temporal_resolution, std::size_t seed) :
    temporal_cluster_sketch(
        std::vector(events), adj, temporal_resolution, seed) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, EdgeT>
  temporal_cluster_sketch<EdgeT, AdjT>::temporal_cluster_sketch(
      Range&& events, AdjT adj,
      EdgeT::TimeType temporal_resolution, std::size_t seed) :
      _dt(temporal_resolution), _adj(adj), _lifetime(
          std::numeric_limits<typename EdgeT::TimeType>::max(),
          std::numeric_limits<typename EdgeT::TimeType>::min()),
      _events(true, static_cast<std::uint64_t>(seed)),
      _verts(true, static_cast<std::uint64_t>(seed)),
      _times(true, static_cast<std::uint64_t>(seed)) {
    for (auto&& e: events)
      insert(e);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  void temporal_cluster_sketch<EdgeT, AdjT>::insert(const EdgeT& e) {
    _events.insert(e);
    typename EdgeT::TimeType max =
      std::numeric_limits<typename EdgeT::TimeType>::max(),
      effect = e.effect_time();
    _lifetime.first = std::min(_lifetime.first, effect);
    for (auto& v: e.mutated_verts()) {
      _verts.insert(v);
      typename EdgeT::TimeType linger = _adj.linger(e, v);
      if (max - effect <= linger) {
        insert_time_range(v, effect, max);
        _lifetime.second = max;
      } else {
        insert_time_range(v, effect, effect + linger);
        _lifetime.second = std::max(_lifetime.second, effect + linger);
      }
    }
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  template <ranges::input_range Range>
  requires std::convertible_to<ranges::range_value_t<Range>, EdgeT>
  void temporal_cluster_sketch<EdgeT, AdjT>::insert(Range&& events) {
    for (auto&& e: events)
      insert(e);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  void temporal_cluster_sketch<EdgeT, AdjT>::merge(
      const temporal_cluster_sketch<EdgeT, AdjT>& c) {
    if (_dt != c._dt)
      throw std::invalid_argument("Cannot merge two temporal cluster sketchs "
          "with different temporal resolutions");

    _lifetime = std::make_pair(
        std::min(c._lifetime.first, _lifetime.first),
        std::max(c._lifetime.second, _lifetime.second));

    _events.merge(c._events);
    _verts.merge(c._verts);
    _times.merge(c._times);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  temporal_cluster_sketch<EdgeT, AdjT>::lifetime() const {
    return _lifetime;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  double temporal_cluster_sketch<EdgeT, AdjT>::size_estimate() const {
    return _events.estimate();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  double temporal_cluster_sketch<EdgeT, AdjT>::volume_estimate() const {
    return _verts.estimate();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  double temporal_cluster_sketch<EdgeT, AdjT>::mass_estimate() const {
    return _times.estimate()*static_cast<double>(_dt);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  void temporal_cluster_sketch<EdgeT, AdjT>::insert_time_range(
      typename EdgeT::VertexType v,
      typename EdgeT::TimeType start, typename EdgeT::TimeType end) {
    typename EdgeT::TimeType
      a = static_cast<EdgeT::TimeType>(std::floor(start/_dt)),
      b = static_cast<EdgeT::TimeType>(std::floor(end/_dt) + 1);
    for (typename EdgeT::TimeType s = a; s <= b; s++)
      if (s*_dt > start && s*_dt <= end)
        _times.insert({v, s});
  }



  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster_size_estimate<EdgeT, AdjT>::
  temporal_cluster_size_estimate(
      const temporal_cluster_sketch<EdgeT, AdjT>& c) :
    _size_estimate(c.size_estimate()),
    _lifetime(c.lifetime()),
    _volume_estimate(c.volume_estimate()),
    _mass_estimate(c.mass_estimate()) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  temporal_cluster_size_estimate<EdgeT, AdjT>::lifetime() const {
    return _lifetime;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  double
  temporal_cluster_size_estimate<EdgeT, AdjT>::size_estimate() const {
    return _size_estimate;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  double
  temporal_cluster_size_estimate<EdgeT, AdjT>::volume_estimate() const {
    return _volume_estimate;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  double
  temporal_cluster_size_estimate<EdgeT, AdjT>::mass_estimate() const {
    return _mass_estimate;
  }
}  // namespace reticula
