#ifndef INCLUDE_RETICULA_COMPONENTS_HPP_
#define INCLUDE_RETICULA_COMPONENTS_HPP_

#include <unordered_set>

#include <hll/hyperloglog.hpp>

#include "ranges.hpp"
#include "network_concepts.hpp"

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


// Implementation
#include "networks.hpp"

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


#endif  // INCLUDE_RETICULA_COMPONENTS_HPP_
