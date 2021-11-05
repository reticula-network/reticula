#ifndef INCLUDE_DAG_EDGES_HPP_
#define INCLUDE_DAG_EDGES_HPP_

#include <concepts>

namespace dag {
  template <typename T, template<typename> typename HashT>
  concept hashable_with_struct = requires(T a) {
    { HashT<T>{}(a) } -> std::convertible_to<std::size_t>;
  };

  template <typename T>
  concept static_edge =
    hashable_with_struct<T, std::hash> &&
    std::is_integral<T::VertexType> &&
    requires(T a, T b) {
      { a == b } -> std::convertible_to<bool>;
      { a != b } -> std::convertible_to<bool>;
      { a < b } -> std::convertible_to<bool>;
      { effect_lt(a, b) } -> std::convertible_to<bool>;
      { adjacent(a, b) } -> std::convertible_to<bool>;
    };
    requires (T a) {
      { a.mutated_verts() } ->
        std::convertible_to<std::vector<T::VertexType>>;
      { a.mutator_verts() } ->
        std::convertible_to<std::vector<T::VertexType>>;
      requires (T::VertexType v) {
        { a.is_incident(v) } -> std::convertible_to<bool>;
        { a.is_in_incident(v) } -> std::convertible_to<bool>;
        { a.is_out_incident(v) } -> std::convertible_to<bool>;
      };
    };

  template <typename T>
  concept temporal_edge =
    static_edge<T> &&
    std::is_arithmatic<T::TimeType> &&
    requires (T a) {
      { a.cause_time() } -> std::convertible_to<T::TimeType>;
      { a.effect_time() } -> std::convertible_to<T::TimeType>;
      { a.static_projection() } -> static_edge;
    };

  template <typename T>
  concept network_edge = static_edge<T> || temporal_edge<T>;
}

#endif  // INCLUDE_DAG_EDGES_HPP_
