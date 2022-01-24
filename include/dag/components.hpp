#ifndef INCLUDE_DAG_COMPONENTS_HPP_
#define INCLUDE_DAG_COMPONENTS_HPP_

#include <ranges>

#include <hll/hyperloglog.hpp>

#include "network_concepts.hpp"
#include "type_traits.hpp"

namespace dag {
  template <typename T>
  concept network_component =
    network_edge<typename T::EdgeType> &&
    std::constructible_from<T, std::size_t, std::size_t> &&
    requires(T a, const T::EdgeType& e) {
      a.insert(e);
    } && requires(T a, const T::EdgeType::VertexType& v) {
      a.insert(v);
    } && requires(T a, const T& b) {
      a.merge(b);
    };  // NOLINT(readability/braces)

  template <typename T>
  concept exact_component =
    network_component<T> &&
    std::ranges::forward_range<T> &&
    std::ranges::viewable_range<T> &&
    std::ranges::sized_range<T> &&
    std::equality_comparable<T> &&
    requires(const T& a, const T::EdgeType::VertexType& v) {
      { a.contains(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <network_edge EdgeT>
  class component : public std::ranges::view_interface<component<EdgeT>> {
  public:
    using EdgeType = EdgeT;
    using VertexType = typename EdgeType::VertexType;
    using IteratorType =
      typename std::unordered_set<VertexType>::const_iterator;

    component(std::size_t size_hint = 0, std::size_t seed = 0);
    void insert(const EdgeType& e);
    void insert(const VertexType& v);
    void merge(const component<EdgeT>& other);

    bool operator==(const component<EdgeT> &other) const;
    std::size_t size() const;
    bool contains(const VertexType& v) const;

    IteratorType begin() const;
    IteratorType end() const;
  private:
    std::unordered_set<VertexType> _verts;
  };

  template <network_edge EdgeT>
  class component_sketch {
  public:
    using EdgeType = EdgeT;
    using VertexType = typename EdgeType::VertexType;
    using SketchType = hll::hyperloglog<VertexType, 13, 14>;

    component_sketch(std::size_t size_hint = 0, std::size_t seed = 0);
    void insert(const EdgeType& e);
    void insert(const VertexType& v);
    void merge(const component_sketch<EdgeT>& other);

    double size_estimate() const;
    double edge_count_estimate() const;
  private:
    SketchType _verts;
    SketchType _edges;
  };
}  // namespace dag


#include "../../src/components.tpp"

#endif  // INCLUDE_DAG_COMPONENTS_HPP_
