#ifndef INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_
#define INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_

#include <ranges>
#include <unordered_set>

#include <hll/hyperloglog.hpp>

#include "network_concepts.hpp"
#include "temporal_adjacency.hpp"
#include "intervals.hpp"
#include "type_traits.hpp"

namespace dag {
  template <typename T>
  concept network_temporal_component =
    temporal_edge<typename T::VertexType> &&
    std::constructible_from<T,
      typename T::AdjacencyType, std::size_t, std::size_t> &&
    requires(T a, const T::VertexType& v) {
      a.insert(v);
    } && requires(T a, const T& b) {
      a.merge(b);
    };  // NOLINT(readability/braces)

  template <typename T>
  concept exact_temporal_component =
    network_temporal_component<T> &&
    std::ranges::forward_range<T> &&
    std::ranges::sized_range<T> &&
    std::equality_comparable<T> &&
    requires(const T& a, const T::VertexType& v) {
      { a.contains(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  class temporal_component {
  public:
    using VertexType = EdgeT;
    using AdjacencyType = AdjT;
    using IteratorType =
      typename std::unordered_set<VertexType, hash<VertexType>>::const_iterator;

    explicit temporal_component(
        AdjT adj, std::size_t size_hint = 0, std::size_t seed = 0);
    temporal_component(
        std::initializer_list<EdgeT> verts, AdjT adj,
        std::size_t size_hint = 0, std::size_t seed = 0);

    template<std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
    explicit temporal_component(const Range& verts, AdjT adj,
        std::size_t size_hint = 0, std::size_t seed = 0);

    template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
    void insert(const Range& events);

    void insert(const EdgeT& e);

    void merge(const temporal_component<EdgeT, AdjT>& other);

    bool operator==(const temporal_component<EdgeT, AdjT>& c) const;
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
      dag::interval_set<typename EdgeT::TimeType>,
      hash<typename EdgeT::VertexType>> _ints;
  };
}  // namespace dag

#include "../../src/temporal_components.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_
