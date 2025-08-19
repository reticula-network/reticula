#pragma once

#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <unordered_set>

#include <hll/hyperloglog.hpp>

#include <reticula/concepts.hpp>

namespace reticula {

template <typename T>
concept component_like = requires(T t, VertexType v) { t.insert(v); } &&
                         requires(T t, const T& other) { t.merge(other); };

template <typename T>
concept exact_component_like =
  component_like<T> && std::ranges::forward_range<T> &&
  std::ranges::sized_range<T> && std::equality_comparable<T> &&
  requires(const T& t, const VertexType& v) {
    { t.contains(v) } -> std::convertible_to<bool>;
  };

class component {
public:
  using IteratorType = typename std::unordered_set<VertexType>::const_iterator;

  explicit component(std::size_t size_hint = 0, std::size_t seed = 0);
  component(
    std::initializer_list<VertexType> verts, std::size_t size_hint = 0,
    std::size_t seed = 0);

  template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
  explicit component(
    Range&& verts, std::size_t size_hint = 0, std::size_t seed = 0);

  template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
  void insert(Range&& verts);
  void insert(VertexType v);

  void merge(const component& other);

  [[nodiscard]] auto operator==(const component&) const -> bool = default;
  [[nodiscard]] auto size() const -> std::size_t;

  [[nodiscard]] auto contains(VertexType v) const -> bool;
  [[nodiscard]] auto empty() const -> bool;

  [[nodiscard]] auto begin() const -> IteratorType;
  [[nodiscard]] auto end() const -> IteratorType;

private:
  std::unordered_set<VertexType> verts_;
};

class component_sketch {
public:
  using SketchType = hll::hyperloglog<VertexType, 13, 14>;

  explicit component_sketch(std::size_t size_hint = 0, std::size_t seed = 0);
  component_sketch(
    std::initializer_list<VertexType> verts, std::size_t size_hint = 0,
    std::size_t seed = 0);

  template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
  component_sketch(
    Range&& verts, std::size_t size_hint = 0, std::size_t seed = 0);

  template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
  void insert(Range&& verts);
  void insert(VertexType v);

  void merge(const component_sketch& other);

  [[nodiscard]] auto size_estimate() const -> double;

private:
  SketchType verts_;
};
} // namespace reticula

namespace reticula {
template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
component::component(
  Range&& verts, std::size_t size_hint, std::size_t /* seed */)
    : verts_{} {
  if (size_hint == 0) {
    if constexpr (std::ranges::sized_range<Range>)
      verts_.reserve(std::ranges::size(verts));
  } else {
    verts_.reserve(size_hint);
  }

  for (auto&& v : verts)
    verts_.insert(v);
}

template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
void component::insert(Range&& verts) {
  for (auto&& v : verts)
    verts_.insert(v);
}

template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
component_sketch::component_sketch(
  Range&& verts, std::size_t /*size_hint*/, std::size_t seed)
    : verts_(true, seed) {
  for (auto&& v : verts)
    verts_.insert(v);
}

template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertexType>
void component_sketch::insert(Range&& verts) {
  for (auto&& v : verts)
    verts_.insert(v);
}
} // namespace reticula
