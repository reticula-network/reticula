#pragma once

#include <utility>
#include <concepts>
#include <cstdint>
#include <random>
#include <ranges>

namespace hll {
template <typename>
struct hash;
}

namespace reticula {
using VertexType = std::uint64_t;
using TimeType = double;

template <typename>
struct hash;

template <typename>
struct edge_relabel;

template <typename R>
concept vertex_range =
  std::ranges::forward_range<R> &&
  std::convertible_to<std::ranges::range_reference_t<R>, VertexType>;

template <typename R>
concept degree_range =
  std::ranges::forward_range<R> &&
  std::convertible_to<std::ranges::range_reference_t<R>, VertexType>;

template <typename T, typename V1, typename V2>
concept pairlike_of =
  std::convertible_to<std::tuple_element_t<0, T>, V1> &&
  std::convertible_to<std::tuple_element_t<1, T>, V2> && requires(T a) {
    requires std::tuple_size<T>::value == 2;
    std::get<0>(a);
    std::get<1>(a);
  };

template <typename R>
concept degree_pair_range =
  std::ranges::forward_range<R> &&
  std::convertible_to<
    std::ranges::range_reference_t<R>, std::pair<VertexType, VertexType>>;

template <typename T, typename KeyT, typename ValueT>
concept mapping = std::indirectly_readable<typename T::const_iterator> &&
                  requires(const T& map) {
                    {
                      map.end()
                    } -> std::sentinel_for<typename T::const_iterator>;
                  } && requires(const T& map, KeyT k) {
                    { map.contains(k) } -> std::convertible_to<bool>;
                    {
                      map.find(k)
                    } -> std::convertible_to<typename T::const_iterator>;
                    { map.at(k) } -> std::convertible_to<ValueT>;
                  };

template <typename T, template <typename> class HashStruct>
concept hashable_with = requires(T a) {
  { HashStruct<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept network_edge =
  std::totally_ordered<T> && hashable_with<T, std::hash> &&
  hashable_with<T, reticula::hash> && requires(const T& a, const T& b) {
    { effect_cmp(a, b) } -> std::convertible_to<bool>;
    { adjacent(a, b) } -> std::convertible_to<bool>;
  } && requires(const T& a) {
    { a.mutated_verts() } -> vertex_range;
    { a.mutator_verts() } -> vertex_range;
    { a.incident_verts() } -> vertex_range;
    {
      edge_relabel<T>{}(a, [](VertexType v) { return v; })
    } -> std::same_as<T>;
  } && requires(const T& a, VertexType v) {
    { a.is_incident(v) } -> std::convertible_to<bool>;
    { a.is_in_incident(v) } -> std::convertible_to<bool>;
    { a.is_out_incident(v) } -> std::convertible_to<bool>;
  };

template <typename T>
concept static_network_edge =
  network_edge<T> &&
  !requires(const T& a) { // it has to be more specialized than network_edge
    a.effect_time();
  };

template <typename T>
concept temporal_network_edge =
  network_edge<T> && static_network_edge<typename T::StaticProjectionType> &&
  requires(const T& a) {
    { a.cause_time() } -> std::convertible_to<TimeType>;
    { a.effect_time() } -> std::convertible_to<TimeType>;
    {
      a.static_projection()
    } -> std::convertible_to<typename T::StaticProjectionType>;
  };

template <typename R, typename EdgeT>
concept edge_range = std::ranges::forward_range<R> &&
                     std::convertible_to<std::ranges::range_value_t<R>, EdgeT>;

template <typename T>
concept network_like =
  network_edge<typename T::EdgeType> && requires(const T& a) {
    { a.edges() } -> edge_range<typename T::EdgeType>;
    { a.edges_cause() } -> edge_range<typename T::EdgeType>;
    { a.edges_effect() } -> edge_range<typename T::EdgeType>;
    { a.vertices() } -> edge_range<VertexType>;
  } && requires(const T& a, const VertexType v) {
    { a.incident_edges(v) } -> edge_range<typename T::EdgeType>;
    { a.out_edges(v) } -> edge_range<typename T::EdgeType>;
    { a.in_edges(v) } -> edge_range<typename T::EdgeType>;
    { a.successors(v) } -> vertex_range;
    { a.predecessors(v) } -> vertex_range;
    { a.neighbours(v) } -> vertex_range;
  };

template <typename T>
concept static_network_like =
  network_like<T> && static_network_edge<typename T::EdgeType>;

template <typename T>
concept temporal_network_like =
  network_like<T> && temporal_network_edge<typename T::EdgeType>;

template <typename T>
concept dense_id_network =
  network_like<T> && requires(const T& a, VertexType v) {
    { a.vertex_id(v) } -> std::convertible_to<std::size_t>;
  } && requires(const T& a, std::size_t idx) {
    { a.id_vertex(idx) } -> std::convertible_to<VertexType>;
  };

template <typename T, typename Result>
concept random_number_distribution = requires(T dist, std::mt19937_64& gen) {
  { dist(gen) } -> std::convertible_to<Result>;
};

template <typename T>
struct is_instantaneous {
  static constexpr bool value = false;
};

template <typename T>
inline constexpr bool is_instantaneous_v = is_instantaneous<T>::value;

template <typename T>
struct is_undirected {
  static constexpr bool value = false;
};

template <typename T>
inline constexpr bool is_undirected_v = is_undirected<T>::value;

// Complementary trait capturing directedness as
// the negation of `is_undirected`
template <typename T>
struct is_directed {
  static constexpr bool value = !is_undirected_v<T>;
};

template <typename T>
inline constexpr bool is_directed_v = is_directed<T>::value;

template <typename T>
struct is_dyadic {
  static constexpr bool value = false;
};

template <typename T>
inline constexpr bool is_dyadic_v = is_dyadic<T>::value;

// Specializations for network_like types that defer to their EdgeType
template <network_like T>
struct is_instantaneous<T> {
  static constexpr bool value = is_instantaneous_v<typename T::EdgeType>;
};

template <network_like T>
struct is_undirected<T> {
  static constexpr bool value = is_undirected_v<typename T::EdgeType>;
};

template <network_like T>
struct is_dyadic<T> {
  static constexpr bool value = is_dyadic_v<typename T::EdgeType>;
};

template <typename T>
concept directed_network_edge = network_edge<T> && !is_undirected_v<T>;

template <typename T>
concept undirected_network_edge = network_edge<T> && is_undirected_v<T>;

template <typename T>
concept directed_static_network_edge =
  static_network_edge<T> && directed_network_edge<T>;

template <typename T>
concept undirected_static_network_edge =
  static_network_edge<T> && undirected_network_edge<T>;

template <typename T>
concept directed_temporal_network_edge =
  temporal_network_edge<T> && directed_network_edge<T>;

template <typename T>
concept undirected_temporal_network_edge =
  temporal_network_edge<T> && undirected_network_edge<T>;

struct effect_lt_t {
  template <network_edge T>
    requires requires(const T& a, const T& b) { effect_cmp(a, b); }
  constexpr auto operator()(const T& a, const T& b) const
    noexcept(noexcept(effect_cmp(a, b))) -> bool {
    return effect_cmp(a, b);
  }
};

inline constexpr effect_lt_t effect_lt{};
} // namespace reticula
