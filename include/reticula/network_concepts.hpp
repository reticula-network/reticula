#ifndef INCLUDE_RETICULA_NETWORK_CONCEPTS_HPP_
#define INCLUDE_RETICULA_NETWORK_CONCEPTS_HPP_

#include <concepts>
#include <tuple>
#include <istream>
#include <ostream>
#include <iterator>

#include "ranges.hpp"

namespace reticula {
  template <typename T, template<typename> class HashStruct>
  concept hashable_with = requires(T a) {
    { HashStruct<T>{}(a) } -> std::convertible_to<std::size_t>;
  };  // NOLINT(readability/braces)

  template <typename Key> struct hash;

  template <typename T>
  concept network_vertex =
    std::default_initializable<T> &&
    std::totally_ordered<T> &&
    hashable_with<T, hash>;

  template <typename T>
  concept integer_network_vertex =
    network_vertex<T> && std::numeric_limits<T>::is_integer;

  template <typename T>
  concept output_streamable = requires(std::ostream os, T value) {
    { os << value };  // NOLINT(readability/braces)
  };  // NOLINT(readability/braces)

  template <typename T>
  concept input_streamable = requires(std::istream is, T value) {
    { is >> value };  // NOLINT(readability/braces)
  };  // NOLINT(readability/braces)

  template <typename R, typename V>
  concept vertex_range =
    network_vertex<V> &&
    ranges::forward_range<R> &&
    std::convertible_to<ranges::range_reference_t<R>, const V>;

  template <typename T>
  concept network_edge =
    std::totally_ordered<T> &&
    hashable_with<T, hash> &&
    hashable_with<T, std::hash> &&
    network_vertex<typename T::VertexType> &&
    requires(const T& a, const T& b) {
      { effect_lt(a, b) } -> std::convertible_to<bool>;
      { adjacent(a, b) } -> std::convertible_to<bool>;
    } && requires(const T& a) {
      { a.mutated_verts() } -> vertex_range<typename T::VertexType>;
      { a.mutator_verts() } -> vertex_range<typename T::VertexType>;
      { a.incident_verts() } -> vertex_range<typename T::VertexType>;
    } && requires(const T& a, typename T::VertexType v) {
      { a.is_incident(v) } -> std::convertible_to<bool>;
      { a.is_in_incident(v) } -> std::convertible_to<bool>;
      { a.is_out_incident(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <typename T>
  concept static_network_edge = network_edge<T> &&
    !requires(const T& a) {  // it has to be more specialized than network_edge
      a.effect_time();
    };  // NOLINT(readability/braces)
}  // namespace reticula

namespace reticula {
  template <typename T>
  struct is_instantaneous {
    static constexpr bool value = false;
  };

  template <typename T>
  inline constexpr bool
  is_instantaneous_v = is_instantaneous<T>::value;

  template <typename T>
  struct is_undirected {
    static constexpr bool value = false;
  };

  template <typename T>
  inline constexpr bool
  is_undirected_v = is_undirected<T>::value;
  // Complementary trait capturing directedness as
  // the negation of `is_undirected`

  template <typename T>
  struct is_directed {
    static constexpr bool value = !is_undirected_v<T>;
  };

  template <typename T>
  inline constexpr bool
  is_directed_v = is_directed<T>::value;

  template <typename T>
  struct is_dyadic {
    static constexpr bool value = false;
  };

  template <typename T>
  inline constexpr bool
  is_dyadic_v = is_dyadic<T>::value;

  template <typename T>
  concept directed_network_edge =
    network_edge<T> && !is_undirected_v<T>;

  template <typename T>
  concept undirected_network_edge =
    network_edge<T> && is_undirected_v<T>;

  template <typename T>
  concept directed_static_network_edge =
    static_network_edge<T> && directed_network_edge<T>;

  template <typename T>
  concept undirected_static_network_edge =
    static_network_edge<T> && undirected_network_edge<T>;

  template <typename T>
  concept temporal_network_edge =
    network_edge<T> &&
    std::is_arithmetic_v<typename T::TimeType> &&
    static_network_edge<typename T::StaticProjectionType> &&
    requires(const T& a) {
      { a.cause_time() } -> std::convertible_to<typename T::TimeType>;
      { a.effect_time() } -> std::convertible_to<typename T::TimeType>;
      { a.static_projection() } -> std::convertible_to<
        typename T::StaticProjectionType>;
    };  // NOLINT(readability/braces)

  template <typename T>
  concept directed_temporal_network_edge =
    temporal_network_edge<T> && directed_network_edge<T>;

  template <typename T>
  concept undirected_temporal_network_edge =
    temporal_network_edge<T> && undirected_network_edge<T>;


  /**
    Degree ranges are sequences of integer numbers
  */
  template <typename Range>
  concept degree_range =
    ranges::range<Range> &&
    std::numeric_limits<
      ranges::range_value_t<Range>>::is_integer;

  template <typename T, typename V1, typename V2>
  concept is_pairlike_of =
    std::convertible_to<std::tuple_element_t<0, T>, V1> &&
    std::convertible_to<std::tuple_element_t<1, T>, V2> &&
    requires(T a) {
      requires std::tuple_size<T>::value == 2;
      std::get<0>(a);
      std::get<1>(a);
    };  // NOLINT(readability/braces)

  /**
    Degree pair ranges are sequences of pairs of integer numbers of same type
  */
  template <typename Range>
  concept degree_pair_range =
    ranges::range<Range> &&
    is_pairlike_of<ranges::range_value_t<Range>,
      std::tuple_element_t<0, ranges::range_value_t<Range>>,
      std::tuple_element_t<0, ranges::range_value_t<Range>>> &&
    std::numeric_limits<
      std::tuple_element_t<0, ranges::range_value_t<Range>>>::is_integer;


  /**
    Weight ranges are sequences of integer or floating point numbers
  */
  template <typename Range>
  concept weight_range =
    ranges::range<Range> &&
    std::is_arithmetic_v<
      ranges::range_value_t<Range>>;

  /**
    Weight pair ranges are sequences of integer or floating point numbers of the
    same type
  */
  template <typename Range>
  concept weight_pair_range =
    ranges::range<Range> &&
    is_pairlike_of<ranges::range_value_t<Range>,
      std::tuple_element_t<0, ranges::range_value_t<Range>>,
      std::tuple_element_t<0, ranges::range_value_t<Range>>> &&
    std::is_arithmetic_v<
      std::tuple_element_t<0, ranges::range_value_t<Range>>>;

  /**
    Mappings are ways to provide algorithms with extra information for each
    vertex or edge. `std::map` and `std::unordered_map` both satisfy this
    concept.
  */
  template <typename T, typename KeyT, typename ValueT>
  concept mapping =
    std::indirectly_readable<typename T::iterator> &&
    requires(T map) {
      { map.end() } -> std::sentinel_for<typename T::iterator>;
    } && requires(T map, KeyT k) {
      { map.contains(k) } -> std::convertible_to<bool>;
      { map.find(k) } -> std::convertible_to<typename T::iterator>;
      { map.at(k) } -> std::convertible_to<ValueT>;
    };  // NOLINT(readability/braces)


  /**
    Rebinds an edge type to a new vertex type. This is useful for algorithms
    that need to change the vertex type of edges, e.g., when relabeling vertices.
    The `EdgeTmpl` template parameter is a template that takes a single vertex
    type as its argument, and `OldVertT` and `NewVertT` are the old and new vertex
    types, respectively.
  */
  template<network_edge EdgeT, network_vertex VertT> struct rebind_edge;

  template<template<class, class...> typename EdgeTmpl,
    network_vertex OldVertT, typename... Extra, network_vertex NewVertT>
  struct rebind_edge<EdgeTmpl<OldVertT, Extra...>, NewVertT> {
    using type = EdgeTmpl<NewVertT, Extra...>;
  };

  template<network_edge EdgeT, network_vertex VertT>
  using rebind_edge_t = typename rebind_edge<EdgeT, VertT>::type;

  struct uniform_const_t {};
  inline constexpr uniform_const_t uniform_const{};
}  // namespace reticula

#endif  // INCLUDE_RETICULA_NETWORK_CONCEPTS_HPP_
