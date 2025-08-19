#include <reticula/temporal_clusters.hpp>

#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace reticula {
template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::volume() const -> std::size_t {
  return ints_.size();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::size() const -> std::size_t {
  return events_.size();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::mass() const -> double {
  double mass = 0.0;
  for (const auto& [v, is] : ints_) {
    auto c = is.cover();
    if (std::isinf(c))
      return std::numeric_limits<double>::infinity();
    mass += c;
  }
  return mass;
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::lifetime() const -> window {
  return lifetime_;
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::intervals(VertexType v) const -> interval_set {
  auto it = ints_.find(v);
  if (it == ints_.end())
    return interval_set{};
  return it->second;
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::end() const -> IteratorType {
  return events_.cend();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::begin() const -> IteratorType {
  return events_.cbegin();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::empty() const -> bool {
  return events_.empty();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::covers(VertexType v, double t) const -> bool {
  auto it = ints_.find(v);
  if (it == ints_.end())
    return false;
  return it->second.covers(t);
}

template <temporal_network_edge EdgeT>
auto temporal_cluster<EdgeT>::contains(const EdgeT& e) const -> bool {
  return events_.contains(e);
}

template <temporal_network_edge EdgeT>
void temporal_cluster<EdgeT>::merge(const temporal_cluster<EdgeType>& other) {
  events_.insert(other.events_.begin(), other.events_.end());

  for (const auto& [v, is] : other.ints_)
    ints_[v].merge(is);

  lifetime_ = lifetime_.union_with(other.lifetime_);
}

template <temporal_network_edge EdgeT>
temporal_cluster<EdgeT>::temporal_cluster(std::size_t size_hint)
    : lifetime_{.start = std::numeric_limits<double>::infinity(), .end = -std::numeric_limits<double>::infinity()},
      events_{}, ints_{} {
  events_.reserve(size_hint);
}

template <temporal_network_edge EdgeT>
auto temporal_cluster_sketch<EdgeT>::size_estimate() const -> double {
  return events_.estimate();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster_sketch<EdgeT>::volume_estimate() const -> double {
  return ints_.cover_estimate();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster_sketch<EdgeT>::mass_estimate() const -> double {
  return events_.estimate() * verts_.estimate();
}

template <temporal_network_edge EdgeT>
auto temporal_cluster_sketch<EdgeT>::lifetime() const -> window {
  return lifetime_;
}

template <temporal_network_edge EdgeT>
void temporal_cluster_sketch<EdgeT>::merge(
  const temporal_cluster_sketch<EdgeType>& other) {
  events_.merge(other.events_);
  verts_.merge(other.verts_);
  ints_.merge(other.ints_);

  lifetime_ = lifetime_.union_with(other.lifetime_);
}

template <temporal_network_edge EdgeT>
temporal_cluster_sketch<EdgeT>::temporal_cluster_sketch(
  double resolution, std::size_t seed)
    : lifetime_{}, events_{true, seed}, verts_{true, seed},
      ints_{resolution, seed} {}

template <temporal_network_edge EdgeT>
temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<EdgeT>& cluster)
    : lifetime_(cluster.lifetime()), mass_(cluster.mass()),
      volume_(cluster.volume()), size_(cluster.size()) {}

auto temporal_cluster_size::lifetime() const -> window { return lifetime_; }

auto temporal_cluster_size::mass() const -> double { return mass_; }

auto temporal_cluster_size::volume() const -> std::size_t { return volume_; }

auto temporal_cluster_size::size() const -> std::size_t { return size_; }

template <temporal_network_edge EdgeT>
temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<EdgeT>& cluster)
    : lifetime_(cluster.lifetime()), mass_(cluster.mass_estimate()),
      volume_(cluster.volume_estimate()), size_(cluster.size_estimate()) {}

auto temporal_cluster_size_estimate::lifetime() const -> window {
  return lifetime_;
}

auto temporal_cluster_size_estimate::mass_estimate() const -> double {
  return mass_;
}

auto temporal_cluster_size_estimate::volume_estimate() const -> double {
  return volume_;
}

auto temporal_cluster_size_estimate::size_estimate() const -> double {
  return size_;
}

// explicitly instantiate the template for temporal_cluster
template class temporal_cluster<undirected_temporal_edge>;
template class temporal_cluster<directed_temporal_edge>;
template class temporal_cluster<directed_delayed_temporal_edge>;
template class temporal_cluster<undirected_temporal_hyperedge>;
template class temporal_cluster<directed_temporal_hyperedge>;
template class temporal_cluster<directed_delayed_temporal_hyperedge>;

template class temporal_cluster_sketch<undirected_temporal_edge>;
template class temporal_cluster_sketch<directed_temporal_edge>;
template class temporal_cluster_sketch<directed_delayed_temporal_edge>;
template class temporal_cluster_sketch<undirected_temporal_hyperedge>;
template class temporal_cluster_sketch<directed_temporal_hyperedge>;
template class temporal_cluster_sketch<directed_delayed_temporal_hyperedge>;

template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<undirected_temporal_edge>&);
template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_temporal_edge>&);
template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_delayed_temporal_edge>&);
template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<undirected_temporal_hyperedge>&);
template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_temporal_hyperedge>&);
template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_delayed_temporal_hyperedge>&);

template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<undirected_temporal_edge>&);
template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_temporal_edge>&);
template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_delayed_temporal_edge>&);
template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<undirected_temporal_hyperedge>&);
template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_temporal_hyperedge>&);
template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_delayed_temporal_hyperedge>&);
} // namespace reticula
