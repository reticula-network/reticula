#ifndef INCLUDE_RETICULA_NETWORK_CONCEPTS_HPP_
#define INCLUDE_RETICULA_NETWORK_CONCEPTS_HPP_

#include <concepts>
#include <tuple>
#include <vector>
#include <istream>
#include <ostream>
#include <iterator>

#include "type_traits.hpp"

namespace reticula {
  template <typename T, template<typename> class HashStruct>
  concept hashable_with = requires(T a) {
    { HashStruct<T>{}(a) } -> std::convertible_to<std::size_t>;
  };  // NOLINT(readability/braces)

  template <typename Key> struct hash;

  template <typename T>
  concept network_vertex =
    std::totally_ordered<T> &&
    hashable_with<T, hash>;

  template <typename T>
  concept integer_vertex =
    network_vertex<T> && std::numeric_limits<T>::is_integer;

  template <typename T>
  concept output_streamable = requires(std::ostream os, T value) {
    { os << value };  // NOLINT(readability/braces)
  };  // NOLINT(readability/braces)

  template <typename T>
  concept input_streamable = requires(std::istream is, T value) {
    { is >> value };  // NOLINT(readability/braces)
  };  // NOLINT(readability/braces)

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
      { a.mutated_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
      { a.mutator_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
      { a.incident_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
    } && requires(const T& a, typename T::VertexType v) {
      { a.is_incident(v) } -> std::convertible_to<bool>;
      { a.is_in_incident(v) } -> std::convertible_to<bool>;
      { a.is_out_incident(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <typename T>
  concept static_edge = network_edge<T> &&
    !requires(const T& a) {  // it has to be more specialized than network_edge
      a.effect_time();
    };  // NOLINT(readability/braces)


  template <typename T>
  concept static_directed_edge =
    static_edge<T> && !is_undirected_v<T>;

  template <typename T>
  concept static_undirected_edge =
    static_edge<T> && is_undirected_v<T>;

  template <typename T>
  concept temporal_edge =
    network_edge<T> &&
    std::is_arithmetic_v<typename T::TimeType> &&
    static_edge<typename T::StaticProjectionType> &&
    requires(const T& a) {
      { a.cause_time() } -> std::convertible_to<typename T::TimeType>;
      { a.effect_time() } -> std::convertible_to<typename T::TimeType>;
      { a.static_projection() } -> std::convertible_to<
        typename T::StaticProjectionType>;
    };  // NOLINT(readability/braces)


  /**
    Degree ranges are sequences of integer numbers
  */
  template <typename Range>
  concept degree_range =
    std::ranges::range<Range> &&
    std::numeric_limits<
      std::ranges::range_value_t<Range>>::is_integer;

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
    std::ranges::range<Range> &&
    is_pairlike_of<std::ranges::range_value_t<Range>,
      std::tuple_element_t<0, std::ranges::range_value_t<Range>>,
      std::tuple_element_t<0, std::ranges::range_value_t<Range>>> &&
    std::numeric_limits<
      std::tuple_element_t<0, std::ranges::range_value_t<Range>>>::is_integer;


  /**
    Weight ranges are sequences of integer or floating point numbers
  */
  template <typename Range>
  concept weight_range =
    std::ranges::range<Range> &&
    std::is_arithmetic_v<
      std::ranges::range_value_t<Range>>;

  /**
    Weight pair ranges are sequences of integer or floating point numbers of the
    same type
  */
  template <typename Range>
  concept weight_pair_range =
    std::ranges::range<Range> &&
    is_pairlike_of<std::ranges::range_value_t<Range>,
      std::tuple_element_t<0, std::ranges::range_value_t<Range>>,
      std::tuple_element_t<0, std::ranges::range_value_t<Range>>> &&
    std::is_arithmetic_v<
      std::tuple_element_t<0, std::ranges::range_value_t<Range>>>;

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
}  // namespace reticula

#endif  // INCLUDE_RETICULA_NETWORK_CONCEPTS_HPP_
