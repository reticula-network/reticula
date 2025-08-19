#pragma once

#include <concepts>
#include <initializer_list>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include <reticula/algorithms/properties.hpp>
#include <reticula/concepts.hpp>
#include <reticula/interval_sets.hpp>
#include <reticula/temporal_adjacency.hpp>

namespace reticula {

template <typename T, typename AdjT>
concept cluster_like =
  temporal_network_edge<typename T::EdgeType> &&
  adjacency::adjacency<AdjT, typename T::EdgeType> &&
  requires(T t, const typename T::EdgeType& e, const AdjT& adj) {
    t.insert(e, adj);
  };

template <temporal_network_edge EdgeT>
class temporal_cluster {
public:
  using EdgeType = EdgeT;
  using IteratorType = typename std::unordered_set<EdgeT>::const_iterator;

  temporal_cluster(std::size_t size_hint = 0);

  template <adjacency::adjacency<EdgeT> AdjT>
  void insert(const EdgeT& e, const AdjT& adj);

  template <adjacency::adjacency<EdgeT> AdjT, std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, EdgeT>
  void insert(R&& e, const AdjT& adj);

  template <adjacency::adjacency<EdgeT> AdjT>
  void insert(const std::initializer_list<EdgeT>& e, const AdjT& adj);

  void merge(const temporal_cluster<EdgeT>& other);

  [[nodiscard]] auto contains(const EdgeT& e) const -> bool;

  [[nodiscard]] auto covers(VertexType v, double t) const -> bool;

  [[nodiscard]] auto empty() const -> bool;

  [[nodiscard]] auto begin() const -> IteratorType;

  [[nodiscard]] auto end() const -> IteratorType;

  [[nodiscard]] auto intervals(VertexType v) const -> interval_set;

  [[nodiscard]] auto lifetime() const -> window;

  [[nodiscard]] auto mass() const -> double;

  [[nodiscard]] auto size() const -> std::size_t;

  [[nodiscard]] auto volume() const -> std::size_t;

private:
  window lifetime_;
  std::unordered_set<EdgeT> events_;
  std::unordered_map<VertexType, interval_set> ints_;
};

template <temporal_network_edge EdgeT>
class temporal_cluster_sketch {
public:
  using EdgeType = EdgeT;

  temporal_cluster_sketch(double resolution, std::size_t seed);

  template <adjacency::adjacency<EdgeT> AdjT>
  void insert(const EdgeT& e, const AdjT& adj);

  template <adjacency::adjacency<EdgeT> AdjT, std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, EdgeT>
  void insert(R&& e, const AdjT& adj);

  template <adjacency::adjacency<EdgeT> AdjT>
  void insert(const std::initializer_list<EdgeT>& e, const AdjT& adj);

  void merge(const temporal_cluster_sketch<EdgeT>& other);

  [[nodiscard]] auto lifetime() const -> window;

  [[nodiscard]] auto mass_estimate() const -> double;

  [[nodiscard]] auto volume_estimate() const -> double;

  [[nodiscard]] auto size_estimate() const -> double;

private:
  window lifetime_;
  hll::hyperloglog<EdgeT, 13, 14> events_;
  hll::hyperloglog<VertexType, 13, 14> verts_;
  interval_set_cluster_sketch ints_;
};

class temporal_cluster_size {
public:
  template <temporal_network_edge EdgeT>
  temporal_cluster_size(const temporal_cluster<EdgeT>& cluster);

  [[nodiscard]] auto lifetime() const -> window;
  [[nodiscard]] auto mass() const -> double;
  [[nodiscard]] auto volume() const -> std::size_t;
  [[nodiscard]] auto size() const -> std::size_t;

private:
  window lifetime_;
  double mass_;
  std::size_t volume_;
  std::size_t size_;
};

class temporal_cluster_size_estimate {
public:
  template <temporal_network_edge EdgeT>
  temporal_cluster_size_estimate(const temporal_cluster_sketch<EdgeT>& cluster);

  [[nodiscard]] auto lifetime() const -> window;
  [[nodiscard]] auto mass_estimate() const -> double;
  [[nodiscard]] auto volume_estimate() const -> double;
  [[nodiscard]] auto size_estimate() const -> double;

private:
  window lifetime_;
  double mass_;
  double volume_;
  double size_;
};
} // namespace reticula

namespace reticula {
template <temporal_network_edge EdgeT>
template <adjacency::adjacency<EdgeT> AdjT>
void temporal_cluster<EdgeT>::insert(const EdgeT& e, const AdjT& adj) {
  events_.insert(e);
  double effect = e.effect_time();
  double window_end = effect;
  for (auto& v : e.mutated_verts()) {
    double linger = adj.linger(e, v);
    ints_[v].insert(effect, effect + linger);
    window_end = std::max(window_end, effect + linger);
  }

  lifetime_ = lifetime_.union_with(window{.start = effect, .end = window_end});
}

template <temporal_network_edge EdgeT>
template <adjacency::adjacency<EdgeT> AdjT, std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, EdgeT>
void temporal_cluster<EdgeT>::insert(R&& e, const AdjT& adj) {
  if constexpr (std::ranges::sized_range<R>)
    events_.reserve(events_.size() + std::ranges::size(e));

  for (const auto& edge : e)
    insert(edge, adj);
}

template <temporal_network_edge EdgeT>
template <adjacency::adjacency<EdgeT> AdjT>
void temporal_cluster<EdgeT>::insert(
  const std::initializer_list<EdgeT>& e, const AdjT& adj) {
  insert(std::ranges::views::all(e), adj);
}

template <temporal_network_edge EdgeT>
template <adjacency::adjacency<EdgeT> AdjT>
void temporal_cluster_sketch<EdgeT>::insert(
  const EdgeT& e, const AdjT& adj) {
  events_.insert(e);

  double effect = e.effect_time();
  double window_end = effect;
  for (const auto& v : e.mutated_verts()) {
    verts_.insert(v);
    double linger = adj.linger(e, v);
    ints_.insert(v, effect, effect + linger);
    window_end = std::max(window_end, effect + linger);
  }

  lifetime_ = lifetime_.union_with(window{.start = effect, .end = window_end});
}

template <temporal_network_edge EdgeT>
template <adjacency::adjacency<EdgeT> AdjT, std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, EdgeT>
void temporal_cluster_sketch<EdgeT>::insert(R&& e, const AdjT& adj) {
  for (const auto& edge : e)
    insert(edge, adj);
}

template <temporal_network_edge EdgeT>
template <adjacency::adjacency<EdgeT> AdjT>
void temporal_cluster_sketch<EdgeT>::insert(
  const std::initializer_list<EdgeT>& e, const AdjT& adj) {
  insert(std::ranges::views::all(e), adj);
}

extern template class temporal_cluster<undirected_temporal_edge>;
extern template class temporal_cluster<directed_temporal_edge>;
extern template class temporal_cluster<directed_delayed_temporal_edge>;
extern template class temporal_cluster<undirected_temporal_hyperedge>;
extern template class temporal_cluster<directed_temporal_hyperedge>;
extern template class temporal_cluster<directed_delayed_temporal_hyperedge>;

extern template class temporal_cluster_sketch<undirected_temporal_edge>;
extern template class temporal_cluster_sketch<directed_temporal_edge>;
extern template class temporal_cluster_sketch<directed_delayed_temporal_edge>;
extern template class temporal_cluster_sketch<undirected_temporal_hyperedge>;
extern template class temporal_cluster_sketch<directed_temporal_hyperedge>;
extern template class temporal_cluster_sketch<
  directed_delayed_temporal_hyperedge>;

extern template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<undirected_temporal_edge>&);
extern template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_temporal_edge>&);
extern template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_delayed_temporal_edge>&);
extern template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<undirected_temporal_hyperedge>&);
extern template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_temporal_hyperedge>&);
extern template temporal_cluster_size_estimate::temporal_cluster_size_estimate(
  const temporal_cluster_sketch<directed_delayed_temporal_hyperedge>&);

extern template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<undirected_temporal_edge>&);
extern template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_temporal_edge>&);
extern template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_delayed_temporal_edge>&);
extern template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<undirected_temporal_hyperedge>&);
extern template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_temporal_hyperedge>&);
extern template temporal_cluster_size::temporal_cluster_size(
  const temporal_cluster<directed_delayed_temporal_hyperedge>&);
} // namespace reticula
