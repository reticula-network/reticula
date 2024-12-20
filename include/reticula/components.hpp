#ifndef INCLUDE_RETICULA_COMPONENTS_HPP_
#define INCLUDE_RETICULA_COMPONENTS_HPP_

#include <unordered_set>

#include <hll/hyperloglog.hpp>

#include "ranges.hpp"
#include "network_concepts.hpp"
#include "type_traits.hpp"

namespace reticula {
  template <typename T>
  concept network_component =
    network_vertex<typename T::VertexType> &&
    requires(T a, const typename T::VertexType& v) {
      a.insert(v);
    } && requires(T a, const T& b) {
      a.merge(b);
    };  // NOLINT(readability/braces)

  template <typename T>
  concept exact_component =
    network_component<T> &&
    ranges::forward_range<T> &&
    ranges::sized_range<T> &&
    std::equality_comparable<T> &&
    requires(const T& a, const typename T::VertexType& v) {
      { a.contains(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <network_vertex VertT>
  class component {
  public:
    using VertexType = VertT;
    using IteratorType =
      typename std::unordered_set<VertexType, hash<VertexType>>::const_iterator;

    explicit component(std::size_t size_hint = 0);
    component(
        std::initializer_list<VertexType> verts,
        std::size_t size_hint = 0);

    template<ranges::input_range Range>
    requires std::convertible_to<ranges::range_value_t<Range>, VertT>
    explicit component(
        Range&& verts,
        std::size_t size_hint = 0);


    template <ranges::input_range Range>
    requires std::convertible_to<ranges::range_value_t<Range>, VertT>
    void insert(Range&& verts);

    void insert(const VertexType& v);

    void merge(const component<VertT>& other);

    bool operator==(const component<VertT>&) const = default;
    [[nodiscard]] std::size_t size() const;

    bool contains(const VertexType& v) const;
    [[nodiscard]] bool empty() const;

    IteratorType begin() const;
    IteratorType end() const;

  private:
    std::unordered_set<VertexType, hash<VertexType>> _verts;
  };

  template <network_vertex VertT>
  class component_size {
  public:
    using VertexType = VertT;

    explicit component_size(const component<VertT>& c);

    [[nodiscard]] std::size_t size() const;

  private:
    std::size_t _verts;
  };



  template <network_vertex VertT>
  class component_sketch {
  public:
    using VertexType = VertT;
    using SketchType = hll::hyperloglog<VertexType, 13, 14>;

    explicit component_sketch(std::size_t seed = 0);
    component_sketch(
        std::initializer_list<VertexType> verts,
        std::size_t seed = 0);

    template<ranges::input_range Range>
    requires std::convertible_to<ranges::range_value_t<Range>, VertT>
    explicit component_sketch(
        Range&& verts,
        std::size_t seed = 0);

    template <ranges::input_range Range>
    requires std::convertible_to<ranges::range_value_t<Range>, VertT>
    void insert(Range&& verts);

    void insert(const VertexType& v);


    void merge(const component_sketch<VertT>& other);

    [[nodiscard]] double size_estimate() const;

  private:
    SketchType _verts;
  };

  template <network_vertex VertT>
  class component_size_estimate {
  public:
    using VertexType = VertT;

    explicit component_size_estimate(const component_sketch<VertT>& c);

    [[nodiscard]] double size_estimate() const;
  private:
    double _verts;
  };
}  // namespace reticula


#include "../../src/components.tpp"

#endif  // INCLUDE_RETICULA_COMPONENTS_HPP_
