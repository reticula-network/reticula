#ifndef INCLUDE_RETICULA_TEMPORAL_CLUSTERS_HPP_
#define INCLUDE_RETICULA_TEMPORAL_CLUSTERS_HPP_

#include <ranges>
#include <unordered_set>

#include <hll/hyperloglog.hpp>

#include "network_concepts.hpp"
#include "temporal_adjacency.hpp"
#include "intervals.hpp"
#include "type_traits.hpp"

namespace reticula {
  template <typename T>
  concept network_temporal_cluster =
    temporal_edge<typename T::VertexType> &&
    requires(T a, const T::VertexType& v) {
      a.insert(v);
    } && requires(T a, const T& b) {
      a.merge(b);
    };  // NOLINT(readability/braces)

  template <typename T>
  concept exact_temporal_cluster =
    network_temporal_cluster<T> &&
    std::ranges::forward_range<T> &&
    std::ranges::sized_range<T> &&
    std::equality_comparable<T> &&
    requires(const T& a, const T::VertexType& v) {
      { a.contains(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  class temporal_cluster {
  public:
    using VertexType = EdgeT;
    using AdjacencyType = AdjT;
    using IteratorType =
      typename std::unordered_set<VertexType, hash<VertexType>>::const_iterator;

    explicit temporal_cluster(
        AdjT adj, std::size_t size_hint = 0);
    temporal_cluster(
        std::initializer_list<EdgeT> verts,
        AdjT adj, std::size_t size_hint = 0);

    template<std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
    explicit temporal_cluster(Range&& verts,
        AdjT adj, std::size_t size_hint = 0);

    template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
    void insert(Range&& events);

    void insert(const EdgeT& e);

    void merge(const temporal_cluster<EdgeT, AdjT>& other);

    bool operator==(const temporal_cluster<EdgeT, AdjT>& c) const;
    std::size_t size() const;

    bool contains(const EdgeT& e) const;
    bool covers(typename EdgeT::VertexType v, typename EdgeT::TimeType t) const;
    bool empty() const;

    IteratorType begin() const;
    IteratorType end() const;

    const std::unordered_map<
      typename EdgeT::VertexType,
      interval_set<typename EdgeT::TimeType>,
      hash<typename EdgeT::VertexType>>&
    interval_sets() const;

    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
    lifetime() const;
    std::size_t volume() const;
    typename EdgeT::TimeType mass() const;

  private:
    AdjT _adj;
    std::unordered_set<EdgeT, hash<EdgeT>> _events;
    std::pair<
      typename EdgeT::TimeType,
      typename EdgeT::TimeType> _lifetime;
    std::unordered_map<
      typename EdgeT::VertexType,
      interval_set<typename EdgeT::TimeType>,
      hash<typename EdgeT::VertexType>> _ints;
  };


  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  class temporal_cluster_size {
  public:
    using VertexType = EdgeT;
    using AdjacencyType = AdjT;

    explicit temporal_cluster_size(const temporal_cluster<EdgeT, AdjT>& c);

    std::size_t size() const;
    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
    lifetime() const;
    std::size_t volume() const;
    typename EdgeT::TimeType mass() const;

  private:
    std::size_t _size;
    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType> _lifetime;
    typename EdgeT::TimeType _mass;
    std::size_t _volume;
  };

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  class temporal_cluster_sketch {
  public:
    using VertexType = EdgeT;
    using AdjacencyType = AdjT;
    using EventSketchType =
      hll::hyperloglog<
        EdgeT,
        13, 14>;
    using VertSketchType =
      hll::hyperloglog<
        typename EdgeT::VertexType,
        13, 14>;
    using TimeSketchType =
      hll::hyperloglog<
        std::pair<typename EdgeT::VertexType, typename EdgeT::TimeType>,
        13, 14>;

    explicit temporal_cluster_sketch(
        AdjT adj,
        EdgeT::TimeType temporal_resolution = 1, std::size_t seed = 0);
    temporal_cluster_sketch(
        std::initializer_list<EdgeT> verts, AdjT adj,
        EdgeT::TimeType temporal_resolution = 1, std::size_t seed = 0);

    template<std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
    explicit temporal_cluster_sketch(Range&& verts, AdjT adj,
        EdgeT::TimeType temporal_resolution = 1, std::size_t seed = 0);

    template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
    void insert(Range&& events);

    void insert(const EdgeT& e);

    void merge(const temporal_cluster_sketch<EdgeT, AdjT>& other);

    double size_estimate() const;
    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
    lifetime() const;
    double volume_estimate() const;
    double mass_estimate() const;
    EdgeT::TimeType temporal_resolution() const;

  private:
    void insert_time_range(
        typename EdgeT::VertexType v,
        typename EdgeT::TimeType start,
        typename EdgeT::TimeType end);

    EdgeT::TimeType _dt;
    AdjT _adj;
    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType> _lifetime;
    EventSketchType _events;
    VertSketchType _verts;
    TimeSketchType _times;
  };

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  class temporal_cluster_size_estimate {
  public:
    using VertexType = EdgeT;
    using AdjacencyType = AdjT;

    explicit temporal_cluster_size_estimate(
        const temporal_cluster_sketch<EdgeT, AdjT>& c);

    double size_estimate() const;
    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
    lifetime() const;
    double volume_estimate() const;
    double mass_estimate() const;
    EdgeT::TimeType temporal_resolution() const;

  private:
    EdgeT::TimeType _dt;
    double _size_estimate;
    std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
    _lifetime;
    double _volume_estimate;
    double _mass_estimate;
  };
}  // namespace reticula

#include "../../src/temporal_clusters.tpp"

#endif  // INCLUDE_RETICULA_TEMPORAL_CLUSTERS_HPP_
