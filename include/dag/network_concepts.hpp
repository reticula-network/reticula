#ifndef INCLUDE_DAG_NETWORK_CONCEPTS_HPP_
#define INCLUDE_DAG_NETWORK_CONCEPTS_HPP_

#include <concepts>

namespace dag {
  template <typename T>
  concept hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
  };

  // template <typename T>
  // concept hll_hashable = requires(T a) {
  //   { hll::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
  // };

  template <typename T>
  concept network_vertex = hashable<T>;

  template <typename T>
  concept static_edge =
    hashable<T> &&
    network_vertex<typename T::VertexType> &&
    requires(T a, T b) {
      { a == b } -> std::convertible_to<bool>;
      { a != b } -> std::convertible_to<bool>;
      { a < b } -> std::convertible_to<bool>;
      { effect_lt(a, b) } -> std::convertible_to<bool>;
      { adjacent(a, b) } -> std::convertible_to<bool>;
    } && requires (T a) {
      { a.mutated_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
      { a.mutator_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
    } && requires (T a, typename T::VertexType v) {
      { a.is_incident(v) } -> std::convertible_to<bool>;
      { a.is_in_incident(v) } -> std::convertible_to<bool>;
      { a.is_out_incident(v) } -> std::convertible_to<bool>;
    };

  template <typename T>
  concept temporal_edge =
    static_edge<T> &&
    std::is_arithmetic_v<typename T::TimeType> &&
    requires (T a) {
      { a.cause_time() } -> std::convertible_to<typename T::TimeType>;
      { a.effect_time() } -> std::convertible_to<typename T::TimeType>;
      { a.static_projection() } -> static_edge;
    };

  template <typename T>
  concept network_edge = static_edge<T> || temporal_edge<T>;
}  // namespace dag

#endif  // INCLUDE_DAG_NETWORK_CONCEPTS_HPP_
