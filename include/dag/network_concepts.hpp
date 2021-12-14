#ifndef INCLUDE_DAG_NETWORK_CONCEPTS_HPP_
#define INCLUDE_DAG_NETWORK_CONCEPTS_HPP_

#include <concepts>

namespace dag {
  template <typename T, template<typename> class HashStruct>
  concept hashable_with = requires(T a) {
    { HashStruct<T>{}(a) } -> std::convertible_to<std::size_t>;
  };  // NOLINT(readability/braces)

  template <typename Key> struct hash;

  template <typename T>
  concept network_vertex =
    hashable_with<T, hash> &&
    requires(T a, T b) {
      { a == b } -> std::convertible_to<bool>;
      { a != b } -> std::convertible_to<bool>;
      { a < b } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <typename T>
  concept output_streamable = requires(std::ostream os, T value) {
    { os << value };  // NOLINT(readability/braces)
  };  // NOLINT(readability/braces)

  template <typename T>
  concept input_streamable = requires(std::istream is, T value) {
    { is >> value };  // NOLINT(readability/braces)
  };  // NOLINT(readability/braces)

  template <typename T>
  concept static_edge =
    hashable_with<T, hash> &&
    network_vertex<typename T::VertexType> &&
    requires(const T& a, const T& b) {
      { a == b } -> std::convertible_to<bool>;
      { a != b } -> std::convertible_to<bool>;
      { a < b } -> std::convertible_to<bool>;
      { effect_lt(a, b) } -> std::convertible_to<bool>;
      { adjacent(a, b) } -> std::convertible_to<bool>;
    } && requires(const T& a) {
      { a.mutated_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
      { a.mutator_verts() } ->
        std::convertible_to<std::vector<typename T::VertexType>>;
    } && requires(const T& a, typename T::VertexType v) {
      { a.is_incident(v) } -> std::convertible_to<bool>;
      { a.is_in_incident(v) } -> std::convertible_to<bool>;
      { a.is_out_incident(v) } -> std::convertible_to<bool>;
    };  // NOLINT(readability/braces)

  template <typename T>
  concept temporal_edge =
    static_edge<T> &&
    std::is_arithmetic_v<typename T::TimeType> &&
    requires(const T& a) {
      { a.cause_time() } -> std::convertible_to<typename T::TimeType>;
      { a.effect_time() } -> std::convertible_to<typename T::TimeType>;
      { a.static_projection() } -> static_edge;
    };  // NOLINT(readability/braces)

  template <typename T>
  concept network_edge = static_edge<T> || temporal_edge<T>;
}  // namespace dag

#endif  // INCLUDE_DAG_NETWORK_CONCEPTS_HPP_
